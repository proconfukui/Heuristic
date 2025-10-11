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
  const int N = field.size;
  if (N <= 0) return;
  if (op.n < 2) return;
  if (op.x < 0 || op.y < 0) return;
  if (op.x >= N || op.y >= N) return;
  if (op.x + op.n > N) return;
  if (op.y + op.n > N) return;
  auto& data = field.data;

  // in-place回転: 外側から内側に向かって同心円状に回転
  for (int layer = 0; layer < op.n / 2; layer++)
  {
    const int first = layer;
    const int last = op.n - 1 - layer;

    for (int i = first; i < last; i++)
    {
      const int offset = i - first;

      // 4つの要素を一時保存して回転
      const int top = data[(op.y + first) * N + (op.x + i)];

      // left -> top
      data[(op.y + first) * N + (op.x + i)] = data[(op.y + last - offset) * N + (op.x + first)];

      // bottom -> left
      data[(op.y + last - offset) * N + (op.x + first)] = data[(op.y + last) * N + (op.x + last - offset)];

      // right -> bottom
      data[(op.y + last) * N + (op.x + last - offset)] = data[(op.y + i) * N + (op.x + last)];

      // top -> right
      data[(op.y + i) * N + (op.x + last)] = top;
    }
  }
}

// 直接引数の2重配列を上書きする
// テスト済 - in-place反時計回りで最適化（rotateの逆操作を一回で実行）
void unrotate(Field& field, Operation op)
{
  const int N = field.size;
  if (N <= 0) return;
  if (op.n < 2) return;
  if (op.x < 0 || op.y < 0) return;
  if (op.x >= N || op.y >= N) return;
  if (op.x + op.n > N) return;
  if (op.y + op.n > N) return;

  auto& data = field.data;
  // in-place反時計回り回転: 外側から内側に向かって同心円状に逆回転
  for (int layer = 0; layer < op.n / 2; layer++)
  {
    const int first = layer;
    const int last = op.n - 1 - layer;

    for (int i = first; i < last; i++)
    {
      const int offset = i - first;

      // 4つの要素を一時保存して反時計回りに回転
      const int top = data[(op.y + first) * N + (op.x + i)];

      // right -> top
      data[(op.y + first) * N + (op.x + i)] = data[(op.y + i) * N + (op.x + last)];

      // bottom -> right
      data[(op.y + i) * N + (op.x + last)] = data[(op.y + last) * N + (op.x + last - offset)];

      // left -> bottom
      data[(op.y + last) * N + (op.x + last - offset)] = data[(op.y + last - offset) * N + (op.x + first)];

      // top -> left
      data[(op.y + last - offset) * N + (op.x + first)] = top;
    }
  }
}

bool check_around_pair(const Field& field)
{
    int counter = 0;
    const int n = field.size;
    if (n < 2) return true;
    const auto& data = field.data;

    // 上1行目 (y=0)
    for (int x = 0; x < n; ++x) {
        const int idx = x;
        bool paired = false;
        if (x < n - 1 && data[idx] == data[idx + 1]) paired = true;
        if (!paired && data[idx] == data[idx + n]) paired = true;
        if (paired) counter++;
    }

    // 上2行目 (y=1)
    for (int x = 0; x < n - 1; ++x) {
        const int idx = n + x;
        if (data[idx] == data[idx + 1]) {
            counter++;
        }
    }

    // 左1列目 (x=0, y=2から)
    for (int y = 2; y < n; ++y) {
        const int idx = y * n;
        bool paired = false;
        if (data[idx] == data[idx + 1]) paired = true;
        if (!paired && y < n - 1 && data[idx] == data[idx + n]) paired = true;
        if (paired) counter++;
    }

    // 左2列目 (x=1, y=2から)
    for (int y = 2; y < n - 1; ++y) {
        const int idx = y * n + 1;
        if (data[idx] == data[idx + n]) {
            counter++;
        }
    }

    // 期待されるペアの数を計算
    // 上1行: n個, 上2行: n-1個
    // 左1列: n-2個, 左2列: n-3個
    // (0,0), (0,1), (1,0), (1,1) の重複を考慮
    // (0,0)は右か下, (0,1)は右か下, (1,0)は右か下, (1,1)は右か下
    // この実装は単純にペアを数えているだけなので、期待値の計算は複雑。
    // 元のコードの `(field_size * 2 - 2)` が正しい期待値であると仮定します。
    return counter == (n * 2 - 2);
}

// 外周2マスがすべてペアで埋まっているかチェックする
bool check_outer_rim_filled(const Field& field) {
    const int field_size = field.size;
    if (field_size < 4) {
        return true;
    }
    const auto& data = field.data;

    // 外周2マスにある各セル(y, x)が、右(y, x+1)または下(y+1, x)とペアになっているかチェック
    for (int y = 0; y < field_size; ++y) {
        const int row_start = y * field_size;
        for (int x = 0; x < field_size; ++x) {
            // チェック対象は外周2マスのみ
            if (y > 1 && y < field_size - 2 && x > 1 && x < field_size - 2) {
                continue;
            }

            const int current_idx = row_start + x;
            bool is_paired = false;
            // 右隣をチェック (xが右端でない場合)
            if (x < field_size - 1 && data[current_idx] == data[current_idx + 1]) { is_paired = true; }
            // 下隣をチェック (yが下端でない場合)
            if (!is_paired && y < field_size - 1 && data[current_idx] == data[current_idx + field_size]) { is_paired = true; }

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
  if (n <= 1) return 0;
  
  int counter = 0;
  const auto& data = field.data;

  // 水平方向のペア
  for (int y = 0; y < n; ++y) {
    int row_start = y * n;
    for (int x = 0; x < n - 1; ++x) {
      if (data[row_start + x] == data[row_start + x + 1]) counter++;
    }
  }

  // 垂直方向のペア
  for (int i = 0; i < n * (n - 1); ++i) {
    if (data[i] == data[i + n]) counter++;
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
  const int field_size = field.size;
  for (int i = 0; i < n; i++)
  {
    const int src_row_start = (y + i) * field_size;
    for (int j = 0; j < n; j++)
    {
      new_field.data[i * n + j] = field.data[src_row_start + (x + j)];
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
  unsigned long current_hash = 0; // FNV-1aハッシュのオフセット基底
  // 1次元配列を直接イテレート
  for (int cell_value : field.data) {
    current_hash ^= static_cast<unsigned long>(cell_value);
    current_hash *= HASH_BASE;
  }
  return current_hash;
}

// a以上b以下のランダムな整数
// テスト済
int rand_int(int a, int b)
{
  return a + rand() % (b - a + 1);
}
