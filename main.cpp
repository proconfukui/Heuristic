#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>
#include <random>
#include <unordered_map>
#include <chrono>
#include <set>
#include <fstream>
#include "utils.hpp"
#include "base.hpp"
#include "evalution.hpp"
#include "steps_table.hpp"
#include "json.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::function;
using std::pair;
using std::rand;
using std::setw;
using std::vector;
using std::unordered_map;
using std::set;
using std::min;
using std::make_pair;
using std::ofstream;
using std::string;


void initialize(int &start_time, vector<vector<int>> &field, vector<float> &weights);
void print_answer(int time, const vector<Operation> &ops, const vector<vector<int>> &field);
vector<Operation> beam_search(const vector<vector<int>>& field, int depth, int width, int commit_step,int num_sample, const function<float(vector<vector<int>> &)> &evaluator);
vector<Operation> best_operations_random(const vector<vector<int>>& field,int num_sample,int width,const function<float(vector<vector<int>>&)> &evaluator);
// void export_answer(const string& filename, vector<Operation> ops);

int main()
{
  std::ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  // 標準入力から変数へ代入
  int start_time;
  vector<vector<int>> field;
  vector<float> weights;
  initialize(start_time, field, weights);


  vector<vector<float>> weight_matrix = create_weight_matrix(field.size(),[](float x){return pow(x,3);});

  // count_weighted_pairの最大の値
  // float max_pair_score = 0;
  // for(int i =0;i<field.size();i++){
  //   for(int j = 0;j<field.size();j++){
  //     max_pair_score += weight_matrix[i][j];
  //   }
  // }

  // // 初期場面のmeasure_distanceの値
  // float first_dis = measure_distance(field);

  // 解答用の配列
  vector<Operation> answer;

  // 処理の本体。時間を計測する
  // auto begin_time = std::chrono::high_resolution_clock::now();
  
  answer = beam_search(field,10,20,7,100,[=](vector<vector<int>>& field){
    return  count_weighted_pair(field,weight_matrix) - measure_distance(field);
  });

  // auto end_time = std::chrono::high_resolution_clock::now();
  // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
  // cout << "exe_time: " << duration.count() << " ms" << endl;



  float max_pair_number = field.size() * field.size() / 2;

  // 解答をターミナルに表示する
  // for(const auto& op : answer){
  //   rotate(field,op);
  //   cout << count_pair(field) << endl;
  //   cout << "closed ratio " << static_cast<float>(count_pair(field)) / max_pair_number * 100<< "%" << endl;
  //   print_matrix(field);
  // }
  // export_answer("./testcase/answer.json",answer);
  print_answer(start_time,answer,field);
}

// ビームサーチ
// 未テスト
vector<Operation> beam_search(const vector<vector<int>>& field, int depth, int width, int commit_step,int num_sample, const function<float(vector<vector<int>> &)> &evaluator)
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
  vector<Operation> tmp_ops;
 // unordered_map<unsigned long,vector<vector<int>>> hash_map;


  // メモリを事前確保
  next_nodes.reserve(depth * num_sample * sizeof(BeamNode));

 

  // ステップごとの時間の変化を計測したものを格納する配列
  vector<int> time_clocks;

  // max_time回操作した時点で強制終了
  int max_time = 10;
  for (int time = 0; time < max_time; time++)
  {
    // 初期場面のノードを代入
    nodes.push_back({tmp_field, {} , evaluator(tmp_field)});

    // commit_step回のステップにどれほど時間がかかるかを計測
    // cout << "time :" << time <<endl;
    // auto begin_time = std::chrono::high_resolution_clock::now();

    for (int d = 0; d < depth; d++)
    {
      // next_nodeに新しい生成されるnodeを作る
      for(auto& node : nodes){
        candidates = best_operations_random(node.field,num_sample,width,evaluator);
        for(const auto& op : candidates){
          rotate(node.field,op);
          float score = evaluator(node.field);
          tmp_ops = node.ops;
          tmp_ops.push_back(op);
          next_nodes.push_back({node.field,tmp_ops,score});
          unrotate(node.field,op);
        }
      }

      // next_nodeから評価値の高いwidth個のnodeを抽出
      sort(next_nodes.begin(), next_nodes.end(), [](const BeamNode& a, const BeamNode& b) {
            return a.score > b.score;
      });
      if (next_nodes.size() > width) next_nodes.resize(width);
      nodes = next_nodes;
      next_nodes.clear();
    }

    // commit_step分だけ局面を動かす
    const auto& best = nodes.front();
    for(int i = 0; i < commit_step;i++){
      answer.push_back(best.ops[i]);
      rotate(tmp_field,best.ops[i]);
      // 全てのペアが完成した時点でゲーム終了
      if(count_pair(tmp_field) == max_pair_number){
        return answer;
      }
    }

    nodes.clear();

    // commit_step回のステップにどれほど時間がかかるかを計測
    // auto end_time = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
    // cout << "exe_time: " << duration.count() << " ms" << endl;
    // time_clocks.push_back(duration.count());
  }

  return answer;
}


// 2.2 ランダムにの手の評価値を計算し、上位width手を返す
vector<Operation> best_operations_random(const vector<vector<int>>& field,int num_sample,int width,const function<float(vector<vector<int>>&)> &evaluator)
{
    int field_size = field.size();
    // 取りうる全ての手とサンプル数を比べ少ないほうを選ぶ
    num_sample = min(num_sample,steps_table[field_size]);

    vector<vector<int>> tmp_field = field;
    

    set<pair<float,Operation>> candidates;
    while(candidates.size() < num_sample)
    {
        int n = rand_int(2, field_size);
        int x = rand_int(0, field_size - n), y = rand_int(0, field_size - n);
        if(n == field_size && x == 0 && y == 0) continue;
        Operation op = {x, y, n};
        
        rotate(tmp_field,op);
        candidates.insert(make_pair(evaluator(tmp_field),op));
        unrotate(tmp_field,op);        
    }

    vector<Operation> result;
    auto it = candidates.rbegin();
    for (int i = 0; i < width && it != candidates.rend(); ++i,++it)
    {
        result.push_back(it->second);
    }
    return result;
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
    cout << op.n <<" "<< op.x << " "<< op.y  << endl;
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


// // 後で別の実行ファイルとして切り分ける
// void export_answer(const string& filename, vector<Operation> ops){
//     nlohmann::json answer;
//     for (size_t i = 0; i < ops.size(); i++) {
//         answer["ops"][i]["x"] = ops[i].x;
//         answer["ops"][i]["y"] = ops[i].y;
//         answer["ops"][i]["n"] = ops[i].n;
//     }
//     ofstream output(filename);
//     output << setw(4) << answer << endl;
//     output.close();
// }

