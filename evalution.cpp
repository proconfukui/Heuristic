#include "evalution.hpp"
#include <vector>
#include <functional>
#include <iostream>
#include <unordered_map>
#include "base.hpp"

using std::cerr;
using std::endl;
using std::min;
using std::function;
using std::vector;

// 各座標の重み
vector<vector<int>> _weight_matrix1;
vector<vector<int>> _weight_matrix2;
vector<vector<int>> _weight_matrix3;

// weights[0] : ペアの数
// weights[1] : ペア候補間の距離
// weights[2] : z=(xy)^2の分布に基づくペアの評価
// weights[3] : z=f(r) (r=root(x^2+y^2)) の分布に基づくペアの評価

vector<int> _weights;

// weights(評価関数の重みと、weight_matrixを初期化)
void initialize_evalutor(const vector<vector<int>> &field, const vector<int> &weigths)
{
  int field_size = field.size();
  _weights = weigths;
  _weight_matrix1 = create_x2y2_weight_matrix(field_size);
  _weight_matrix2 = create_weight_matrix(field_size, [](int x)
                                         { return pow(x, 2); });
  _weight_matrix3 = create_around_weight_matrix(field_size);
}

// ペア候補間の距離を測定する
// テスト済
int measure_distance(const vector<vector<int>> &field)
{
  // ラベル値（number）がサブフィールドでは連続かつ小さいとは限らないため、
  // 動的なマップで最初の出現座標を保持し、2回目で距離を加算する。
  int field_size = field.size();
  const int n = static_cast<int>(field_size);
  if (n <= 0)
    return 0;

  std::unordered_map<int, std::pair<int, int>> first_pos;
  first_pos.reserve(n * n / 2);

  int total = 0;
  for (int y = 0; y < n; ++y)
  {
    for (int x = 0; x < n; ++x)
    {
      int number = field[y][x];
      auto it = first_pos.find(number);
      if (it == first_pos.end())
      {
        first_pos.emplace(number, std::make_pair(x, y));
      }
      else
      {
        int dx = it->second.first - x;
        int dy = it->second.second - y;
        total += dx * dx + dy * dy;
        // 必要なら消してもよいが、2回出現想定なので放置でも可
      }
    }
  }
  return total;
}

// // ペア候補間の距離を測定する（高速版: 1パス・動的配列の再利用）
// int measure_distance(const vector<vector<int>> &field)
// {
//   const int n = static_cast<int>(field_size);
//   const int max_pair_number = (n * n) / 2;

//   // 初回座標だけ保持（静的ベクタで容量を使い回し）
//   static vector<int> first_x;
//   static vector<int> first_y;
//   first_x.assign(max_pair_number, -1);
//   first_y.assign(max_pair_number, 0);

//   int total = 0;

//   for (int y = 0; y < n; ++y) {
//     const int* row = field[y].data();
//     for (int x = 0; x < n; ++x) {
//       int number = row[x];
//       // 想定: 0 <= number < max_pair_number（範囲外は無視）
//       if (static_cast<unsigned>(number) >= static_cast<unsigned>(max_pair_number)) continue;

//       int px = first_x[number];
//       if (px == -1) {
//         first_x[number] = x;
//         first_y[number] = y;
//       } else {
//         int dx = px - x;
//         int dy = first_y[number] - y;
//         total += dx * dx + dy * dy;
//       }
//     }
//   }
//   return total;
// }

// vector<vector<int>> product_matrix(const vector<vector<int>>& field,int term){
//   int size = field_size;
//   vector<vector<int>> matrix = field;
//   for (int y = 0; y < size; y++) {
//     for (int x = 0; x < size; x++) {
//       matrix[y][x] *= term;
//     }
//   }
//   return matrix;
// }

// ある関数を与えることにより、その関数をZ軸を中心に回転させたときの、(X,Y)のZの大きさが格納された大きさsizeの二重配列を返す
// 最大値が1になるように標準化される
// テスト済
vector<vector<int>> create_weight_matrix(int size, function<int(int)> func)
{
  int center = size / 2;
  vector<vector<int>> matrix = vector<vector<int>>(size, vector<int>(size, -1));
  int max_value = 0;

  // まず、すべての値を計算
  for (int y = 0; y < size; y++)
  {
    for (int x = 0; x < size; x++)
    {
      int x_dis = 0;
      int y_dis = 0;
      if (x < center)
      {
        x_dis = center - x;
      }
      else
      {
        x_dis = x - center + 1;
      }
      if (y < center)
      {
        y_dis = center - y;
      }
      else
      {
        y_dis = y - center + 1;
      }
      int distance = pow(pow(x_dis, 2) + pow(y_dis, 2), 0.5);
      matrix[y][x] = func(distance);

      // 最大値を追跡
      if (matrix[y][x] > max_value)
      {
        max_value = matrix[y][x];
      }
    }
  }

  return matrix;
}

vector<vector<int>> add_matrix(const vector<vector<int>> &matrix1, const vector<vector<int>> &matrix2)
{
  int size = matrix1.size();
  vector<vector<int>> matrix = vector<vector<int>>(size, vector<int>(size, 0));
  for (int y = 0; y < size; y++)
  {
    for (int x = 0; x < size; x++)
    {
      matrix[y][x] = matrix1[y][x] + matrix2[y][x];
    }
  }
  // print_matrix(matrix);
  return matrix;
}

// z = (xy)^2の分布に基づいた重みの二重配列を返す
vector<vector<int>> create_x2y2_weight_matrix(int size)
{
  vector<vector<int>> matrix = vector<vector<int>>(size, vector<int>(size, 0));

  for (int y = 0; y < size; y++)
  {
    for (int x = 0; x < size; x++)
    {
      // (x,y)を[-1,1]の範囲にマッピング
      int normalized_x = (2 * x) / (size - 1) - 1;
      int normalized_y = (2 * y) / (size - 1) - 1;

      // z = (xy)^2 を計算
      int z = pow(normalized_x * normalized_y, 2);
      matrix[y][x] = z;
    }
  }
  return matrix;
}

// フィールドの周囲二マスのペアだけ評価する
vector<vector<int>> create_around_weight_matrix(int size)
{
  vector<vector<int>> matrix = vector<vector<int>>(size, vector<int>(size, 0));
  for (int y = 0; y < size; y++)
  {
    for (int x = 0; x < size; x++)
    {

      if (x == 0 || x == 1 || y == 0 || y == 1||x==size-1||x==size-2||y==size-1||y==size-2)
      {
        matrix[y][x] = 1;
      }
      else
      {
        matrix[y][x] = 0;
      }
    }
  }
  return matrix;
}

// 1.1 隅のペアを評価する (プロジェクトに合わせて修正)
// ペアがフィールドの隅に近いほど高いスコアを付ける評価関数。
// edge_weight はペナルティの重み。
int evaluate_edge_pairs(const vector<vector<int>>& field, int edge_weight)
{
    const int field_size = field.size();
    if (field_size == 0)
    {
        return 0;
    }
    
    long long total_score = 0;
    const int max_pair_number = field_size * field_size / 2;

    // 水平方向のペアをチェック
    for (int y = 0; y < field_size; ++y)
    {
        for (int x = 0; x < field_size - 1; ++x)
        {
            if (field[y][x] == field[y][x + 1])
            {
                int penalty = (min(x, field_size - 1 - x) + min(y, field_size - 1 - y) +
                               min(x + 1, field_size - 1 - (x + 1)) + min(y, field_size - 1 - y));
                total_score += _weights[1] - edge_weight * penalty;
            }
        }
    }

    // 垂直方向のペアをチェック
    for (int y = 0; y < field_size - 1; ++y)
    {
        for (int x = 0; x < field_size; ++x)
        {
            if (field[y][x] == field[y + 1][x])
            {
                int penalty = (min(x, field_size - 1 - x) + min(y, field_size - 1 - y) +
                               min(x, field_size - 1 - x) + min(y + 1, field_size - 1 - (y + 1)));
                total_score += _weights[1] - edge_weight * penalty;
            }
        }
    }

    // スケールを調整して返す（オーバーフローを避けるためlong longで計算）
    return static_cast<int>(total_score / (max_pair_number + 1));
}

// ペアの数を重みを付けて計算する
int count_weighted_pair(const vector<vector<int>> &field, const vector<vector<int>> &weight_matrix)
{
  int field_size = field.size();
  int counter = 0;
  for (int y = 0; y < field_size; y++)
  {
    for (int x = 0; x < field_size - 1; x++)
    {
      if (field[y][x] == field[y][x + 1])
      {
        counter += weight_matrix[y][x] + weight_matrix[y][x + 1];
      }
    }
  }
  for (int y = 0; y < field_size - 1; y++)
  {
    for (int x = 0; x < field_size; x++)
    {
      if (field[y][x] == field[y + 1][x])
      {
        counter += weight_matrix[y][x] + weight_matrix[y + 1][x];
      }
    }
  }
  return counter;
}

int func1(const vector<vector<int>> &field)
{
  return count_weighted_pair(field, _weight_matrix1);
}

int func2(const vector<vector<int>> &field)
{
  int term1 = measure_distance(field) * _weights[1];
  int term2 = count_weighted_pair(field, _weight_matrix2) * _weights[3];
  return -term1 + term2;
}

int func3(const vector<vector<int>> &field)
{
  int term1 = count_pair(field) * _weights[0];
  int term2 = measure_distance(field) * _weights[1];
  // cerr << term1 <<" "<< term2<< endl;
  return term1 - term2;
}

int func4(const vector<vector<int>> &field)
{
  int term1 = count_pair(field) * _weights[0];
  int term2 = measure_distance(field) * _weights[1];
  // cerr << term1 <<" "<< term2<<endl;
  return term1 - term2;
}

// 外周2マスにあるペアを評価する。外側ほど高スコア。
int evaluate_outer_rim_pairs(const vector<vector<int>>& field)
{
    const int field_size = field.size();
    if (field_size < 4)
    {
        return 0;
    }

    long long total_score = 0;

    const auto get_rim_weight = [&](int pos) -> int {
        if (pos == 0 || pos == field_size - 1) return 8; // 最も外側
        if (pos == 1 || pos == field_size - 2) return 4; // 2番目に外側
        if (pos == 2 || pos == field_size - 3) return 2; // 3番目に外側
        if (pos == 3 || pos == field_size - 4) return 1; // 4番目に外側
        return 0;
    };

    // 水平方向のペアをチェック
    for (int y = 0; y < field_size; ++y)
    {
        for (int x = 0; x < field_size - 1; ++x)
        {
            if (field[y][x] == field[y][x + 1] && (y < 4 || y >= field_size - 4 || x < 4 || x >= field_size - 4 -1)) {
                total_score += get_rim_weight(y) * 2 + get_rim_weight(x) + get_rim_weight(x + 1);
            }
        }
    }

    // 垂直方向のペアをチェック
    for (int y = 0; y < field_size - 1; ++y)
    {
        for (int x = 0; x < field_size; ++x)
        {
            if (field[y][x] == field[y + 1][x] && (y < 4 || y >= field_size - 4 -1 || x < 4 || x >= field_size - 4)) {
                total_score += get_rim_weight(x) * 2 + get_rim_weight(y) + get_rim_weight(y + 1);
            }
        }
    }

    return static_cast<int>(total_score);
}
