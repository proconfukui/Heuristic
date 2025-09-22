#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>
#include <random>
#include <unordered_map>
#include <chrono>
#include "utils.hpp"
#include "base.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::function;
using std::pair;
using std::rand;
using std::setw;
using std::vector;
using std::unordered_map;

void initialize(int &start_time, vector<vector<int>> &field, vector<float> &weights);
void print_answer(int time, int step, const vector<Operation> &ops, const vector<vector<int>> &field);
void beam_search(vector<vector<int>> &field, function<float(vector<vector<int>> &)> &evaluator, int depth, int width, int commit_step, int num_sample);


int main()
{
  std::ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int start_time;
  vector<vector<int>> field;
  vector<float> weights;
  initialize(start_time, field, weights);
  print_field(field);

  auto begin_time = std::chrono::high_resolution_clock::now();
  for(int i = 0;i< 1000000;i++){
    count_pair(field);
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
  cout << "exe_time: " << duration.count() << " ms" << endl;

  cout << count_pair(field) << endl;
  print_field(field);
}

// ビームサーチ
// 未テスト
void beam_search(vector<vector<int>> &field, function<float(vector<vector<int>> &)> &evaluator, int depth, int width, int commit_step,int num_sample)
{

  // 最大のペア数
  int max_pair_number = field.size() * field.size() / 2;

  // 使う配列を事前に宣言
  vector<BeamNode> nodes;
  vector<BeamNode> next_nodes;
  vector<Operation> samples;
  vector<vector<int>> tmp_field;
  vector<Operation> tmp_ops;
  unordered_map<unsigned long,vector<vector<int>>> hash_map;


  // メモリを事前確保
  next_nodes.reserve(depth * num_sample * sizeof(BeamNode));
  samples.reserve(num_sample * sizeof(Operation));

  // 初期場面のノードを代入
  nodes.push_back({calculate_hash(field), {} , evaluator(field)});

  // 300回操作した時点で強制終了
  for (int time = 0; time < 300; time++)
  {
    for (int d = 0; d < depth; d++)
    {
      // sampleにnum_sample個のランダムの要素を入れる
      for(int s = 0; s < num_sample; s++){
        int n = rand_int(2, field.size());
        int x = rand_int(0, field.size() - n);
        int y = rand_int(0, field.size() - n);
        samples.push_back({x,y,n});
      }

      // next_nodeに新しい生成されるnodeを作る
      for(auto node : nodes){
        for(const auto& op : samples){
          tmp_field = hash_map[node.field_hash];
          rotate(tmp_field,op);

          // 同じ局面が既に出てきている場合は飛ばす
          if (hash_map.find(calculate_hash(tmp_field)) != hash_map.end()) continue;

          tmp_ops = node.ops;
          tmp_ops.push_back(op);

          next_nodes.push_back({calculate_hash(tmp_field),tmp_ops,evaluator(tmp_field)});
        }
      }

      // next_nodeから評価値の高いwidth個のnodeを抽出
      sort(next_nodes.begin(), next_nodes.end(), [](const BeamNode& a, const BeamNode& b) {
            return a.score > b.score;
      });
      if (next_nodes.size() > width) next_nodes.resize(width);

      
      nodes = move(next_nodes);
      next_nodes.clear();
    }

    // commit_step分だけ局面を動かす
    for(int i = 0; commit_step;i++){
      rotate(field,nodes[0].ops[i]);
      // 全てのペアが完成した時点でゲーム終了
      if(count_pair(field) == max_pair_number){
        return;
      }
    }
  }
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
void print_answer(int time, int step, const vector<Operation> &ops, const vector<vector<int>> &field)
{
  cout << time << endl;
  cout << step << endl;
  cout << ops.size() << endl;
  for (const auto &op : ops)
  {
    cout << op.x << op.y << op.n << endl;
  }
  cout << endl;
  for (const auto &row : field)
  {
    for (const auto &element : row)
    {
      cout << element;
    }
    cout << endl;
  }
}
