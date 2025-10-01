#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <fstream>
#include "json.hpp"
using namespace std;
using namespace nlohmann;

int main()
{
    int size;
    cout << "フィールドのサイズ(4~24の偶数)? ";
    cin >> size;

    vector<int> entities(size * size);
    for (int index = 0; index < size * size; index++)
    {
        entities[index] = index / 2;
    }
    random_device seed_gen;
    mt19937 engine(seed_gen());
    shuffle(entities.begin(), entities.end(), engine);

    nlohmann::json problem;
    problem["startsAt"] = 0;
    problem["problem"]["field"]["size"] = size;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            problem["problem"]["field"]["entities"][y][x] = entities[y * size + x];
        }
    }
    ofstream output("./testcase/problem.json");
    output << setw(4) << problem << endl;
    output.close();
}