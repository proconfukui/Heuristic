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

struct OperationHash {
    std::size_t operator()(const Operation& op) const {
        // 簡単なハッシュ関数。より良いハッシュ関数も検討可能
        auto h1 = std::hash<int>{}(op.x);
        auto h2 = std::hash<int>{}(op.y);
        auto h3 = std::hash<int>{}(op.n);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
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

struct Field {
    vector<int> data;
    int size = 0;

    Field() = default;
    Field(int s) : data(s * s), size(s) {}
    Field(const vector<vector<int>>& field2d) {
        if (!field2d.empty()) {
            size = field2d.size();
            data.reserve(size * size);
            for (const auto& row : field2d) {
                data.insert(data.end(), row.begin(), row.end());
       光速か
      }
        }
    }

    int& at(int y, int x) { return data[y * size + x]; }
    const int& at(int y, int x) const { return data[y * size + x]; }
};

struct BeamNode {
    Field field;
    vector<Operation> ops;
    int score;
    
    // デフォルトコンストラクタ
    BeamNode() : score(0) {}
    
    // コンストラクタ
    BeamNode(const Field& f, const vector<Operation>& o, int s)
        : field(f), ops(o), score(s) {}
    
    BeamNode(Field&& f, vector<Operation>&& o, int s)
        : field(std::move(f)), ops(std::move(o)), score(s) {}

    // 明示的なムーブ/コピー（realloc時にムーブが優先されるようにnoexceptを付与）
    BeamNode(const BeamNode&) = default;
    BeamNode& operator=(const BeamNode&) = default;
    BeamNode(BeamNode&&) noexcept = default;
    BeamNode& operator=(BeamNode&&) noexcept = default;
};

struct State {
    Field field;
    vector<Operation> ops;
    int g; // 実際にかかった手数
    int h; // 予想距離
    int f; // g + h

    // 比較演算子（priority_queue用、fが小さい順）
    bool operator<(const State& other) const {
        return f > other.f;
    }
};
