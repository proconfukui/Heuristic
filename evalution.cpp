#include "evalution.hpp"
#include "base.hpp"


float evaluate_square_avg_distance(vector<vector<int>> field)
{
  int max_pair_number = field.size() * field.size() / 2;
  float total_dist = 0;
  for (int i = 0; i < max_pair_number; i++)
  {
      float dist = abs(puzzle.pair_coordinates[i][0] - puzzle.pair_coordinates[i][2]) + abs(puzzle.pair_coordinates[i][1] - puzzle.pair_coordinates[i][3]);
      total_dist += pow(dist, 2) / puzzle.field_size;
  }
  return total_dist / puzzle.max_pair_number;
}