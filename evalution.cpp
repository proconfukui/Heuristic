#include "evalution.hpp"
#include <vector>
#include <functional>
#include "base.hpp"

using std::vector;
using std::function;

// 各座標の重み
vector<vector<float>> weight_matrix;

// ペア候補間の距離を測定する
// テスト済
int measure_distance(const vector<vector<int>> &field)
{
  int max_pair_number = field.size() * field.size() / 2;
  vector<vector<int>> pair_coordinates = vector<vector<int>>(max_pair_number, vector<int>(4, -1));
  for (int y = 0; y < field.size(); y++)
  {
    for (int x = 0; x < field.size(); x++)
    {
      int number = field[y][x];
      if (pair_coordinates[number][0] == -1)
      {
        pair_coordinates[number][0] = x;
        pair_coordinates[number][1] = y;
      }
      else
      {
        pair_coordinates[number][2] = x;
        pair_coordinates[number][3] = y;
      }
    }
  }
  int total_dist = 0;
  for (int i = 0; i < max_pair_number; i++)
  {
    total_dist += pow(pair_coordinates[i][0] - pair_coordinates[i][2], 2) + pow(pair_coordinates[i][1] - pair_coordinates[i][3], 2);
  }
  return total_dist;
}

// ある関数を与えることにより、その関数をZ軸を中心に回転させたときの、(X,Y)のZの大きさが格納された大きさsizeの二重配列を返す
// テスト済
vector<vector<float>> create_weight_matrix(int size, function<float(float)> func)
{
  int center = size/2;
  vector<vector<float>> matrix = vector<vector<float>>(size, vector<float>(size, 0));
  for (int y = 0; y < size; y++)
  {
    for (int x = 0; x < size; x++)
    {
      int x_dis = 0;
      int y_dis = 0;
      if(x < center){
        x_dis = center - x;
      }else{
        x_dis = x - center + 1;
      }
      if(y < center){
        y_dis = center - y;
      }else{
        y_dis = y - center + 1;
      }
      float distance = pow(pow(x_dis, 2) + pow(y_dis, 2), 0.5);
      matrix[y][x] = func(distance);
    }
  }
  return matrix;
}

// z = (xy)^2の分布に基づいた重みの二重配列を返す
vector<vector<float>> create_x2y2_weight_matrix(int size){
  vector<vector<float>> matrix = vector<vector<float>>(size, vector<float>(size, 0));
  
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      // (x,y)を[-1,1]の範囲にマッピング
      float normalized_x = (2.0f * x) / (size - 1) - 1.0f;
      float normalized_y = (2.0f * y) / (size - 1) - 1.0f;
      
      // z = (xy)^2 を計算
      float z = pow(normalized_x * normalized_y, 2);
      matrix[y][x] = z;
    }
  }
  
  return matrix;
}

// ペアの数を重みを付けて計算する
float count_weighted_pair(const vector<vector<int>> &field)
{
  float counter = 0;
  for (int y = 0; y < field.size() - 1; y++)
  {
    for (int x = 0; x < field.size() - 1; x++)
    {
      if (field[y][x] == field[y][x + 1])
      {
        counter += weight_matrix[y][x] + weight_matrix[y][x + 1];
      }
      if (field[y][x] == field[y + 1][x])
      {
        counter += weight_matrix[y][x] + weight_matrix[y + 1][x];
      }
    }
  }
  return counter;
}

float func1(const vector<vector<int>> &field){
  static bool isFirstCall = true;
  if (isFirstCall) {
    // weight_matrix = create_weight_matrix(field.size(),[](float x){return pow(x,3);});
    weight_matrix = create_x2y2_weight_matrix(field.size());
    print_matrix(weight_matrix);
    isFirstCall = false;
  }
  return  count_weighted_pair(field) - measure_distance(field);
}
