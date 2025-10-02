#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include <functional>

using std::vector;
using std::function;

// グローバル変数の宣言
extern vector<vector<float>> _weight_matrix;
extern vector<float> _weights;

void initialize_evalutor(const vector<vector<int>> &field,const vector<float>& weigths);
int measure_distance(const vector<vector<int>>& field);
vector<vector<float>> create_weight_matrix(int size, function<float(float)> func);
float count_weighted_pair(const vector<vector<int>> &field);
float func1(const vector<vector<int>> &field);
vector<vector<float>> create_x2y2_weight_matrix(int size);
