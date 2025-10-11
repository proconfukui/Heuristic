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
vector<int> _weight_matrix1;
vector<int> _weight_matrix2;
vector<int> _weight_matrix3;

// weights[0] : ペアの数
// weights[1] : ペア候補間の距離
// weights[2] : z=(xy)^2の分布に基づくペアの評価
// weights[3] : z=f(r) (r=root(x^2+y^2)) の分布に基づくペアの評価

vector<int> _weights;

// weights(評価関数の重みと、weight_matrixを初期化)
void initialize_evalutor(const Field& field, const vector<int> &weigths)
{
  int field_size = field.size;
  _weights = weigths;
  _weight_matrix1 = create_x2y2_weight_matrix(field_size);
  _weight_matrix2 = create_weight_matrix(field_size, [](int x)
                                         { return pow(x, 2); });
  _weight_matrix3 = create_around_weight_matrix(field_size);
}

// ペア候補間の距離を測定する
// テスト済
int measure_distance(const Field& field)
 {
     const int n = field.size;
     if (n <= 0)
         return 0;
 
     const int max_pair_number = (n * n) / 2;
 
     // ペアの最初の出現位置を保持するための静的ベクタ
     // -1で初期化されているかどうかで、すでに出現したかを判断
     static vector<Point> first_pos;
     if (static_cast<int>(first_pos.size()) < max_pair_number) {
         first_pos.resize(max_pair_number);
     }
     std::fill(first_pos.begin(), first_pos.begin() + max_pair_number, Point{-1, -1});
 
     int total_distance = 0;
     for (int i = 0; i < n * n; ++i) {
         int number = field.data[i];
         if (number >= 0 && number < max_pair_number) {
             if (first_pos[number].x == -1) {
                 first_pos[number] = {i % n, i / n};
             } else {
                 int x = i % n;
                 int y = i / n;
                 int dx = first_pos[number].x - x;
                 int dy = first_pos[number].y - y;
                 total_distance += dx * dx + dy * dy;
             }
         }
     }
     return total_distance;
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
vector<int> create_weight_matrix(int size, function<int(int)> func)
{
  int center = size / 2;
  vector<int> matrix(size * size, -1);
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
      matrix[y * size + x] = func(distance);

      // 最大値を追跡
      if (matrix[y * size + x] > max_value)
      {
        max_value = matrix[y * size + x];
      }
    }
  }

  return matrix;
}

vector<int> add_matrix(const vector<int> &matrix1, const vector<int> &matrix2)
{
  size_t size = matrix1.size();
  vector<int> matrix(size, 0);
  for (size_t i = 0; i < size; ++i)
  {
    matrix[i] = matrix1[i] + matrix2[i];
  }
  // print_matrix(matrix);
  return matrix;
}

// z = (xy)^2の分布に基づいた重みの二重配列を返す
vector<int> create_x2y2_weight_matrix(int size)
{
  vector<int> matrix(size * size, 0);

  for (int y = 0; y < size; y++)
  {
    for (int x = 0; x < size; x++)
    {
      // (x,y)を[-1,1]の範囲にマッピング
      double normalized_x = (2.0 * x) / (size - 1) - 1.0;
      double normalized_y = (2.0 * y) / (size - 1) - 1.0;

      // z = (xy)^2 を計算
      double z = pow(normalized_x * normalized_y, 2.0);
      matrix[y * size + x] = static_cast<int>(z * 100); // Scale to int
    }
  }
  return matrix;
}

// フィールドの周囲二マスのペアだけ評価する
vector<int> create_around_weight_matrix(int size)
{
  vector<int> matrix(size * size, 0);
  for (int y = 0; y < size; y++)
  {
    for (int x = 0; x < size; x++)
    {

      if (x == 0 || x == 1 || y == 0 || y == 1||x==size-1||x==size-2||y==size-1||y==size-2)
      {
        matrix[y * size + x] = 1;
      }
      else
      {
        matrix[y * size + x] = 0;
      }
    }
  }
  return matrix;
}

// 1.1 隅のペアを評価する (プロジェクトに合わせて修正)
// ペアがフィールドの隅に近いほど高いスコアを付ける評価関数。
// edge_weight はペナルティの重み。
int evaluate_edge_pairs(const Field& field, int edge_weight)
{
    const int field_size = field.size;
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
            if (field.at(y, x) == field.at(y, x + 1))
            {
                int penalty = (min(x, field_size - 1 - x) + min(y, field_size - 1 - y) +
                               min(x + 1, field_size - 1 - (x + 1)) + min(y, field_size - 1 - y));
                total_score += _weights[2] - edge_weight * penalty * penalty;
            }
        }
    }

    // 垂直方向のペアをチェック
    for (int y = 0; y < field_size - 1; ++y)
    {
        for (int x = 0; x < field_size; ++x)
        {
            if (field.at(y, x) == field.at(y + 1, x))
            {
                int penalty = (min(x, field_size - 1 - x) + min(y, field_size - 1 - y) +
                               min(x, field_size - 1 - x) + min(y + 1, field_size - 1 - (y + 1)));
                total_score += _weights[2] - edge_weight * penalty * penalty;
            }
        }
    }

    // スケールを調整して返す（オーバーフローを避けるためlong longで計算）
    return static_cast<int>(total_score / (max_pair_number + 1));
}

// ペアの数を重みを付けて計算する
int count_weighted_pair(const Field& field, const vector<int>& weight_matrix)
{
  int field_size = field.size;
  if (field_size <= 1) return 0;

  int counter = 0;
  const auto& data = field.data;

  // 水平方向
  for (int y = 0; y < field_size; ++y) {
    int row_start = y * field_size;
    for (int x = 0; x < field_size - 1; ++x) {
      int idx = row_start + x;
      if (data[idx] == data[idx + 1]) {
        counter += weight_matrix[idx] + weight_matrix[idx + 1];
      }
    }
  }

  // 垂直方向
  for (int i = 0; i < field_size * (field_size - 1); ++i) {
    if (data[i] == data[i + field_size]) {
      counter += weight_matrix[i] + weight_matrix[i + field_size];
    }
  }
  return counter;
}

int func1(const Field& field)
{
  return count_weighted_pair(field, _weight_matrix1);
}

int func2(const Field& field)
{
  int term1 = measure_distance(field) * _weights[1];
  int term2 = count_weighted_pair(field, _weight_matrix2) * _weights[3];
  return -term1 + term2;
}

int func3(const Field& field)
{
  int term1 = count_pair(field) * _weights[0];
  int term2 = measure_distance(field) * _weights[1];
  // cerr << term1 <<" "<< term2<< endl;
  return term1 - term2;
}

int func4(const Field& field)
{
  int term1 = count_pair(field) * _weights[0];
  int term2 = measure_distance(field) * _weights[1];
  // cerr << term1 <<" "<< term2<<endl;
  return term1 - term2;
}

// 外周2マスにあるペアを評価する。外側ほど高スコア。
int evaluate_outer_rim_pairs(const Field& field)
{
    const int field_size = field.size;
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

    const auto& data = field.data;
    // 水平方向のペアをチェック
    for (int y = 0; y < field_size; ++y) {
        int row_start = y * field_size;
        for (int x = 0; x < field_size - 1; ++x) {
            int idx = row_start + x;
            if (data[idx] == data[idx + 1] && (y < 4 || y >= field_size - 4 || x < 4 || x >= field_size - 4 -1)) {
                total_score += get_rim_weight(y) * 2 + get_rim_weight(x) + get_rim_weight(x + 1);
            }
        }
    }

    // 垂直方向のペアをチェック
    for (int y = 0; y < field_size - 1; ++y) {
        int row_start = y * field_size;
        for (int x = 0; x < field_size; ++x) {
            int idx = row_start + x;
            if (data[idx] == data[idx + field_size] && (y < 4 || y >= field_size - 4 -1 || x < 4 || x >= field_size - 4)) {
                total_score += get_rim_weight(x) * 2 + get_rim_weight(y) + get_rim_weight(y + 1);
            }
        }
    }

    return static_cast<int>(total_score);
}

// 中心からの距離に基づいてペアを評価する関数
// 探索の進行度(progress)に応じて、評価の重点を外側から内側へ動的にシフトさせる
int evaluate_by_distance_from_center(const Field& field, double progress) {
    const int size = field.size;
    const double center = (size - 1.0) / 2.0;
    long long total_score = 0;

    // progress (0.0 -> 1.0) に応じて、評価の重点を置く「リング」の半径を決定
    // progress=0.0 のとき、最も外側を重視
    // progress=1.0 のとき、中心を重視
    const double target_radius = center * (1.0 - progress);

    const auto get_weight = [&](double dist_from_center) {
        // target_radiusからの距離が小さいほど高い重みを与えるガウス関数的な重み付け
        double diff = dist_from_center - target_radius;
        // 分散を調整して、重みの集中度合いを変える (小さいほどシャープになる)
        double sigma = center / 4.0; // 例: 半径の1/4を標準偏差とする
        return static_cast<long long>(1000.0 * exp(-(diff * diff) / (2.0 * sigma * sigma)));
    };

    const auto& data = field.data;
    // 水平ペア
    for (int y = 0; y < size; ++y) {
        int row_start = y * size;
        for (int x = 0; x < size - 1; ++x) {
            int idx = row_start + x;
            if (data[idx] == data[idx + 1]) {
                double dist_y = y - center;
                double dist_x = (x + 0.5) - center;
                total_score += get_weight(sqrt(dist_x * dist_x + dist_y * dist_y));
            }
        }
    }
    // 垂直ペア
    for (int i = 0; i < size * (size - 1); ++i) {
        if (data[i] == data[i + size]) {
            int y = i / size;
            int x = i % size;
            double dist_y = (y + 0.5) - center;
            double dist_x = x - center;
            total_score += get_weight(sqrt(dist_x * dist_x + dist_y * dist_y));
        }
    }

    return static_cast<int>(total_score);
}
