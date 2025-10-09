#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include <functional>

using std::vector;
using std::function;

// グローバル変数の宣言
extern vector<vector<float>> _weight_matrix1;
extern vector<vector<float>> _weight_matrix2;
extern vector<vector<float>> _weight_matrix3;
extern vector<float> _weights;

void initialize_evalutor(const vector<vector<int>> &field,const vector<float>& weigths);
int measure_distance(const vector<vector<int>>& field);
vector<vector<float>> create_weight_matrix(int size, function<float(float)> func);
float count_weighted_pair(const vector<vector<int>> &field,const vector<vector<float>> & weight_matrix);
float func1(const vector<vector<int>> &field);
float func2(const vector<vector<int>> &field);
float func3(const vector<vector<int>> &field);
float func4(const vector<vector<int>> &field);
vector<vector<float>> create_x2y2_weight_matrix(int size);
vector<vector<float>> add_matrix(const vector<vector<float>>& matrix1,const vector<vector<float>>& matrix2);
// vector<vector<float>> product_matrix(const vector<vector<float>>& field,float term);
vector<vector<float>> create_around_weight_matrix(int size);

