#include <iostream>
#include <vector>
#include <functional>
#include "utils.hpp"
#include <iomanip>


using std::vector;
using std::function;
using std::setw;
using std::cin;
using std::cout;
using std::endl;

void initialize(int& start_time,vector<vector<int>>& field,vector<float>& weights);
void print_answer(int time,int step,vector<Operation>& ops,vector<vector<int>>& field);
vector<Operation> beam_search(vector<vector<int>>& field,function<float(vector<vector<int>>&)> &evaluator,int depth,int width,int num_sample);
void print_field(vector<vector<int>>& field);
void rotate(vector<vector<int>>& field,Operation op);
void unrotate(vector<vector<int>>& field,Operation op);

int main(){
  // std::ios_base::sync_with_stdio(false);
  // cin.tie(NULL);

  int start_time;
  vector<vector<int>> field;
  vector<float> weights;
  initialize(start_time,field,weights);
  // print_field(field);
  // unrotate(field,{1,1,2});
  // print_field(field);
}

vector<Operation> beam_search(vector<vector<int>>& field,function<float(vector<vector<int>>&)> &evaluator,int depth,int width,int num_sample){
  return {{1,1,1}};
}

// 直接引数の2重配列を上書きする
void rotate(vector<vector<int>>& field,Operation op){
  vector<vector<int>> memo(op.n, vector<int>(op.n));
    for (int dy = 0; dy < op.n; dy++) {
        for (int dx = 0; dx < op.n; dx++) {
            memo[dy][dx] = field[op.y + op.n - 1 - dx][op.x + dy];
        }
    }
    for (int dy = 0; dy < op.n; dy++) {
        for (int dx = 0; dx < op.n; dx++) {
            field[op.y + dy][op.x + dx] = memo[dy][dx];
        }
    }
}

// 直接引数の2重配列を上書きする
void unrotate(vector<vector<int>>& field,Operation op){
  vector<vector<int>> memo(op.n, vector<int>(op.n));
    for (int dy = 0; dy < op.n; dy++) {
        for (int dx = 0; dx < op.n; dx++) {
          memo[dy][dx] = field[op.y + dx][op.x + op.n - 1 - dy];
        }
    }
    for (int dy = 0; dy < op.n; dy++) {
        for (int dx = 0; dx < op.n; dx++) {
            field[op.y + dy][op.x + dx] = memo[dy][dx];
        }
    }
}

void print_field(vector<vector<int>>& field){
  cout << "   ";
  for (int i = 0; i < field.size(); i++) {
      cout << setw(3) << i << "|";
  }
  cout << endl;
  for (int y = 0; y < field.size(); y++) {
      cout << setw(2) << y << "|";
      for (int x = 0; x < field.size(); x++) {
          cout << setw(3) << field[y][x] << " ";
      }
      cout << endl;
  }
  cout << endl;
}

// 入力用関数
void initialize(int& start_time,vector<vector<int>>& field,vector<float>& weights){
  cin >> start_time;
  int field_size;
  cin >> field_size;
  for(int i = 0; i < field_size ; i++){
    vector<int> row(field_size);
    for(int j = 0; j < field_size;j++){
      cin >> row[j];
    }
    field.push_back(row);
  }
  int weight_size;
  cin >> weight_size;
  float value;
  for(int i = 0; i < weight_size; i++){
    cin >> value;
    weights.push_back(value);
  }
}

// 出力用関数
void print_answer(int time,int step,vector<Operation>& ops,vector<vector<int>>& field){
  cout << time << endl;
  cout << step << endl;
  cout << ops.size() << endl;
  for(const auto& op : ops){
    cout << op.x <<  op.y << op.n << endl;
  }
  cout << endl;
  for(const auto& row : field){
    for(const auto& element:row){
      cout << element;
    }
    cout << endl;
  }
}
