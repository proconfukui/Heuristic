#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>
#include <random>
#include "utils.hpp"
#include "base.hpp"

using std::cerr;
using std::cin;
using std::endl;
using std::function;
using std::rand;
using std::setw;
using std::vector;

// 直接引数の2重配列を上書きする
// テスト済 - in-place回転で最適化
void rotate(vector<vector<int>> &field, Operation op)
{
  // 安全性チェック: 範囲外アクセス防止
  int N = static_cast<int>(field.size());
  if (N <= 0) return;
  if (op.n < 2) return;
  if (op.x < 0 || op.y < 0) return;
  if (op.x >= N || op.y >= N) return;
  if (op.x + op.n > N) return;
  if (op.y + op.n > N) return;

  // in-place回転: 外側から内側に向かって同心円状に回転
  for (int layer = 0; layer < op.n / 2; layer++)
  {
    int first = layer;
    int last = op.n - 1 - layer;

    for (int i = first; i < last; i++)
    {
      int offset = i - first;

      // 4つの要素を一時保存して回転
      int top = field[op.y + first][op.x + i];

      // left -> top
      field[op.y + first][op.x + i] = field[op.y + last - offset][op.x + first];

      // bottom -> left
      field[op.y + last - offset][op.x + first] = field[op.y + last][op.x + last - offset];

      // right -> bottom
      field[op.y + last][op.x + last - offset] = field[op.y + i][op.x + last];

      // top -> right
      field[op.y + i][op.x + last] = top;
    }
  }
}

// 直接引数の2重配列を上書きする
// テスト済 - in-place反時計回りで最適化（rotateの逆操作を一回で実行）
void unrotate(vector<vector<int>> &field, Operation op)
{
  // 安全性チェック: 範囲外アクセス防止
  int N = static_cast<int>(field.size());
  if (N <= 0) return;
  if (op.n < 2) return;
  if (op.x < 0 || op.y < 0) return;
  if (op.x >= N || op.y >= N) return;
  if (op.x + op.n > N) return;
  if (op.y + op.n > N) return;

  // in-place反時計回り回転: 外側から内側に向かって同心円状に逆回転
  for (int layer = 0; layer < op.n / 2; layer++)
  {
    int first = layer;
    int last = op.n - 1 - layer;

    for (int i = first; i < last; i++)
    {
      int offset = i - first;

      // 4つの要素を一時保存して反時計回りに回転
      int top = field[op.y + first][op.x + i];

      // right -> top
      field[op.y + first][op.x + i] = field[op.y + i][op.x + last];

      // bottom -> right
      field[op.y + i][op.x + last] = field[op.y + last][op.x + last - offset];

      // left -> bottom
      field[op.y + last][op.x + last - offset] = field[op.y + last - offset][op.x + first];

      // top -> left
      field[op.y + last - offset][op.x + first] = top;
    }
  }
}

bool check_around_pair(const vector<vector<int>> &field)
{
  int counter = 0;

  int base_x,base_y;
  base_y=0;
  // 上1行目
  for (int x = 0; x <= field.size() - 1; x++)
  {
    if (x == field.size() - 1 && field[base_y][x] == field[base_y + 1][x])
    {
      counter++;
      break;
    }

    if (field[base_y][x] == field[base_y][x + 1] || field[base_y][x] == field[base_y + 1][x])
    {
      counter++;
    }
  }
  base_y = 1;
  // 上2行目
  for (int x = 0; x <= field.size() - 1; x++)
  {
    if (field[base_y][x] == field[base_y][x + 1])
    {
      counter++;
    }
  }

  base_x = 0;
  // 左1列目
  for (int y = 2; y < field.size() - 1; y++)
  {
    if (y == field.size() - 1 && field[y][base_x] == field[y][base_x+1])
    {
      counter++;
      break;
    }

    if (field[y][base_x] == field[y][base_x+1] || field[y][base_x] == field[y + 1][base_x])
    {
      counter++;
    }
  }
  // 左2列目
  base_x = 1;
  for (int y = 2; y < field.size() - 1; y++)
  {
    if (field[y][base_x] == field[y + 1][base_x])
    {
      counter++;
    }
  }
  return counter == (field.size() * 2 - 2);
}

bool check_all_pair(const vector<vector<int>> &field)
{
  return field.size() * field.size() / 2 == count_pair(field);
}

// // ペアの数を数える
// // テスト済
// int count_pair(const vector<vector<int>>& field){
//     int counter = 0;
//     for (int y = 0; y < field.size(); y++) {
//         for (int x = 0; x < field.size() -1 ; x++) {
//             if(field[y][x] == field[y][x+1]){
//                 counter++;
//             }
//         }
//     }
//     for (int y = 0; y < field.size()-1; y++) {
//         for (int x = 0; x < field.size(); x++) {
//             if(field[y][x] == field[y+1][x]){
//                 counter++;
//             }
//         }
//     }
//     return counter;
// }

int count_pair(const vector<vector<int>> &field)
{
  const int n = static_cast<int>(field.size());
  if (n <= 0)
    return 0;

  int counter = 0;
  for (int y = 0; y < n; ++y)
  {
    const int *row = field[y].data();
    const int *next = (y + 1 < n) ? field[y + 1].data() : nullptr;

    // 水平: 行内の隣接要素を比較（0..n-2）
    for (int x = 0; x + 1 < n; ++x)
    {
      counter += (row[x] == row[x + 1]);
    }
    // 垂直: 下の行と同じ列を比較（0..n-1）
    if (next)
    {
      for (int x = 0; x < n; ++x)
      {
        counter += (row[x] == next[x]);
      }
    }
  }
  return counter;
}

// デバッグ用。matrixの状態をターミナルに表示する
// テスト済
void print_matrix(const vector<vector<int>> &field)
{
  cerr << "   ";
  for (int i = 0; i < field.size(); i++)
  {
    cerr << setw(3) << i << "|";
  }
  cerr << endl;
  for (int y = 0; y < field.size(); y++)
  {
    cerr << setw(2) << y << "|";
    for (int x = 0; x < field.size(); x++)
    {
      cerr << setw(3) << field[y][x] << " ";
    }
    cerr << endl;
  }
  cerr << endl;
}

// デバッグ用。matrixの状態をターミナルに表示する。
// テスト済
void print_matrix(const vector<vector<float>> &field)
{
  cerr << "            ";
  for (int i = 0; i < field.size(); i++)
  {
    cerr << setw(12) << i << "|";
  }
  cerr << endl;
  for (int y = 0; y < field.size(); y++)
  {
    cerr << setw(11) << y << "|";
    for (int x = 0; x < field.size(); x++)
    {
      cerr << setw(12) << field[y][x] << " ";
    }
    cerr << endl;
  }
  cerr << endl;
}

void apply_ops(vector<vector<int>> &field, const vector<Operation> ops)
{
  for (const auto &op : ops)
  {
    rotate(field, op);
  }
}

vector<vector<int>> cut_field(vector<vector<int>> field, int x, int y, int n)
{
  vector<vector<int>> new_field(n, vector<int>(n, -1));
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      new_field[i][j] = field[y + i][x + j];
    }
  }
  return new_field;
}

vector<Operation> correct_op(const vector<Operation> &ops, int x, int y)
{
  vector<Operation> new_ops(ops.size(), {-1, -1, -1});
  for (int i = 0; i < ops.size(); i++)
  {
    new_ops[i].x = ops[i].x + x;
    new_ops[i].y = ops[i].y + y;
    new_ops[i].n = ops[i].n;
  }
  return new_ops;
}

// 二次元配列のハッシュ値を計算するための関数（参照渡しでコピー回避）
unsigned long hash_field(const vector<vector<int>> &field)
{
  int HASH_BASE = 41;
  unsigned long current_hash = 0;
  for (const auto &row : field)
  {
    for (int cell_value : row)
    {
      current_hash = current_hash * HASH_BASE + cell_value;
    }
  }
  return current_hash;
}

// a以上b以下のランダムな整数
// テスト済
int rand_int(int a, int b)
{
  return a + rand() % (b - a + 1);
}
