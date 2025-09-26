#include <fstream>
#include <iostream>

#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int main() {
  // JSONファイル読み込み
  ifstream ifs("input.json");
  if (!ifs) {
    cerr << "JSONファイルが開けませんでした。" << endl;
    return 1;
  }

  json data;
  ifs >> data;

  // startsAtの出力
  if (data.contains("startsAt")) {
    cout << data["startsAt"] << endl;
  } else {
    cout << "startsAt: 未定義" << endl;
  }

  // sizeの出力
  if (data.contains("problem") && data["problem"].contains("field") &&
      data["problem"]["field"].contains("size")) {
    cout << data["problem"]["field"]["size"] << endl;
  } else {
    cout << "size: 未定義" << endl;
  }

  // entitiesの出力
  if (data.contains("problem") && data["problem"].contains("field") &&
      data["problem"]["field"].contains("entities")) {
    for (const auto& row : data["problem"]["field"]["entities"]) {
      for (const auto& val : row) {
        cout << val << " ";
      }
      cout << endl;
    }
  } else {
    cout << "entities: 未定義" << endl;
  }

  return 0;
}