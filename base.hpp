#pragma once // ヘッダーの重複インクルードを防止
#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>
#include <random>
#include <unordered_map>
#include "utils.hpp"

using std::vector;

void rotate(vector<vector<int>> &field, Operation op);
void unrotate(vector<vector<int>> &field, Operation op);
void print_matrix(const vector<vector<int>> &field);
void print_matrix(const vector<vector<double>> &field);
int count_pair(const vector<vector<int>>& field);
int rand_int(int a, int b);
void apply_ops(vector<vector<int>>& field,const vector<Operation> ops);
// Avoid copying the entire field when hashing
unsigned long hash_field(const vector<vector<int>>& field);
vector<vector<int>> cut_field(vector<vector<int>> field, int x, int y, int n);
vector<Operation> correct_op(const vector<Operation>& ops,int x,int y);
bool check_all_pair(const vector<vector<int>> &field);
bool check_around_pair(const vector<vector<int>> &field);


