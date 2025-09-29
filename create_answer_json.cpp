#include <iostream>
#include <vector>

#include "json.hpp"
using json = nlohmann::ordered_json;

struct ops {
  int x;
  int y;
  int n;
};

void to_json(json& j, const ops& o) {
  j = json{{"x", o.x}, {"y", o.y}, {"n", o.n}};
}

int main() {
  std::vector<ops> data;

  // 記録された計算結果（例）
  // data.push_back({0, 0, 2});
  // data.push_back({2, 2, 2});
  // data.push_back({4, 4, 8});

  // JSONに変換して出力
  json j;
  j["ops"] = data;

  std::cout << j.dump(4) << std::endl;

  return 0;
}