#pragma once

std::set<Point> generate_fixed_cells_phase1(int target_x, int target_y, int field_size);
std::set<Point> generate_fixed_cells_phase2(int target_x, int target_y, int field_size);
Point get_rotated_pos(int x, int y, int en_x, int en_y, int en_size);
std::vector<Operation> calculate_shortest_moves_with_obstacles(
    const vector<vector<int>>& field, 
    int phase,
    int start_x, 
    int start_y
);