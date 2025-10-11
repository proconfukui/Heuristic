#pragma once // ヘッダーの重複インクルードを防止
#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>
#include <random>
#include <unordered_map>
#include "utils.hpp"

using std::vector;

void rotate(Field& field, Operation op);
void unrotate(Field& field, Operation op);
void print_matrix(const Field& field);
void print_matrix(const vector<vector<double>> &field);
int count_pair(const Field& field);
int rand_int(int a, int b);
void apply_ops(Field& field, const vector<Operation> ops);
// Avoid copying the entire field when hashing
unsigned long hash_field(const Field& field);
Field cut_field(const Field& field, int x, int y, int n);
vector<Operation> correct_op(const vector<Operation>& ops,int x,int y);
bool check_all_pair(const Field& field);
bool check_around_pair(const Field& field);
bool check_outer_rim_filled(const Field& field);
