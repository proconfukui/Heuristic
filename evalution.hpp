#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include <functional>

using std::vector;
using std::function;

// グローバル変数の宣言
extern vector<vector<int>> _weight_matrix1;
extern vector<vector<int>> _weight_matrix2;
extern vector<vector<int>> _weight_matrix3;
extern vector<int> _weights;

void initialize_evalutor(const vector<vector<int>> &field,const vector<int>& weigths);
int measure_distance(const vector<vector<int>>& field);
vector<vector<int>> create_weight_matrix(int size, function<int(int)> func);
int count_weighted_pair(const vector<vector<int>> &field,const vector<vector<int>> & weight_matrix);
int func1(const vector<vector<int>> &field);
int func2(const vector<vector<int>> &field);
int func3(const vector<vector<int>> &field);
int func4(const vector<vector<int>> &field);
vector<vector<int>> create_x2y2_weight_matrix(int size);
vector<vector<int>> add_matrix(const vector<vector<int>>& matrix1,const vector<vector<int>>& matrix2);
// vector<vector<int>> product_matrix(const vector<vector<int>>& field,int term);
vector<vector<int>> create_around_weight_matrix(int size);

