import glob
import json
import os
import subprocess
import sys
import multiprocessing
import threading
import time
import requests
import copy
from typing import Dict, Any, Optional, Tuple

# --- 定数定義 ---

# サーバー/API関連
API_URL: str = "http://localhost:3000"  # 競技サーバー用APIのURL
TOKEN: str = "player1"  # 認証トークン

# ソルバーで必要なファイルのパス
PROBLEM_PATH: str = "testcase/problem.json"
WEIGHT_PATH: str = "testcase/weights.txt"

# 外部実行ファイルのパス (client.pyから引用)
# 注意: これらのパスは環境に合わせて修正が必要な場合があります
INPUT_PROBLEM_PATH: str = "./bin/input_problem.exe"
MAIN_CPP_PATH: str = "./bin/main.exe"
CREATE_ANSWER_JSON_PATH: str = "./bin/create_answer_json.exe"

# --- グローバル変数 ---

match_info: Optional[Dict[str, Any]] = None  # 試合情報
best_solution: Optional[Dict[str, Any]] = None  # これまでに受け取った最も良い解
best_pair_count: int = 0  # 最も良い解のペア数
best_ops_count: int = 0  # 最も良い解の手数
lock: threading.Lock = threading.Lock()  # グローバル変数を安全に更新するためのロック

# --- ヘルパー関数 (server.py, client.pyから引用・統合) ---

def get_better_solution(sol1: Optional[Dict[str, Any]], sol2: Optional[Dict[str, Any]]) -> Tuple[Optional[Dict[str, Any]], int]:
    """2つの解を比較して、良い方とそのペア数を返す"""
    if sol2 is None:
        sol1_pair_count = sol1.get("pair_count", 0) if sol1 else 0
        return sol1, sol1_pair_count
    if sol1 is None:
        sol2_pair_count = sol2.get("pair_count", 0) if sol2 else 0
        return sol2, sol2_pair_count

    sol1_pair_count = sol1.get("pair_count", 0)
    sol2_pair_count = sol2.get("pair_count", 0)

    if sol1_pair_count > sol2_pair_count:
        return sol1, sol1_pair_count
    if sol2_pair_count > sol1_pair_count:
        return sol2, sol2_pair_count

    # ペア数が同じ場合は手数を比較
    if len(sol1.get("ops", [])) < len(sol2.get("ops", [])):
        return sol1, sol1_pair_count

    return sol2, sol2_pair_count

def run_solver(weight_line: int) -> Optional[Dict[str, Any]]:
    """ソルバーを実行して解を生成する (client.pyのrun_solverを改変)"""
    pid = os.getpid()
    # ファイル名が衝突しないように一意なパスを生成
    answer_path = f"testcase/answer_{pid}_{weight_line}.json"
    problem_txt_path = f"testcase/problem_{pid}_{weight_line}.txt"
    answer_txt_path = f"testcase/answer_{pid}_{weight_line}.txt"

    print(f"[Worker PID:{pid}] ソルバー実行開始 (WeightLine: {weight_line})")

    try:
        # 1. input_problem.exe: problem.json -> problem.txt
        cmd1 = f"{INPUT_PROBLEM_PATH} {PROBLEM_PATH} {WEIGHT_PATH} {weight_line} > {problem_txt_path}"
        subprocess.run(cmd1, shell=True, check=True, capture_output=True, text=True, timeout=60)

        # 2. main.exe: problem.txt -> answer.txt
        cmd2 = f"{MAIN_CPP_PATH} < {problem_txt_path} > {answer_txt_path}"
        subprocess.run(cmd2, shell=True, check=True, capture_output=True, text=True, timeout=600)

        # 3. create_answer_json.exe: answer.txt -> answer.json
        cmd3 = f"{CREATE_ANSWER_JSON_PATH} {answer_path} < {answer_txt_path}"
        subprocess.run(cmd3, shell=True, check=True, capture_output=True, text=True, timeout=60)

        print(f"[Worker PID:{pid}] コマンド実行完了 (WeightLine: {weight_line})")

        # 結果ファイルを読み込む
        with open(answer_path, 'r') as f:
            solution = json.load(f)
        
        print(f"[Worker PID:{pid}] ソルバー実行完了 (WeightLine: {weight_line})")
        return solution

    except FileNotFoundError as e:
        print(f"[Worker PID:{pid}] エラー: 実行ファイルが見つかりません: {e.filename}", file=sys.stderr)
        return None
    except subprocess.CalledProcessError as e:
        print(f"[Worker PID:{pid}] エラー: コマンド '{e.cmd}' が失敗しました (コード: {e.returncode})", file=sys.stderr)
        if e.stderr:
            print(f"エラー出力:\n{e.stderr}", file=sys.stderr)
        return None
    except Exception as e:
        print(f"[Worker PID:{pid}] 予期せぬエラー: {e}", file=sys.stderr)
        return None
    finally:
        # 一時ファイルを削除
        for path in [answer_path, problem_txt_path, answer_txt_path]:
            if os.path.exists(path):
                os.remove(path)

def fetch_match_info() -> bool:
    """競技サーバーから試合情報を取得し、グローバル変数に格納する"""
    global match_info
    try:
        print(f"{API_URL}/ から試合情報を取得中...")
        headers: Dict[str, str] = {"Procon-Token": TOKEN}
        response: requests.Response = requests.get(f"{API_URL}/", headers=headers)
        response.raise_for_status()
        data: Dict[str, Any] = response.json()
        with lock:
            match_info = data
        print("試合情報の取得に成功しました。")
        return True
    except requests.exceptions.RequestException as e:
        print(f"試合情報の取得に失敗しました: {e}", file=sys.stderr)
        return False

def submit_to_official_server(solution_to_submit: Dict[str, Any]):
    """解を競技サーバーに提出する"""
    if not match_info:
        print("エラー: 試合情報がありません。提出できません。", file=sys.stderr)
        return
    
    print("現在の最良解を競技サーバーに提出します...")
    headers: Dict[str, str] = {"Procon-Token": TOKEN}
    try:
        response = requests.post(f"{API_URL}/", json=solution_to_submit, headers=headers)
        if response.status_code == 200:
            response_data = response.json()
            revision = response_data.get("revision", -1)
            print(f"回答が受理されました (受理番号: {revision})")
        else:
            print(f"提出エラー: ステータスコード {response.status_code}", file=sys.stderr)
            print(f"エラー内容: {response.text}", file=sys.stderr)
    except requests.exceptions.RequestException as e:
        print(f"提出中に通信エラーが発生しました: {e}", file=sys.stderr)

def wait_for_match_start(info: Optional[Dict[str, Any]]):
    """試合開始時刻まで待機する"""
    start_at_unix = info.get("startsAt", 0)
    current_unix = int(time.time())
    wait_time = start_at_unix - current_unix
    if wait_time > 0:
        print(f"試合開始まで {wait_time} 秒待機します...")
        time.sleep(wait_time)
    print("試合開始！")

def update_best_solution(solution: Optional[Dict[str, Any]]):
    """ワーカープロセスからの解を受け取り、最良解を更新するコールバック関数"""
    if not solution:
        return

    with lock:
        global best_solution, best_pair_count, best_ops_count
        new_best, new_pair_count = get_better_solution(best_solution, solution)

        if new_best is not best_solution:
            best_solution = new_best
            best_pair_count = new_pair_count
            best_ops_count = len(new_best.get('ops', []))
            # ユーザー入力行を上書きしないように、カーソルを先頭に戻してから出力
            print(f"\r{' ' * 80}\r", end='')
            print(f"新しい最良解を発見！ (ペア数: {best_pair_count}, 手数: {best_ops_count})")
            print("\nコマンド > ", end='', flush=True) # プロンプトを再表示

def print_help():
    """利用可能なコマンドを表示する"""
    print("\n利用可能なコマンド:")
    print("  submit: 現時点の最良解を競技サーバーに提出します。")
    print("  status: 現時点の最良解のペア数と手数を表示します。")
    print("  help  : このヘルプメッセージを表示します。")
    print("  exit  : プログラムを終了します。")

# --- メイン処理 ---

def main():
    """メイン関数"""
    global best_solution

    # 1. 試合情報を取得
    if not fetch_match_info():
        print("プログラムを終了します。", file=sys.stderr)
        return

    # 2. problem.json を作成
    os.makedirs("testcase", exist_ok=True)
    with open(PROBLEM_PATH, 'w') as f:
        json.dump(match_info, f, indent=4)
    print(f"'{PROBLEM_PATH}' に試合情報を書き込みました。")

    # 3. 試合開始まで待機
    wait_for_match_start(match_info)

    # 4. 並列処理でソルバーを実行
    # メインプロセス用に1コア空ける
    num_workers = max(1, (os.cpu_count() or 1) - 1)
    print(f"{num_workers}個のワーカープロセスを使用して並列処理を開始します。")

    # 試行する重みの行番号の範囲 (例: 1から16まで)
    # TODO: weights.txt の行数に合わせて調整してください
    weight_lines = range(1, 17)

    pool = multiprocessing.Pool(processes=num_workers)
    for line in weight_lines:
        pool.apply_async(run_solver, args=(line,), callback=update_best_solution)
    
    pool.close() # 新しいタスクの受付を終了

    # 5. ユーザーからのコマンド入力を処理
    print_help()
    print("ソルバーがバックグラウンドで実行中です。")
    try:
        while True:
            user_input = input("\nコマンド > ").lower().strip()
            if user_input == "submit":
                with lock:
                    if best_solution is None:
                        print("\n最良解はまだ見つかっていません。")
                    else:
                        # 提出中にbest_solutionが更新されないようにディープコピー
                        solution_for_submission = copy.deepcopy(best_solution)
                        submit_to_official_server(solution_for_submission)
            elif user_input == "status":
                with lock:
                    if best_solution is None:
                        print("\n最良解はまだ見つかっていません。")
                    else:
                        print(f"\n現在の最良解: ペア数={best_pair_count}, 手数={best_ops_count}")
            elif user_input == "help":
                print_help()
            elif user_input == "exit":
                print("\nプログラムを終了します...")
                break
            else:
                print("\n不明なコマンドです。'help'でコマンド一覧を確認してください。")
    except (KeyboardInterrupt, EOFError):
        print("\nプログラムを終了します...")
    finally:
        # 6. クリーンアップ
        print("ワーカープロセスを終了しています...")
        pool.terminate() # 実行中のタスクを強制終了
        pool.join()
        print("すべてのプロセスが終了しました。")
        if os.path.exists(PROBLEM_PATH):
            os.remove(PROBLEM_PATH)
            print(f"'{PROBLEM_PATH}' を削除しました。")
        for path in glob.glob("testcase/answer_*.json") + glob.glob("testcase/problem_*.txt") + glob.glob("testcase/answer_*.txt"):
            os.remove(path)
            print(f"'{path}' を削除しました。")

if __name__ == "__main__":
    # Windowsでmultiprocessingを使用する際の定型句
    multiprocessing.freeze_support()
    main()