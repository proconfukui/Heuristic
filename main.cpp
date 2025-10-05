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
#include "utils.hpp"
#include "base.hpp"
#include "evalution.hpp"
#include "steps_table.hpp"
#include "json.hpp"

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

void initialize(int &start_time, vector<vector<int>> &field, vector<float> &weights);
void print_answer(int time, const vector<Operation> &ops, const vector<vector<int>> &field);
vector<Operation> beam_search(const vector<vector<int>> &field, vector<float> weights, int depth, int width, int commit_step, int num_sample, float tarm_ratio, const function<float(vector<vector<int>> &)> &evaluator);
vector<Operation> best_operations_random(const vector<vector<int>> &field, int num_sample, int width, const function<float(vector<vector<int>> &)> &evaluator);
vector<Operation> a_star(vector<vector<int>> field,const function<float(vector<vector<int>> &)> &evaluator);

int main()
{
  std::ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  // 標準入力から変数へ代入
  int start_time;
  vector<vector<int>> field;
  vector<float> weights;
  initialize(start_time, field, weights);
  initialize_evalutor(field, weights);
  // print_analysisで正しく解析できるよう、初期盤面を保持しておく
  vector<vector<int>> original_field = field;

  // weights_matrixのテスト用
  //  func1(field);

  // 処理の本体。時間を計測する
  auto begin_time = std::chrono::high_resolution_clock::now();

  // ビームサーチによる探索
  vector<Operation> answer;

  vector<Operation> answer1 = beam_search(field, weights, 20, 20, 7, 100, 0.30, func1);

  for(const auto& op : answer1){
    rotate(field,op);
  }

  vector<Operation> answer2 = beam_search(field, weights, 20, 30, 7, 100, 0.95, [](const vector<vector<int>>& field){
    return count_pair(field) - measure_distance(field) + 0.0;
  });
  
  for(const auto& op : answer2){
    rotate(field,op);
  }

  // 幅、深さ双方を大きくする代わりに評価関数をペア数のみだけにする
  vector<Operation> answer3 = beam_search(field, weights, 50, 50, 7, 300, 1.0, [](const vector<vector<int>>& field){
    return count_pair(field) + 0.0;
  });


  answer1.insert(answer1.end(),answer2.begin(),answer2.end());

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
  cerr << "exe_time: " << duration.count() << " ms" << endl;

  // float max_pair_number = field.size() * field.size() / 2;
  // 解答をターミナルに表示する
  // for(const auto& op : answer){
  //   rotate(field,op);
  //   cout << count_pair(field) << endl;
  //   cout << "closed ratio " << static_cast<float>(count_pair(field)) / max_pair_number * 100<< "%" << endl;
  //   print_matrix(field);
  // }
  // export_answer("./testcase/answer.json",answer);

  // 解析ツール（print_analysis）は「初期盤面」に対して手順を順に適用して評価を出すため
  // 出力時の盤面は初期盤面を渡す
  print_answer(start_time, answer1, original_field);
}

// ビームサーチ（最適化版 - moveセマンティクス使用）
vector<Operation> beam_search(const vector<vector<int>> &field, vector<float> weights, int depth, int width, int commit_step, int num_sample, float tarm_ratio, const function<float(vector<vector<int>> &)> &evaluator)
{
  vector<vector<int>> tmp_field = field;

  // 最大のペア数
  int max_pair_number = tmp_field.size() * tmp_field.size() / 2;

  // 解答用の配列
  vector<Operation> answer = {};

  // 使う配列を事前に宣言
  vector<BeamNode> nodes;
  vector<BeamNode> next_nodes;
  vector<Operation> candidates;

  // メモリを事前確保
  next_nodes.reserve(depth * num_sample);

  while(true)
  {
    // commit_step回のステップにどれほど時間がかかるかを計測
    cerr << "time :" << time << endl;
    auto begin_time = std::chrono::high_resolution_clock::now();

    // 初期場面のノードを代入
    nodes.emplace_back(tmp_field, vector<Operation>{}, evaluator(tmp_field));

    for (int d = 0; d < depth; d++)
    {
      // next_nodeに新しい生成されるnodeを作る
      for (const auto &node : nodes)
      {
        candidates = best_operations_random(node.field, num_sample, width, evaluator);
        for (const auto &op : candidates)
        {
          vector<vector<int>> work_field = node.field; // コピーを作成
          rotate(work_field, op);
          float score = evaluator(work_field);
          vector<Operation> new_ops = node.ops;
          new_ops.push_back(op);
          next_nodes.emplace_back(std::move(work_field), std::move(new_ops), score);
        }
      }

      // next_nodeから評価値の高いwidth個のnodeを抽出
      if (next_nodes.size() > width)
      {
        partial_sort(next_nodes.begin(), next_nodes.begin() + width, next_nodes.end(),
                     [](const BeamNode &a, const BeamNode &b)
                     {
                       return a.score > b.score;
                     });
        next_nodes.resize(width);
      }
      nodes = std::move(next_nodes);
      next_nodes.clear();
    }

    // commit_step分だけ局面を動かす
    const auto &best = nodes.front();
    for (int i = 0; i < commit_step; i++)
    {
      answer.push_back(best.ops[i]);
      rotate(tmp_field, best.ops[i]);
      // 全てのペアが完成した時点でゲーム終了
      // ペアの割合がterm_ratioを超えた段階でビームサーチを終える
      if(static_cast<float>(count_pair(tmp_field))/max_pair_number >= tarm_ratio){
        cerr << "end this search" << endl;
        return answer;
      }
    }

    nodes.clear();

    // commit_step回のステップにどれほど時間がかかるかを計測
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
    cerr << "exe_time: " << duration.count() << " ms" << endl;
    // timelocal.push_back(duration.count());
  }
}

// その局面での全ての手を返す
vector<Operation> select_all_operation(const vector<vector<int>> &field)
{

  int field_size = field.size();
  vector<Operation> candidates;
  candidates.reserve(sizeof(Operation) * steps_table[field_size]);
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
vector<Operation> best_operations_random(const vector<vector<int>> &field, int num_sample, int width, const function<float(vector<vector<int>> &)> &evaluator)
{
  int field_size = field.size();
  // 取りうる全ての手とサンプル数を比べ少ないほうを選ぶ
  num_sample = min(num_sample, steps_table[field_size]);

  vector<vector<int>> tmp_field = field;

  set<pair<float, Operation>> candidates;
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

vector<Operation> a_star(vector<vector<int>> field,const function<float(vector<vector<int>> &)> &evaluator)
{
  priority_queue<State> pq;
  unordered_set<size_t> visited; // 状態のハッシュ値で管理

  float h0 = evaluator(field);
  pq.push(State{field, {}, 0, h0, h0});

  int max_pair_number = field.size() * field.size() / 2;
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
    if (pairs > best_pairs) {
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
      vector<vector<int>> next_field = cur.field;
      rotate(next_field, op);

      float g = cur.g + 1; // 1手進めた
      float h = evaluator(next_field);
      vector<Operation> next_ops = cur.ops;
      next_ops.push_back(op);

      pq.push(State{next_field, next_ops, g, h, g + h});
      expanded++;
      // セーフガード判定
      if (expanded % 2048 == 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        if (elapsed > time_limit_ms || expanded > expand_limit) {
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
void initialize(int &start_time, vector<vector<int>> &field, vector<float> &weights)
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
  float value;
  for (int i = 0; i < weight_size; i++)
  {
    cin >> value;
    weights.push_back(value);
  }
}

// 出力用関数
// 未テスト
void print_answer(int time, const vector<Operation> &ops, const vector<vector<int>> &field)
{
  cout << time << endl;
  cout << ops.size() << endl;
  for (const auto &op : ops)
  {
    cout << op.n << " " << op.x << " " << op.y << endl;
  }
  cout << field.size() << endl;
  for (const auto &row : field)
  {
    for (const auto &element : row)
    {
      cout << element << " ";
    }
    cout << endl;
  }
}
