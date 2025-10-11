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
void rotate(Field& field, Operation op)
{
  // 安全性チェック: 範囲外アクセス防止
  int N = field.size;
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
      int top = field.at(op.y + first, op.x + i);

      // left -> top
      field.at(op.y + first, op.x + i) = field.at(op.y + last - offset, op.x + first);

      // bottom -> left
      field.at(op.y + last - offset, op.x + first) = field.at(op.y + last, op.x + last - offset);

      // right -> bottom
      field.at(op.y + last, op.x + last - offset) = field.at(op.y + i, op.x + last);

      // top -> right
      field.at(op.y + i, op.x + last) = top;
    }
  }
}

// 直接引数の2重配列を上書きする
// テスト済 - in-place反時計回りで最適化（rotateの逆操作を一回で実行）
void unrotate(Field& field, Operation op)
{
  // 安全性チェック: 範囲外アクセス防止
  int N = field.size;
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
      int top = field.at(op.y + first, op.x + i);

      // right -> top
      field.at(op.y + first, op.x + i) = field.at(op.y + i, op.x + last);

      // bottom -> right
      field.at(op.y + i, op.x + last) = field.at(op.y + last, op.x + last - offset);

      // left -> bottom
      field.at(op.y + last, op.x + last - offset) = field.at(op.y + last - offset, op.x + first);

      // top -> left
      field.at(op.y + last - offset, op.x + first) = top;
    }
  }
}

bool check_around_pair(const Field& field)
{
  int counter = 0;
  int field_size = field.size;

  int base_x,base_y;
  base_y=0;
  // 上1行目
  for (int x = 0; x <= field_size - 1; x++)
  {
    if (x == field_size - 1 && field.at(base_y, x) == field.at(base_y + 1, x))
    {
      counter++;
      break;
    }

    if (field.at(base_y, x) == field.at(base_y, x + 1) || field.at(base_y, x) == field.at(base_y + 1, x))
    {
      counter++;
    }
  }
  base_y = 1;
  // 上2行目
  for (int x = 0; x <= field_size - 1; x++)
  {
    if (field.at(base_y, x) == field.at(base_y, x + 1))
    {
      counter++;
    }
  }

  base_x = 0;
  // 左1列目
  for (int y = 2; y < field_size - 1; y++)
  {
    if (y == field_size - 1 && field.at(y, base_x) == field.at(y, base_x + 1))
    {
      counter++;
      break;
    }

    if (field.at(y, base_x) == field.at(y, base_x + 1) || field.at(y, base_x) == field.at(y + 1, base_x))
    {
      counter++;
    }
  }
  // 左2列目
  base_x = 1;
  for (int y = 2; y < field_size - 1; y++)
  {
    if (field.at(y, base_x) == field.at(y + 1, base_x))
    {
      counter++;
    }
  }
  return counter == (field_size * 2 - 2);
}

// 外周2マスがすべてペアで埋まっているかチェックする
bool check_outer_rim_filled(const Field& field) {
    const int field_size = field.size;
    if (field_size < 4) return true;

    // 外周2マスにある各セル(y, x)が、右(y, x+1)または下(y+1, x)とペアになっているかチェック
    for (int y = 0; y < field_size; ++y) {
        for (int x = 0; x < field_size; ++x) {
            // チェック対象は外周2マスのみ
            if (y > 1 && y < field_size - 2 && x > 1 && x < field_size - 2) {
                continue;
            }

            bool is_paired = false;
            // 右隣をチェック (xが右端でない場合)
            if (x < field_size - 1 && field.at(y, x) == field.at(y, x + 1)) {
                is_paired = true;
            }
            // 下隣をチェック (yが下端でない場合)
            if (!is_paired && y < field_size - 1 && field.at(y, x) == field.at(y + 1, x)) {
                is_paired = true;
            }

            // どちらともペアになっていない場合、そのセルは未完成
            if (!is_paired) {
                return false;
            }
        }
    }
    return true;
}

bool check_all_pair(const Field& field)
{
  int field_size = field.size;
  return field_size * field_size / 2 == count_pair(field);
}

int count_pair(const Field& field)
{
  const int n = field.size;
  if (n <= 0)
    return 0;

  int counter = 0;
  // 水平方向のペア
  for (int y = 0; y < n; ++y) {
    for (int x = 0; x < n - 1; ++x) {
      if (field.at(y, x) == field.at(y, x + 1)) counter++;
    }
  }
  // 垂直方向のペア
  for (int y = 0; y < n - 1; ++y) {
    for (int x = 0; x < n; ++x) {
      if (field.at(y, x) == field.at(y + 1, x)) counter++;
    }
  }
  return counter;
}

// デバッグ用。matrixの状態をターミナルに表示する
// テスト済
void print_matrix(const Field& field)
{
  int field_size = field.size;
  cerr << "   ";
  for (int i = 0; i < field_size; i++)
  {
    cerr << setw(3) << i << "|";
  }
  cerr << endl;
  for (int y = 0; y < field_size; y++)
  {
    cerr << setw(2) << y << "|";
    for (int x = 0; x < field_size; x++)
    {
      cerr << setw(3) << field.at(y, x) << " ";
    }
    cerr << endl;
  }
  cerr << endl;
}

// デバッグ用。matrixの状態をターミナルに表示する。
// テスト済
void print_matrix(const vector<vector<double>> &field)
{
  int field_size = field.size();

  cerr << "            ";
  for (int i = 0; i < field_size; i++)
  {
    cerr << setw(12) << i << "|";
  }
  cerr << endl;
  for (int y = 0; y < field_size; y++)
  {
    cerr << setw(11) << y << "|";
    for (int x = 0; x < field_size; x++)
    {
      cerr << setw(12) << field[y][x] << " ";
    }
    cerr << endl;
  }
  cerr << endl;
}

void apply_ops(Field& field, const vector<Operation> ops)
{
  for (const auto &op : ops)
  {
    rotate(field, op);
  }
}

Field cut_field(const Field& field, int x, int y, int n)
{
  Field new_field(n);
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      new_field.at(i, j) = field.at(y + i, x + j);
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
unsigned long hash_field(const Field& field)
{
  int HASH_BASE = 41;
  unsigned long current_hash = 0;
  for (int cell_value : field.data)
  {
    current_hash = current_hash * HASH_BASE + cell_value;
  }
  return current_hash;
}

// a以上b以下のランダムな整数
// テスト済
int rand_int(int a, int b)
{
  return a + rand() % (b - a + 1);
}
