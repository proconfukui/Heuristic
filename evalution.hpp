#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include <functional>
#include "utils.hpp"

using std::vector;
using std::function;

// グローバル変数の宣言
extern vector<int> _weight_matrix1;
extern vector<int> _weight_matrix2;
extern vector<int> _weight_matrix3;
extern vector<int> _weights;

void initialize_evalutor(const Field& field, const vector<int>& weigths);
int measure_distance(const Field& field);
vector<int> create_weight_matrix(int size, function<int(int)> func);
int count_weighted_pair(const Field& field, const vector<int>& weight_matrix);
int func1(const Field& field);
int func2(const Field& field);
int func3(const Field& field);
int func4(const Field& field);
vector<int> create_x2y2_weight_matrix(int size);
vector<int> add_matrix(const vector<int>& matrix1, const vector<int>& matrix2);
vector<int> create_around_weight_matrix(int size);
int evaluate_edge_pairs(const Field& field, int edge_weight);
int evaluate_outer_rim_pairs(const Field& field);
int evaluate_by_distance_from_center(const Field& field, double progress);
