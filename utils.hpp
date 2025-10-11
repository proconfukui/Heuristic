// utils.hpp
#pragma once // ヘッダーの重複インクルードを防止
#include <vector>
#include <iostream>
#include <memory>

using std::vector;

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

struct Point {
    int x, y;

    // std::setで使えるように比較演算子を定義
    bool operator<(const Point& other) const {
        if (y != other.y) return y < other.y;
        return x < other.x;
    }
    bool operator == (const Point& other) const
    {
        return x == other.x && y == other.y;
    }
};

// BFS探索で使用する構造体に経路情報を追加
struct BFSNode {
    int x, y, dist;
    std::vector<Operation> path;  // 経路を記録
};


struct BeamNode {
    vector<vector<int>> field;
    vector<Operation> ops;
    int score;
    
    // デフォルトコンストラクタ
    BeamNode() : score(0.0f) {}
    
    // コンストラクタ
    BeamNode(const vector<vector<int>>& f, const vector<Operation>& o, int s)
        : field(f), ops(o), score(s) {}
    
    BeamNode(vector<vector<int>>&& f, vector<Operation>&& o, int s)
        : field(std::move(f)), ops(std::move(o)), score(s) {}

    // 明示的なムーブ/コピー（realloc時にムーブが優先されるようにnoexceptを付与）
    BeamNode(const BeamNode&) = default;
    BeamNode& operator=(const BeamNode&) = default;
    BeamNode(BeamNode&&) noexcept = default;
    BeamNode& operator=(BeamNode&&) noexcept = default;
};

struct State {
    vector<vector<int>> field;
    vector<Operation> ops;
    int g; // 実際にかかった手数
    int h; // 予想距離
    int f; // g + h

    // 比較演算子（priority_queue用、fが小さい順）
    bool operator<(const State& other) const {
        return f > other.f;
    }
};

