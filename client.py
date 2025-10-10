import socket
import json
import os
import subprocess
import sys
import multiprocessing
from typing import Dict, Any

# サーバーPCのIPアドレスとポート
SERVER_HOST: str = "172.28.240.1"
SERVER_PORT: int = 8888

# ソルバーで必要なファイルのパス
PROBLEM_PATH: str = "testcase/problem.json"
WEIGHT_PATH: str = "testcase/weights.txt"
INPUT_PROBLEM_PATH: str = "./bin/input_problem.exe"
MAIN_CPP_PATH: str = "./bin/main.exe"
CREATE_ANSWER_JSON_PATH: str = "./bin/create_answer_json.exe"

# ソルバーを実行して解を生成する
def run_solver(weight_line: int) -> Dict[str, Any]:
    # 各プロセスでファイル名が衝突しないように、プロセスIDと重みの行番号を使って一意なファイルパスを生成
    pid = os.getpid()
    answer_path = f"testcase/answer_{pid}_{weight_line}.json"
    print(f"[PID:{pid}] ソルバーを実行中 (WeightLine: {weight_line})...")

    # testcaseディレクトリがなければ作成
    os.makedirs("testcase", exist_ok=True)

    try:
        # 一連のコマンドを実行
        # 各コマンドをリストとして定義
        cmd1 = [INPUT_PROBLEM_PATH, PROBLEM_PATH, WEIGHT_PATH, str(weight_line)]
        cmd2 = [MAIN_CPP_PATH]
        cmd3 = [CREATE_ANSWER_JSON_PATH, answer_path]

        print(f"[PID:{pid}] パイプラインを実行: {' '.join(cmd1)} | {' '.join(cmd2)} | {' '.join(cmd3)}")

        # Popenを使ってパイプラインを構築
        p1 = subprocess.Popen(cmd1, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        p2 = subprocess.Popen(cmd2, stdin=p1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        # p1の標準出力をp2に渡したら、p1のstdoutは閉じる
        if p1.stdout:
            p1.stdout.close()
        p3 = subprocess.Popen(cmd3, stdin=p2.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if p2.stdout:
            p2.stdout.close()

        # 最後のコマンドの出力を取得し、すべてのプロセスが終了するのを待つ
        stdout3, stderr3 = p3.communicate()
        _, stderr2 = p2.communicate()
        _, stderr1 = p1.communicate()

        # 各コマンドの終了コードをチェック
        if p1.returncode != 0:
            raise subprocess.CalledProcessError(p1.returncode, cmd1, stderr=stderr1)
        if p2.returncode != 0:
            raise subprocess.CalledProcessError(p2.returncode, cmd2, stderr=stderr2)
        if p3.returncode != 0:
            raise subprocess.CalledProcessError(p3.returncode, cmd3, stderr=stderr3)

        # 結果ファイルを読み込む
        with open(answer_path, 'r') as f:
            solution = json.load(f)
        
        print(f"[PID:{pid}] ソルバー実行完了")
        return solution

    except FileNotFoundError:
        print(f"[PID:{pid}] エラー：ソルバーの実行ファイルが見つかりません", file=sys.stderr)
        return {}
    except subprocess.CalledProcessError as e:
        # どのコマンドでエラーが発生したかを表示
        failed_command = " ".join(e.cmd)
        print(f"[PID:{pid}] エラー：コマンド '{failed_command}' が終了コード {e.returncode} で失敗しました。", file=sys.stderr)
        if e.stderr:
            print(f"[PID:{pid}] エラー出力：\n{e.stderr}", file=sys.stderr)
        return {}
    except Exception as e:
        print(f"[PID:{pid}] エラー：{e}", file=sys.stderr)
        return {}
    finally:
        # 一時ファイルを削除
        if os.path.exists(answer_path):
            os.remove(answer_path)

# ソルバーを実行し、解が見つかればサーバーに送信する
def run_and_send_solver(weight_line: int) -> None:
    pid = os.getpid()

    # ソルバーを実行
    solution: Dict[str, Any] = run_solver(weight_line)

    # 解があればサーバーに送信
    if solution:
        try:
            print(f"[PID:{pid}] 解をサーバーに送信中...")
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sender_socket:
                sender_socket.connect((SERVER_HOST, SERVER_PORT))
                sender_socket.sendall(json.dumps(solution).encode('utf-8'))
            print(f"[PID:{pid}] 解をサーバーに送信完了")
        except Exception as e:
            print(f"[PID:{pid}] エラー：解の送信に失敗しました - {e}", file=sys.stderr)
    else:
        print(f"[PID:{pid}] 解が生成されませんでした。")

def main() -> None:
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # サーバーに接続
            print(f"{SERVER_HOST}:{SERVER_PORT}に接続中...")
            s.connect((SERVER_HOST, SERVER_PORT))
            print("接続成功。問題受信を待機...")
            
            # サーバーから問題を受け取る
            data: bytes = b""
            while True:
                chunk: bytes = s.recv(8192)
                if not chunk:
                    break
                data += chunk
            if not data:
                print("エラー：サーバーから受信ができませんでした", file=sys.stderr)
                return
            match_info: Dict[str, Any] = json.loads(data.decode("utf-8"))
            print("問題受信に成功")

            # 全プロセスで共通のproblem.jsonを生成
            os.makedirs("testcase", exist_ok=True)
            with open(PROBLEM_PATH, 'w') as f:
                json.dump(match_info, f, indent=4)
            print(f"{PROBLEM_PATH} に試合情報を書き込み成功")

            # CPUコア数に合わせて並列実行
            num_cores = os.cpu_count() or 1
            print(f"{num_cores}コアを使用して並列処理を開始します。")

            # 試行する重みの行番号の範囲 (例: 1から16まで)
            weight_lines = range(1, 17)

            # プロセスプールを作成し、各プロセスでソルバーを実行・送信
            with multiprocessing.Pool(processes=num_cores) as pool:
                # map_asyncはノンブロッキング。各プロセスにタスクを割り当てる
                result = pool.map_async(run_and_send_solver, weight_lines)
                # すべてのタスクが完了するのを待つ
                result.wait()
            
            print("すべての並列処理が完了しました。")
            # 共通のproblem.jsonを削除
            if os.path.exists(PROBLEM_PATH):
                os.remove(PROBLEM_PATH)
                print(f"{PROBLEM_PATH} を削除しました。")

    except Exception as e:
        print(f"エラー：{e}", file=sys.stderr)

if __name__ == "__main__":
    # Windowsでmultiprocessingを使用する際の定型句
    main()
