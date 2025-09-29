#include <iostream>
#include <vector>
#include "utils.hpp"
#include "base.hpp"
#include "evalution.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::vector;

void initialize(int& time, vector<Operation> &ops, vector<vector<int>> &field);
void print_analysis(const vector<float> values,const vector<float> pair_ratios);
void analys_answer(const vector<Operation>& ops,const vector<vector<int>>& field,vector<float>& values,vector<float>& pair_ratios,const function<float(vector<vector<int>>&)> &evaluator);


int main(){
  int time;
  vector<Operation> ops;
  vector<vector<int>> field;
  initialize(time,ops,field);

  vector<float> values;
  vector<float> pair_ratios;
  
  // main.cppと合わせる
  vector<vector<float>> weight_matrix = create_weight_matrix(field.size(),[](float x){return pow(x,3);});

  analys_answer(ops,field,values,pair_ratios,[=](vector<vector<int>>& field){
    // main.cppと合わせる
    return  count_weighted_pair(field,weight_matrix) - measure_distance(field);
  });
  print_analysis(values,pair_ratios);
  return 0;
}

void analys_answer(const vector<Operation>& ops,const vector<vector<int>>& field,vector<float>& values,vector<float>& pair_ratios,const function<float(vector<vector<int>>&)> &evaluator){
  vector<vector<int>> tmp_field = field;
  int max_pair_number = field.size() * field.size() / 2;
  for(const auto& op :ops){
    rotate(tmp_field,op);
    values.push_back(evaluator(tmp_field));
    pair_ratios.push_back(static_cast<float>(evaluator(tmp_field)) / max_pair_number);
  }
}

void print_analysis(const vector<float> values,const vector<float> pair_ratios){
  cout << values.size() << endl;
  for(const auto& v : values){
    cout << v << " ";
  }
  cout << endl;

  for(const auto& pr : pair_ratios){
    cout << pr << " ";
  }
  cout << endl;
}

// 入力用関数
// テスト済
void initialize(int& time, vector<Operation> &ops, vector<vector<int>> &field)
{
  cin >> time;
  int step;
  cin >> step;
  for (int i = 0; i<step;i++)
  {
    int x,y,n;
    cin >> n >> x >> y;
    ops.push_back({x,y,n});
  }

  int field_size;
  cin >> field_size;
  for (int i = 0;i <field_size;i++)
  {
    vector<int> colm;
    for (int j=0;j<field_size;j++)
    {
      int element;
      cin >> element;
      colm.push_back(element);
    }
    field.push_back(colm);
  }
}

void print_answer(){

}