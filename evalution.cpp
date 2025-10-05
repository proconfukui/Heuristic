#include "evalution.hpp"
#include <vector>
#include <functional>
#include <iostream>
#include "base.hpp"

using std::vector;
using std::function;
using std::cerr;
using std::endl;

// 各座標の重み
vector<vector<float>> _weight_matrix;
vector<float> _weights;

// weights(評価関数の重みと、weight_matrixを初期化)
void initialize_evalutor(const vector<vector<int>> &field,const vector<float>& weigths){
  _weights = weigths;
  _weight_matrix = add_matrix(create_x2y2_weight_matrix(field.size()),create_weight_matrix(field.size(),[](float x){return pow(x,2);}));
  _weight_matrix = product_matrix(_weight_matrix,weigths[1]); 
}



// ペア候補間の距離を測定する
// テスト済
int measure_distance(const vector<vector<int>> &field)
{
  const int size = field.size();
  const int max_pair_number = size * size / 2;
  // x1,y1,x2,y2 を一列の配列で管理（-1 初期化）。reserve/clear コスト回避。
  vector<int> coords(max_pair_number * 4, -1);
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      int number = field[y][x];
      int idx = number * 4;
      if (coords[idx] == -1) {
        coords[idx] = x;     // x1
        coords[idx + 1] = y; // y1
      } else {
        coords[idx + 2] = x;     // x2
        coords[idx + 3] = y;     // y2
      }
    }
  }
  int total_dist = 0;
  for (int i = 0; i < max_pair_number; i++) {
    int idx = i * 4;
    int dx = coords[idx] - coords[idx + 2];
    int dy = coords[idx + 1] - coords[idx + 3];
    total_dist += dx * dx + dy * dy; // pow を避ける
  }
  return total_dist;
}

vector<vector<float>> product_matrix(const vector<vector<float>>& field,float term){
  int size = field.size();
  vector<vector<float>> matrix = field;
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      matrix[y][x] *= term; 
    }
  }
  return matrix;
}

// ある関数を与えることにより、その関数をZ軸を中心に回転させたときの、(X,Y)のZの大きさが格納された大きさsizeの二重配列を返す
// 最大値が1になるように標準化される
// テスト済
vector<vector<float>> create_weight_matrix(int size, function<float(float)> func)
{
  int center = size/2;
  vector<vector<float>> matrix = vector<vector<float>>(size, vector<float>(size, 0));
  float max_value = 0.0f;
  
  // まず、すべての値を計算
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
      
      // 最大値を追跡
      if (matrix[y][x] > max_value) {
        max_value = matrix[y][x];
      }
    }
  }
  
  // 最大値で標準化（最大値が0でない場合のみ）
  if (max_value > 0.0f) {
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        matrix[y][x] /= max_value;
      }
    }
  }
  
  return matrix;
}

vector<vector<float>> add_matrix(const vector<vector<float>>& matrix1,const vector<vector<float>>& matrix2){
  int size = matrix1.size();
  vector<vector<float>> matrix = vector<vector<float>>(size, vector<float>(size, 0));
  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      matrix[y][x] = matrix1[y][x] + matrix2[y][x]; 
    }
  }
  print_matrix(matrix);
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
    for (int y = 0; y < field.size(); y++) {
        for (int x = 0; x < field.size() -1 ; x++) {
            if(field[y][x] == field[y][x+1]){
                counter += _weight_matrix[y][x] + _weight_matrix[y][x+1];
            } 
        }
    }
    for (int y = 0; y < field.size()-1; y++) {
        for (int x = 0; x < field.size(); x++) {
            if(field[y][x] == field[y+1][x]){
                counter += _weight_matrix[y][x] + _weight_matrix[y+1][x];
            } 
        }
    }
    return counter;
}

float func1(const vector<vector<int>> &field){
  float term1 = count_pair(field)*_weights[0];
  float term2 = count_weighted_pair(field);
  float term3 = measure_distance(field);
  // cerr << term1 <<" "<< term2 <<" "<< term3 << endl;
  return  term1 + term2 - term3;
}
