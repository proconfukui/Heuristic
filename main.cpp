#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <set>
#include <fstream>
#include <queue>
#include <algorithm>
#include "utils.hpp"
#include "base.hpp"
#include "evalution.hpp"
#include "steps_table.hpp"
#include "json.hpp"
#include "solver.hpp"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::function;
using std::make_pair;
using std::min;
using std::ofstream;
using std::pair;
using std::priority_queue;
using std::rand;
using std::set;
using std::setw;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

void initialize(int &start_time, vector<vector<int>> &field, vector<int> &weights);
void print_answer(int time, const vector<Operation> &ops, const Field& field);
vector<Operation> beam_search(const Field& field, vector<int> weights, int depth, int width, int commit_step, int num_sample, int max_time, int random_injection_ratio, const function<bool(const Field&)>& judge, const function<int(const Field&)>& evaluator);
vector<Operation> best_operations_random(const Field& field, int num_sample, int width, const function<int(const Field&)>& evaluator);
vector<Operation> best_operations_random2(const Field& field, int num_sample, int width, int random_injection_ratio, const function<int(const Field&)>& evaluator);
vector<Operation> a_star(const Field& field, const vector<int>& weights, const function<int(const Field&)>& evaluator);
pair<vector<Operation>, int> solve_outer_layer(Field& field, int change_point);


/**
 * @brief フィールドの外周2マスを確定させる処理
 * @param field 対象のフィールド（この関数内で変更されます）
 * @param change_point このサイズ以下になったら処理を終了する
 * @return 外周を揃えるための一連の操作
 */
pair<vector<Operation>, int> solve_outer_layer(Field& field, int change_point) {
    vector<Operation> ops;
    int initial_field_size = field.size;
    int final_layer = 0;

    for (int layer = 0; (initial_field_size - 2 * layer) > change_point; layer += 2) {
        int current_size = initial_field_size - 2 * layer;
        Field current_field = cut_field(field, layer, layer, current_size);

        // 上2行を揃える
        for (int y = 0; y < 2; ++y) {
            for (int x = (y == 0) ? 1 : 0; x < current_size - 1; x += 2) {
                vector<Operation> tmp_ops = calculate_shortest_moves_with_obstacles(current_field, 1, x, y);
                apply_ops(current_field, tmp_ops);
                vector<Operation> corrected_ops = correct_op(tmp_ops, layer, layer);
                ops.insert(ops.end(), corrected_ops.begin(), corrected_ops.end());
            }
        }
        // 左2列を揃える
        for (int x = 0; x < 2; ++x) {
            // 上2行はすでに揃っているので、y=2から始める
            for (int y = (x == 0) ? 3 : 2; y < current_size; y += 2) {
                vector<Operation> tmp_ops = calculate_shortest_moves_with_obstacles(current_field, 2, x, y);
                apply_ops(current_field, tmp_ops);
                vector<Operation> corrected_ops = correct_op(tmp_ops, layer, layer);
                ops.insert(ops.end(), corrected_ops.begin(), corrected_ops.end());
            }
        }
        final_layer = layer + 2;
        field = cut_field(current_field, 2, 2, current_size - 2);
    }

    return {ops, final_layer};
}

int main()
{
  std::ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  // 標準入力から変数へ代入
  int start_time;
  vector<vector<int>> field_2d;
  vector<int> weights;
  initialize(start_time, field_2d, weights);
  Field field(field_2d);
  initialize_evalutor(field,weights);
  int field_size = field.size;

  // print_analysisで正しく解析できるよう、初期盤面を保持しておく
  Field original_field = field;

  // 処理の本体。時間を計測する
  auto begin_time = std::chrono::high_resolution_clock::now();
  // print_matrix(field);
  vector<Operation> answer;

  int change_point = 14; 
  int remaining_field_offset = 0;

  if (field_size > change_point) {
      pair<vector<Operation>, int> outer_result = solve_outer_layer(field, change_point);
      answer.insert(answer.end(), outer_result.first.begin(), outer_result.first.end());
      remaining_field_offset = outer_result.second;
  }
  Field remaining_field = cut_field(field, remaining_field_offset, remaining_field_offset, field_size - 2 * remaining_field_offset);

  // 残った中央部分をビームサーチで解く
  vector<Operation> answer2 = beam_search(remaining_field, weights, 200, 100, 2, 400, 100, 5, [&](const Field& f){
      return  100 * count_pair(f) / (field_size * field_size / 2) > 90 ;
  },
      [&weights](const Field& f){ return count_pair(f) * weights[0] - measure_distance(f); });
  apply_ops(remaining_field, answer2);
  vector<Operation> corrected_ops2 = correct_op(answer2, remaining_field_offset, remaining_field_offset);

  vector<Operation> answer3 = beam_search(remaining_field, weights, 200, 70, 2, 600, 100, 5, [&](const Field& f){
      return  100 * count_pair(f) / (field_size * field_size / 2) ==100 ;
  },
      [&weights](const Field& f){ return count_pair(f) * weights[0] - measure_distance(f); });
  vector<Operation> corrected_ops3 = correct_op(answer3, remaining_field_offset, remaining_field_offset);

  answer.insert(answer.end(), corrected_ops2.begin(), corrected_ops2.end());
  answer.insert(answer.end(), corrected_ops3.begin(), corrected_ops3.end());
// ---------------------------------------------------------------------------------------------------------------------------------

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
  // cerr << "exe_time: " << duration.count() << " ms" << endl;
  // print_matrix(field);
  print_answer(start_time, answer, original_field);
}

// ビームサーチ
vector<Operation> beam_search(const Field& field, vector<int> weights, int depth, int width, int commit_step, int num_sample, int max_time, int random_injection_ratio, const function<bool(const Field&)>& judge, const function<int(const Field&)>& evaluator)
{
  Field tmp_field = field;
  int tmp_field_size = tmp_field.size;
  int max_pair_number = tmp_field_size * tmp_field_size / 2;

  vector<Operation> answer = {};
  auto start_time_total = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < max_time; i++)
  {
    cerr << "time :" << i * commit_step << endl;
    auto begin_time = std::chrono::high_resolution_clock::now();

    // 各探索サイクルの開始時の盤面
    auto cycle_start_field_ptr = std::make_shared<const Field>(tmp_field);

    // nodes[depth] にその深さでの候補手(BeamNode)を格納する
    vector<vector<BeamNode>> nodes(depth);

    // depth 0: 最初の盤面から手を生成
    vector<Operation> initial_candidates = best_operations_random2(*cycle_start_field_ptr, num_sample, width, random_injection_ratio, evaluator);
    for(const auto& op : initial_candidates) {
        auto next_field_ptr = std::make_shared<Field>(*cycle_start_field_ptr);
        rotate(*next_field_ptr, op);
        nodes[0].emplace_back(op, evaluator(*next_field_ptr), -1, std::const_pointer_cast<const Field>(next_field_ptr));
    }

    for (int d = 0; d < depth - 1; d++)
    {
        priority_queue<pair<int, BeamNode>> next_candidates_pq;

        for (int p_idx = 0; p_idx < nodes[d].size(); ++p_idx) {
            const auto& parent_node = nodes[d][p_idx];

            // 親ノードの盤面状態を取得
            const auto& parent_field_ptr = parent_node.field_ptr;

            // 次の手を生成・評価
            vector<Operation> candidates = best_operations_random2(*parent_field_ptr, num_sample, width, random_injection_ratio, evaluator);
            for (const auto& op : candidates) {
                auto next_field_ptr = std::make_shared<Field>(*parent_field_ptr);
                rotate(*next_field_ptr, op);
                int score = evaluator(*next_field_ptr);
                next_candidates_pq.push({score, BeamNode(op, score, p_idx, std::const_pointer_cast<const Field>(next_field_ptr))});
            }
        }

        // 上位width個を次の層のノードとして採用
        for (int k = 0; k < width && !next_candidates_pq.empty(); ++k) {
            nodes[d+1].push_back(next_candidates_pq.top().second);
            next_candidates_pq.pop();
        }
    }

    // 最も評価の高い操作系列を見つける
    int best_final_node_idx = -1;
    int max_score = -1e9;
    if (!nodes.back().empty()) {
        for(int i=0; i<nodes.back().size(); ++i) {
            if(nodes.back()[i].score > max_score) {
                max_score = nodes.back()[i].score;
                best_final_node_idx = i;
            }
        }
    }

    if (best_final_node_idx == -1) {
        cerr << "no candidate nodes; terminate beam." << endl;
        return answer;
    }

    // 最良の操作系列を復元
    vector<Operation> best_ops;
    int current_p_idx = best_final_node_idx;
    for(int d = depth - 1; d >= 0; --d) {
        best_ops.push_back(nodes[d][current_p_idx].op);
        current_p_idx = nodes[d][current_p_idx].parent_index;
    }
    std::reverse(best_ops.begin(), best_ops.end());

    // commit_step分だけ局面を進める
    for (int j = 0; j < commit_step && j < best_ops.size(); j++)
    {
      answer.push_back(best_ops[j]);
      rotate(tmp_field, best_ops[j]);
      cerr << 100 * count_pair(tmp_field) / max_pair_number << "%" << endl;
      print_matrix(tmp_field);
      if (judge(tmp_field))
      {     
        cerr << "end this search" << endl;
        return answer;
      }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
    cerr << "exe_time: " << duration.count() << " ms" << endl;

    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_total).count();
    if (total_duration > 180000) { // 3分でタイムアウト
        cerr << "Timeout. Terminating search." << endl;
        return answer;
    }
  }
  cerr << "end this search" << endl;
  return answer;
}

// その局面での全ての手を返す
vector<Operation> select_all_operation(const Field& field)
{

  int field_size = field.size;
  vector<Operation> candidates;
  // おおよその手数: sum_{n=2..N} (N-n+1)^2 = sum_{k=1..N-1} k^2 = N(N-1)(2N-1)/6
  if (field_size >= 2)
  {
    int approx = field_size * (field_size - 1) * (2 * field_size - 1) / 6;
    if (approx > 0)
      candidates.reserve(approx);
  }
  for (int y = 0; y < field_size - 1; y++)
  {
    for (int x = 0; x < field_size - 1; x++)
    {
      for (int n = 2; x + n <= field_size && y + n <= field_size; n++)
      {
        Operation op = {x, y, n};
        candidates.push_back(op);
      }
    }
  }
  return candidates;
}

// 2.2 ランダムにの手の評価値を計算し、上位width手を返す
vector<Operation> best_operations_random(const Field& field, int num_sample, int width, const function<int(const Field&)>& evaluator)
{
  int field_size = field.size;
  // 取りうる全ての手とサンプル数を比べ少ないほうを選ぶ
  num_sample = min(num_sample, steps_table[field_size]);

  Field tmp_field = field;

  set<pair<int, Operation>> candidates;
  while (candidates.size() < num_sample)
  {
    int n = rand_int(2, field_size);
    int x = rand_int(0, field_size - n), y = rand_int(0, field_size - n);
    if (n == field_size && x == 0 && y == 0)
      continue;
    Operation op = {x, y, n};

    rotate(tmp_field, op);
    candidates.insert(make_pair(evaluator(tmp_field), op));
    unrotate(tmp_field, op);
  }

  vector<Operation> result;
  auto it = candidates.rbegin();
  for (int i = 0; i < width && it != candidates.rend(); ++i, ++it)
  {
    result.push_back(it->second);
  }
  return result;
}

// 2.2 別アルゴリズム版: 全手を列挙→シャッフル→上からnum_sample個を評価し、上位width手を返す
vector<Operation> best_operations_random2(const Field& field, int num_sample, int width, int random_injection_ratio, const function<int(const Field&)>& evaluator)
{
    const int field_size = field.size;
    priority_queue<pair<int, Operation>, vector<pair<int, Operation>>, std::greater<pair<int, Operation>>> top_k_ops;

    Field tmp_field = field; // オブジェクトのコピーを一度だけ行う

    // num_sample回、ランダムな操作を生成して評価
    for (int i = 0; i < num_sample; ++i) {
        int n = rand_int(2, field_size);
        int x = rand_int(0, field_size - n);
        int y = rand_int(0, field_size - n);
        Operation op = {x, y, n};

        rotate(tmp_field, op);
        int sc = evaluator(tmp_field);
        unrotate(tmp_field, op); // 適用した操作を元に戻す

        if (top_k_ops.size() < static_cast<size_t>(width)) {
            top_k_ops.push({sc, op});
        } else if (sc > top_k_ops.top().first) {
            top_k_ops.pop();
            top_k_ops.push({sc, op});
        }
    }

    // ランダムな手を注入
    int random_injection_count = (width * random_injection_ratio) / 100;
    for(int i = 0; i < random_injection_count && !top_k_ops.empty(); ++i) {
        top_k_ops.pop(); // スコアの低いものを削除
        
        // 新しいランダムな操作を生成
        int n = rand_int(2, field_size);
        int x = rand_int(0, field_size - n);
        int y = rand_int(0, field_size - n);
        Operation op = {x, y, n};

        rotate(tmp_field, op);
        int sc = evaluator(tmp_field);
        unrotate(tmp_field, op); // 元に戻す
        top_k_ops.push({sc, op});
    }

    vector<Operation> result;
    result.reserve(width);
    while(!top_k_ops.empty()) {
        result.push_back(top_k_ops.top().second);
        top_k_ops.pop();
    }
    std::reverse(result.begin(), result.end());

    return result;
}

vector<Operation> a_star(const Field& field, const vector<int>& weights, const function<int(const Field&)>& evaluator)
{
  int field_size = field.size;
  priority_queue<State> pq;
  unordered_set<size_t> visited; // 状態のハッシュ値で管理

  int h0 = -evaluator(field); // スコアをコストに変換
  pq.push(State{field, {}, 0, h0, h0});

  int max_pair_number = field_size * field_size / 2;
  // セーフガード
  const auto start = std::chrono::high_resolution_clock::now();
  const int64_t time_limit_ms = 1800; // 1.8秒程度で強制打ち切り（WSL切断対策）
  const size_t expand_limit = 200000; // 展開ノード上限
  size_t expanded = 0;
  // ベスト暫定解
  vector<Operation> best_ops;
  int best_pairs = count_pair(field);

  while (!pq.empty())
  {
    State cur = pq.top();
    pq.pop();

    // ゴール判定（例: 全ペア揃ったか）: 現在ノードの盤面で判定する
    int pairs = count_pair(cur.field);
    if (pairs > best_pairs)
    {
      best_pairs = pairs;
      best_ops = cur.ops;
    }
    if (pairs == max_pair_number)
    {
      return cur.ops;
    }

    // 状態のハッシュ化（例: fieldの内容をハッシュ化）
    unsigned long hash = hash_field(cur.field);
    if (visited.count(hash))
      continue;
    visited.insert(hash);

    // 可能な操作を列挙
    for (const Operation &op : select_all_operation(cur.field))
    {
      Field next_field = cur.field;
      rotate(next_field, op);

      int g = cur.g + 1; // 1手進めた
      int h = -evaluator(next_field); // スコアをコストに変換
      vector<Operation> next_ops = cur.ops;
      next_ops.push_back(op);

      pq.push(State{next_field, next_ops, g, h, g + h});
      expanded++;
      // セーフガード判定
      if (expanded % 2048 == 0)
      {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        if (elapsed > time_limit_ms || expanded > expand_limit)
        {
          return best_ops; // 暫定ベストを返す
        }
      }
    }
  }
  // 解が見つからない場合
  return best_ops;
}

// 入力用関数
// テスト済
void initialize(int &start_time, vector<vector<int>> &field, vector<int> &weights)
{
  cin >> start_time;
  int field_size;
  cin >> field_size;
  for (int i = 0; i < field_size; i++)
  {
    vector<int> row(field_size);
    for (int j = 0; j < field_size; j++)
    {
      cin >> row[j];
    }
    field.push_back(row);
  }
  int weight_size;
  cin >> weight_size;
  int value;
  for (int i = 0; i < weight_size; i++)
  {
    cin >> value;
    weights.push_back(value);
  }
}

// 出力用関数
// 未テスト
void print_answer(int time, const vector<Operation> &ops, const Field& field)
{
  int field_size = field.size;
  cout << time << endl;
  cout << ops.size() << endl;
  for (const auto &op : ops)
  {
    cout << op.n << " " << op.x << " " << op.y << endl;
  }
  cout << field_size << endl;
  for (int y = 0; y < field_size; ++y)
  {
    for (int x = 0; x < field_size; ++x)
    {
      cout << field.at(y, x) << " ";
    }
    cout << endl;
  }
}
