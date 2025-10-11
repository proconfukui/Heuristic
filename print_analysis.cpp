#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "utils.hpp"
#include "base.hpp"
#include "evalution.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::string;
using std::endl;
using std::vector;
using std::ifstream;
using std::istringstream;


void initialize(int& time, vector<Operation> &ops, vector<vector<int>> &field);
void print_analysis(const vector<int> values,const vector<int> pair_ratios);
void analys_answer(const vector<Operation>& ops,const vector<vector<int>>& field,vector<int>& values,vector<int>& pair_ratios,const function<int(vector<vector<int>>&)> &evaluator);
vector<int> read_weights_file(char* file,int target_line);


// コマンドライン引数
// 重みのファイル(weight.txt)とその行数を読み取る
int main(int argc, char* argv[])
{

  if(argc !=3){
    cerr << "引数が足りません" << endl;
  }
  int time;
  vector<Operation> ops;
  vector<vector<int>> field;
  initialize(time, ops, field);
  vector<int> weights = read_weights_file(argv[1],atoi(argv[2]));
  initialize_evalutor(field,weights);
  vector<int> values;
  vector<int> pair_ratios;

  // main.cppと合わせる
  analys_answer(ops, field, values, pair_ratios, [&weights](const vector<vector<int>>& field){
    return count_pair(field)*weights[0] - measure_distance(field);
  });

  print_analysis(values, pair_ratios);
  return 0;
}

void analys_answer(const vector<Operation>& ops,const vector<vector<int>>& field,vector<int>& values,vector<int>& pair_ratios,const function<int(vector<vector<int>>&)> &evaluator){
  vector<vector<int>> tmp_field = field;
  int field_size = tmp_field.size();
  int max_pair_number = field_size * field_size / 2;
  for (const auto &op : ops)
  {
    rotate(tmp_field, op);
    values.push_back(evaluator(tmp_field));
    pair_ratios.push_back(100 *count_pair(tmp_field) / max_pair_number);
  }
}

void print_analysis(const vector<int> values, const vector<int> pair_ratios)
{
  cout << values.size() << endl;
  for (const auto &v : values)
  {
    cout << v << " ";
  }
  cout << endl;

  for (const auto &pr : pair_ratios)
  {
    cout << pr << " ";
  }
  cout << endl;
}

// 入力用関数
// テスト済
void initialize(int &time, vector<Operation> &ops, vector<vector<int>> &field)
{
  cin >> time;
  int step;
  cin >> step;
  for (int i = 0; i < step; i++)
  {
    int x, y, n;
    cin >> n >> x >> y;
    ops.push_back({x, y, n});
  }

  int field_size;
  cin >> field_size;
  for (int i = 0; i < field_size; i++)
  {
    vector<int> colm;
    for (int j = 0; j < field_size; j++)
    {
      int element;
      cin >> element;
      colm.push_back(element);
    }
    field.push_back(colm);
  }
}

vector<int> read_weights_file(char* file,int target_line){
  vector<int> weight ={};
  // 重みファイルから指定された行を読み取り
  ifstream weight_file(file);
  if (!weight_file) {
    cerr << "重みファイルが開けませんでした。" << endl;
    exit(1);
  }

  if (target_line <= 0) {
    cerr << "行番号は1以上の整数を指定してください。" << endl;
    exit(1);
  }

  string line;
  int current_line = 0;
  bool found = false;
  
  while (getline(weight_file, line)) {
    current_line++;
    if (current_line == target_line) {
      // 重みの値をパース
      istringstream iss(line);
      int value;
      
      while (iss >> value) {
        weight.push_back(value);
      }
      
      // cout << "重みの数: " << weight.size() << endl;
      // cout << "重みの値: " << line << endl;

      found = true;
      break;
    }
  }
  
  weight_file.close();
  
  if (!found) {
    cerr << "指定された行 " << target_line << " が見つかりませんでした。" << endl;
    exit(1);
  }
  
  return weight;
}