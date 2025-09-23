#include "evalution.hpp"
#include <vector>
#include "base.hpp"

using std::vector;

// ペア候補間の距離を測定する
int evaluate_distance(const vector<vector<int>> &field)
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
vector<vector<float>> create_weight_matrix(int size,function<float(float)>& func) {
  vector<vector<float>> matrix = vector<vector<float>>(size,vector<float>(size,0));
  for(int y = 0;y<size;y++){
    for(int x = 0;x<size;x++){
      float distance = pow(pow(x,2) + pow(y,2),0.5);
      matrix[y][x] = funx(distance);
    }
  }
  return matrix;
}

// ペアの数を重みを付けて計算する
float count_weighted_pair(const vector<vector<int>>& field,const vector<vector<float>>& weight_matrix){
  
}

