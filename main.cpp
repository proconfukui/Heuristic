#include <iostream>
#include <vector>
#include <pair>
#include <functional>
#include <iomanip>
#include <random>
#include <unordered_map>
#include "utils.hpp"

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
void print_answer(int time, int step, vector<Operation> &ops, vector<vector<int>> &field);
vector<Operation> beam_search(vector<vector<int>> &field, function<float(vector<vector<int>> &)> &evaluator, int depth, int width, int num_sample);
void print_field(vector<vector<int>> &field);
void rotate(vector<vector<int>> &field, Operation op);
void unrotate(vector<vector<int>> &field, Operation op);
int rand_int(int a, int b);
unsigned long calculate_hash(vector<vectro<int>> field);



int main()
{
  std::ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int start_time;
  vector<vector<int>> field;
  vector<float> weights;
  initialize(start_time, field, weights);
  print_field(field);
  unrotate(field, {1, 1, 2});
  print_field(field);
}

// ビームサーチ
// 未テスト
vector<Operation> beam_search(vector<vector<int>> &field, function<float(vector<vector<int>> &)> &evaluator, int depth, int width, int num_sample)
{
  // 使う配列を事前に宣言
  vector<BeamNode> nodes;
  vector<BeamNode> next_nodes;
  vector<Operation> samples;
  vector<vector<int>> tmp_field;
  vector<Operation> tmp_ops;
  unordered_map<unsigned long,vector<vector<int>>> hash_map;


  // メモリを事前確保
  next_nodes.reserve(depth * num_sample * sizeof(BeamNode));
  samples(num_sample * sizeof(Operation));

  // 初期場面のノードを代入
  nodes.push_back({calculate_hash(field), evaluator(field), []});

  // 300回操作した時点で強制終了
  for (int time = 0; time < 300; time++)
  {
    for (int d = 0; d < depth; d++)
    {
      // sampleにnum_sample個のランダムの要素を入れる
      for(int s = 0; s < num_sample; s++){
        int x = rand_int(0, field.size() - n);
        int y = rand_int(0, field.size() - n);
        int n = rand_int(2, field.size());
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
  }
  return ;
}

// 二次元配列のハッシュ値を計算するための関数
unsigned long calculate_hash(vector<vectro<int>> field){
  int HASH_BASE = 41;
    unsigned long current_hash = 0;
    for(const auto& row : field){
        for(int cell_value : row){
            current_hash = current_hash * HASH_BASE + cell_value;
        }
    }
    return current_hash;
}


// a以上b以下のランダムな整数
// テスト済
int rand_int(int a, int b)
{
    return a + rand() % (b - a + 1);
}

// 直接引数の2重配列を上書きする
// テスト済
void rotate(vector<vector<int>> &field, Operation op)
{
  vector<vector<int>> memo(op.n, vector<int>(op.n));
  for (int dy = 0; dy < op.n; dy++)
  {
    for (int dx = 0; dx < op.n; dx++)
    {
      memo[dy][dx] = field[op.y + op.n - 1 - dx][op.x + dy];
    }
  }
  for (int dy = 0; dy < op.n; dy++)
  {
    for (int dx = 0; dx < op.n; dx++)
    {
      field[op.y + dy][op.x + dx] = memo[dy][dx];
    }
  }
}

// 直接引数の2重配列を上書きする
// テスト済
void unrotate(vector<vector<int>> &field, Operation op)
{
  vector<vector<int>> memo(op.n, vector<int>(op.n));
  for (int dy = 0; dy < op.n; dy++)
  {
    for (int dx = 0; dx < op.n; dx++)
    {
      memo[dy][dx] = field[op.y + dx][op.x + op.n - 1 - dy];
    }
  }
  for (int dy = 0; dy < op.n; dy++)
  {
    for (int dx = 0; dx < op.n; dx++)
    {
      field[op.y + dy][op.x + dx] = memo[dy][dx];
    }
  }
}

// デバッグ用。fieldの状態をターミナルに表示する
// テスト済
void print_field(vector<vector<int>> &field)
{
  cout << "   ";
  for (int i = 0; i < field.size(); i++)
  {
    cout << setw(3) << i << "|";
  }
  cout << endl;
  for (int y = 0; y < field.size(); y++)
  {
    cout << setw(2) << y << "|";
    for (int x = 0; x < field.size(); x++)
    {
      cout << setw(3) << field[y][x] << " ";
    }
    cout << endl;
  }
  cout << endl;
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
void print_answer(int time, int step, vector<Operation> &ops, vector<vector<int>> &field)
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
