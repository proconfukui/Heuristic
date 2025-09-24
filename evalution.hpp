#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include <functional>

using std::vector;
using std::function;

int measure_distance(const vector<vector<int>>& field);
vector<vector<float>> create_weight_matrix(int size, function<float(float)> func);
float count_weighted_pair(const vector<vector<int>> &field, const vector<vector<float>> &weight_matrix);
