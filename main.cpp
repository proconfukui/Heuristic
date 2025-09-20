#include <iostream>
#include <vector>
#include "utils.hpp"

void initialize(int& start_time,vector<vector<int>>& field,vector<float>& weights);
void print_answer(int time,int step,vector<Operation>& ops,vector<vector<int>>& field);

namespace{
  
}
int main(){
  // std::ios_base::sync_with_stdio(false);
  // std::cin.tie(NULL);
  int start_time;
  vector<vector<int>> field;
  vector<float> weights;
  initialize(start_time,field,weights);
}

// 入力用関数
void initialize(int& start_time,vector<vector<int>>& field,vector<float>& weights){
  std::cin >> time >> std::endl;
  int field_size;
  std::cin >> field_size >> std::endl;
  for(int i = 0; i < size ; i++){
    for(int j = 0; i < size;i++){
      std::cin >> field[i][j];
    }
    std::cin >> std::endl;
  }
  int weight_size;
  std::cin >> weight_size >> std::endl;
  for(int i = 0; i < weight_size; i++){
    std::cin >> weights[i];
  }
  std::cin >> std::endl;
}

// 出力用関数
void print_answer(int time,int step,vector<Operation>& ops,vector<vector<int>>& field){
  std::cout << time << std::endl;
  std::cout << step << std::endl;
  std::cout << ops.size() << std::endl;
  for(const auto& op : ops){
    std::cout << op.x <<  op.y << op.n << std::endl;
  }
  std::cout << std::endl;
  for(const auto& row : field){
    for(const auto& element){
      std::cout << element;
    }
    std::cout << std::endl;
  }
}
