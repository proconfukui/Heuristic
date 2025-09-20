// utils.hpp
#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include<iostream>

struct Operation
{
    int x = 0;
    int y = 0;
    int n = 0;
    void print() const {
        std::cout << "x :" << x << ", y :" << y << ", n :" << n << std::endl;
    }
    bool operator == (const Operation& other) const
    {
        return x == other.x && y == other.y && n == other.n;
    }
    bool operator < (const Operation& other) const
    {
        if(x != other.x) return x < other.x;
        if(y != other.y) return y < other.y;
        return n < other.n; 
    }
};

struct BeamNode {
    vector<vector<int>> field;
    vector<Operation> ops;
    float score;
};
