#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <chrono>

// Pythonの(x, y)座標タプルを扱うための構造体
struct Point {
    int x, y;

    // std::setで使えるように比較演算子を定義
    bool operator<(const Point& other) const {
        if (y != other.y) return y < other.y;
        return x < other.x;
    }
};

// Pythonのqueueで使っていたタプル(x, y, dist)を扱うための構造体
struct BFSNode {
    int x, y, dist;
};


std::set<Point> generate_fixed_cells_phase1(int target_x, int target_y, int field_size) {
    std::set<Point> fixed_cells;
    for (int y = 0; y < target_y; ++y) {
        for (int x = 0; x < field_size; ++x) {
            fixed_cells.insert({x, y});
        }
    }
    for (int x = 0; x < target_x; ++x) {
        fixed_cells.insert({x, target_y});
    }
    return fixed_cells;
}

std::set<Point> generate_fixed_cells_phase2(int target_x, int target_y, int field_size) {
    std::set<Point> fixed_cells;
    for (int y = 0; y < 2; ++y) {
        for (int x = 0; x < field_size; ++x) {
            fixed_cells.insert({x, y});
        }
    }
    for (int x = 0; x < target_x; ++x) {
        for (int y = 2; y < field_size; ++y) {
            fixed_cells.insert({x, y});
        }
    }
    for (int y = field_size - 1; y > target_y; --y) {
        fixed_cells.insert({target_x, y});
    }
    return fixed_cells;
}

// 逆算を行うための関数なので、回転方向は反時計回り
Point get_rotated_pos(int x, int y, int en_x, int en_y, int en_size) {
    int rel_x = x - en_x;
    int rel_y = y - en_y;
    int new_rel_x = rel_y;
    int new_rel_y = (en_size - 1) - rel_x;
    return {en_x + new_rel_x, en_y + new_rel_y};
}

std::vector<std::vector<int>> calculate_shortest_moves_with_obstacles(
    int field_size,
    int phase,
    int start_x, 
    int start_y, 
    const std::set<Point>& fixed_cells
) {
    if (fixed_cells.count({start_x, start_y})) {
        std::cerr << "Error: Target (" << start_x << ", " << start_y << ") is inside a fixed area. Cannot calculate." << std::endl;
        return {{}};
    }

    std::vector<std::vector<int>> distances(field_size, std::vector<int>(field_size, -1));
    std::deque<BFSNode> queue;

    queue.push_back({start_x, start_y, 0});
    distances[start_y][start_x] = 0;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    std::cout << "Calculating moves for Phase " << phase << ", Target (" << start_x << ", " << start_y 
              << ") with " << fixed_cells.size() << " fixed cells..." << std::endl;

    while (!queue.empty()) {
        BFSNode current = queue.front();
        queue.pop_front();

        for (int n = 2; n <= field_size; ++n) {
            int min_rx = std::max(0, current.x - n + 1);
            int max_rx = std::min(current.x, field_size - n);
            int min_ry = std::max(0, current.y - n + 1);
            int max_ry = std::min(current.y, field_size - n);

            for (int rx = min_rx; rx <= max_rx; ++rx) {
                for (int ry = min_ry; ry <= max_ry; ++ry) {
                    bool is_valid_en = true;
                    for (int ex = rx; ex < rx + n; ++ex) {
                        for (int ey = ry; ey < ry + n; ++ey) {
                            if (fixed_cells.count({ex, ey})) {
                                is_valid_en = false;
                                break;
                            }
                        }
                        if (!is_valid_en) break;
                    }

                    if (!is_valid_en) continue;

                    Point next_pos = get_rotated_pos(current.x, current.y, rx, ry, n);

                    if (distances[next_pos.y][next_pos.x] == -1 && !fixed_cells.count({next_pos.x, next_pos.y})) {
                        distances[next_pos.y][next_pos.x] = current.dist + 1;
                        queue.push_back({next_pos.x, next_pos.y, current.dist + 1});
                    }
                }
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "\nCalculation complete in " << elapsed.count() << " seconds." << std::endl;
    
    return distances;
}

// --- 実行と結果表示のためのメイン関数 ---
int main() {
    // --- 設定項目 ---
    const int FIELD_SIZE = 12;
    const int PHASE = 2;
    const int TARGET_X = 1;
    const int TARGET_Y = 4;
    // -----------------

    std::cout << "--- Move Distribution Generator (Phase " << PHASE << ") ---" << std::endl;
    std::cout << "Field Size: " << FIELD_SIZE << "x" << FIELD_SIZE << std::endl;
    std::cout << "Target Coordinate: (" << TARGET_X << ", " << TARGET_Y << ")" << std::endl;

    std::set<Point> fixed_cells;
    if (PHASE == 1) {
        fixed_cells = generate_fixed_cells_phase1(TARGET_X, TARGET_Y, FIELD_SIZE);
    } else if (PHASE == 2) {
        fixed_cells = generate_fixed_cells_phase2(TARGET_X, TARGET_Y, FIELD_SIZE);
    } else {
        std::cerr << "Error: Unknown Phase " << PHASE << ". Please set PHASE to 1 or 2." << std::endl;
        return 1;
    }

    std::cout << "Auto-generated Fixed Cells: " << fixed_cells.size() << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;

    std::vector<std::vector<int>> move_distribution = 
        calculate_shortest_moves_with_obstacles(FIELD_SIZE, PHASE, TARGET_X, TARGET_Y, fixed_cells);

    if (!move_distribution.empty() && !move_distribution[0].empty()) {
        std::cout << "Resulting distribution grid:" << std::endl;
        for (int y = 0; y < FIELD_SIZE; ++y) {
            for (int x = 0; x < FIELD_SIZE; ++x) {
                std::cout << move_distribution[y][x] << "\t";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}