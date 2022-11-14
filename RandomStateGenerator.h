#include <chrono>
#include <random>
#include <algorithm>
#include <vector>

class RandomStateGenerator {

public:
    static std::vector<int> getRandomCells() {
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::vector<int> cells = {1, 1, 2, 3, 3, 4, 5, 6, -1};
        shuffle(cells.begin(), cells.end(), rng);

        return cells;
    } 
};