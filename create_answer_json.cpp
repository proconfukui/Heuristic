#include <iostream>
#include <vector>

#include "json.hpp"
using json = nlohmann::ordered_json;

struct ops {
  int x;
  int y;
  int n;
};

// この関数でキーを追加した順番が保持されるようになります
void to_json(json& j, const ops& o) {
  j = json{
      {"x", o.x},  // 1番目
      {"y", o.y},  // 2番目
      {"n", o.n}   // 3番目
  };
}

int main() {
  std::vector<ops> data = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

  json j;
  j["ops"] = data;

  // dump(4)で整形して出力
  std::cout << j.dump(4) << std::endl;

  return 0;
}