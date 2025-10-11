#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <chrono>
#include "utils.hpp"

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

// ペアの片割れを探す
Point find_pair(const Field& field,Point p){
    int number = field.at(p.y, p.x);
    int field_size = field.size;
    for(int y = 0;y < field_size;y++){
        for(int x = 0;x< field_size;x++){
            if(x == p.x && y== p.y) continue;
            if(field.at(y, x) == number){
                return {x,y};
            }
        }
    }
    return {-1,-1};
}


vector<Operation> calculate_shortest_moves_with_obstacles(
    const Field& field, 
    int phase,
    int start_x, 
    int start_y
) {
    int field_size = field.size;

    Point move_p;    // 動かすピース
    Point target_p;  // ターゲット座標

    std::deque<BFSNode> queue;
    std::set<Point> fixed_cells;

    if (phase == 1) {
        fixed_cells = generate_fixed_cells_phase1(start_x, start_y, field_size);
        move_p = find_pair(field,{start_x-1,start_y});
        target_p = {start_x, start_y};
        queue.push_back({move_p.x, move_p.y, 0, {}});
        queue.push_back({start_x-1,start_y+1,1,{{start_x-1,start_y,2}}});
    } else if (phase == 2) {
        fixed_cells = generate_fixed_cells_phase2(start_x, start_y, field_size);
        move_p = find_pair(field,{start_x,start_y+1});
        target_p = {start_x, start_y};
        queue.push_back({move_p.x, move_p.y, 0, {}});
        queue.push_back({start_x+1,start_y+1,1,{{start_x,start_y,2}}});
    } else {
        std::cerr << "Error: Unknown Phase " << phase << ". Please set PHASE to 1 or 2." << std::endl;
        exit(1);
    }


    if (fixed_cells.count({start_x, start_y})) {
        std::cerr << "Error: Target (" << start_x << ", " << start_y << ") is inside a fixed area. Cannot calculate." << std::endl;
        exit(1);
    }

    std::vector<std::vector<int>> distances(field_size, std::vector<int>(field_size, -1));
    

    
    distances[start_y][start_x] = 0;
    
    while (!queue.empty()) {
        BFSNode current = queue.front();
        queue.pop_front();
        // ターゲットに到達した場合
        if (current.x == target_p.x && current.y == target_p.y) {
            std::reverse(current.path.begin(),current.path.end());
            return current.path;
        }

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
                        vector<Operation> new_path = current.path;
                        new_path.push_back({rx,ry,n});
                        queue.push_back({next_pos.x, next_pos.y, current.dist + 1,new_path});
                    }
                }
            }
        }
    }
    return {};
}
