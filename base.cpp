#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>
#include <random>
#include "utils.hpp"
#include "base.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::function;
using std::rand;
using std::setw;
using std::vector;



// 直接引数の2重配列を上書きする
// テスト済
void rotate(vector<vector<int>> &field, Operation op)
{
  vector<vector<int>> memo(op.n, vector<int>(op.n));
  for (int dy = 0; dy < op.n; dy++)
  {
    for (int dx = 0; dx < op.n; dx++)
    {
      memo[dy][dx] = field[op.y + op.n - 1 - dx][op.x + dy];
    }
  }
  for (int dy = 0; dy < op.n; dy++)
  {
    for (int dx = 0; dx < op.n; dx++)
    {
      field[op.y + dy][op.x + dx] = memo[dy][dx];
    }
  }
}

// 直接引数の2重配列を上書きする
// テスト済
void unrotate(vector<vector<int>> &field, Operation op)
{
  vector<vector<int>> memo(op.n, vector<int>(op.n));
  for (int dy = 0; dy < op.n; dy++)
  {
    for (int dx = 0; dx < op.n; dx++)
    {
      memo[dy][dx] = field[op.y + dx][op.x + op.n - 1 - dy];
    }
  }
  for (int dy = 0; dy < op.n; dy++)
  {
    for (int dx = 0; dx < op.n; dx++)
    {
      field[op.y + dy][op.x + dx] = memo[dy][dx];
    }
  }
}

// ペアの数を数える
// 
int count_pair(const vector<vector<int>>& field){
    int counter = 0;
    for (int y = 0; y < field.size() -1 ; y++) {
        for (int x = 0; x < field.size() -1 ; x++) {
            if(field[y][x] == field[y][x+1]&& field[y][x] == field[y+1][x]){
                counter++;
            } 
        }
    }
    int max_pair_number = field.size()*field.size()/2;
    return counter;
}



// デバッグ用。matrixの状態をターミナルに表示する
// テスト済
void print_matrix(const vector<vector<int>> &field)
{
  cout << "   ";
  for (int i = 0; i < field.size(); i++)
  {
    cout << setw(3) << i << "|";
  }
  cout << endl;
  for (int y = 0; y < field.size(); y++)
  {
    cout << setw(2) << y << "|";
    for (int x = 0; x < field.size(); x++)
    {
      cout << setw(3) << field[y][x] << " ";
    }
    cout << endl;
  }
  cout << endl;
}


// デバッグ用。matrixの状態をターミナルに表示する。
// テスト済
void print_matrix(const vector<vector<float>> &field)
{
  cout << "     ";
  for (int i = 0; i < field.size(); i++)
  {
    cout << setw(5) << i << "|";
  }
  cout << endl;
  for (int y = 0; y < field.size(); y++)
  {
    cout << setw(4) << y << "|";
    for (int x = 0; x < field.size(); x++)
    {
      cout << setw(5) << field[y][x] << " ";
    }
    cout << endl;
  }
  cout << endl;
}


// 二次元配列のハッシュ値を計算するための関数
unsigned long calculate_hash(vector<vector<int>> field){
  int HASH_BASE = 41;
    unsigned long current_hash = 0;
    for(const auto& row : field){
        for(int cell_value : row){
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
