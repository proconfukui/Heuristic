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
// テスト済 - in-place回転で最適化
void rotate(vector<vector<int>> &field, Operation op)
{
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

// ペアの数を数える
// テスト済
int count_pair(const vector<vector<int>>& field){
    int counter = 0;
    for (int y = 0; y < field.size(); y++) {
        for (int x = 0; x < field.size() -1 ; x++) {
            if(field[y][x] == field[y][x+1]){
                counter++;
            } 
        }
    }
    for (int y = 0; y < field.size()-1; y++) {
        for (int x = 0; x < field.size(); x++) {
            if(field[y][x] == field[y+1][x]){
                counter++;
            } 
        }
    }
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
  cout << "            ";
  for (int i = 0; i < field.size(); i++)
  {
    cout << setw(12) << i << "|";
  }
  cout << endl;
  for (int y = 0; y < field.size(); y++)
  {
    cout << setw(11) << y << "|";
    for (int x = 0; x < field.size(); x++)
    {
      cout << setw(12) << field[y][x] << " ";
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
