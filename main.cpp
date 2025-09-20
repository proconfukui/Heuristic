#include <iostream>
#include <vector>
#include "utils.hpp"

using std::vector;
using std::cin;
using std::cout;
using std::endl;

void initialize(int& start_time,vector<vector<int>>& field,vector<float>& weights);
void print_answer(int time,int step,vector<Operation>& ops,vector<vector<int>>& field);


int main(){
  // std::ios_base::sync_with_stdio(false);
  // cin.tie(NULL);

  int start_time;
  vector<vector<int>> field;
  vector<float> weights;
  initialize(start_time,field,weights);
}

// 入力用関数
void initialize(int& start_time,vector<vector<int>>& field,vector<float>& weights){
  cin >> start_time;
  int field_size;
  cin >> field_size;
  for(int i = 0; i < field_size ; i++){
    for(int j = 0; i < field_size;i++){
      cin >> field[i][j];
    }
  }
  int weight_size;
  cin >> weight_size;
  for(int i = 0; i < weight_size; i++){
    cin >> weights[i];
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
