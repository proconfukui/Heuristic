#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include <functional>

using std::vector;
using std::function;

// グローバル変数の宣言
extern vector<vector<double>> _weight_matrix1;
extern vector<vector<double>> _weight_matrix2;
extern vector<vector<double>> _weight_matrix3;
extern vector<double> _weights;

void initialize_evalutor(const vector<vector<int>> &field,const vector<double>& weigths);
int measure_distance(const vector<vector<int>>& field);
vector<vector<double>> create_weight_matrix(int size, function<double(double)> func);
double count_weighted_pair(const vector<vector<int>> &field,const vector<vector<double>> & weight_matrix);
double func1(const vector<vector<int>> &field);
double func2(const vector<vector<int>> &field);
double func3(const vector<vector<int>> &field);
double func4(const vector<vector<int>> &field);
vector<vector<double>> create_x2y2_weight_matrix(int size);
vector<vector<double>> add_matrix(const vector<vector<double>>& matrix1,const vector<vector<double>>& matrix2);
// vector<vector<double>> product_matrix(const vector<vector<double>>& field,double term);
vector<vector<double>> create_around_weight_matrix(int size);

