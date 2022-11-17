#include <iostream>
#include <assert.h>
#include <vector>
#include <queue>
#include <set>
#include <functional>
#include <unordered_map>

#include "RandomStateGenerator.h"

using namespace std;

class Board {
    vector<vector<int>> grid;
    
    void initialize(const vector<int> &cells) {
        grid.resize(3, vector<int>(3));
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                grid[i][j] = cells[i * 3 + j]; 
            }
        }
    }

public:
    Board() {
        initialize(RandomStateGenerator::getRandomCells());
    }

    Board(const vector<int> &cells) {
        initialize(cells);
    }

    int getCell(int i, int j) const {
        return grid[i][j];
    }

    pair<int, int> getEmptyCell() const {
        pair<int, int> ret = {-1, -1};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (grid[i][j] == -1) ret = {i, j};
            }
        }

        return ret;
    }
    
    void swapCells(int i, int j, int w, int k) {
        swap(grid[i][j], grid[w][k]);
    }

    bool operator<(const Board &rhs) const {
        return grid < rhs.grid;
    }

    bool operator==(const Board &rhs) const {
        return grid == rhs.grid;
    }
};

class Node {
public:
    Board state;
    char action;
    int cost;
    int parentId;
    int heuristic;

    Node(Board b, int h) : Node(b, 0, 0, -1, h) {}

    Node(Board b, char a, int c, int p, int h)
        : state(b), action(a), cost(c), parentId(p), heuristic(h) {}

    bool operator==(const Node &rhs) const {
        return state == rhs.state;
    }

    bool operator<(const Node &rhs) const  { 
        if (cost + heuristic != rhs.cost + rhs.heuristic) {
            return cost + heuristic > rhs.cost + rhs.heuristic;
        }
        if (cost != rhs.cost) return cost < rhs.cost;
        if (heuristic != rhs.heuristic) return heuristic < rhs.heuristic;
        if (action != rhs.action) return action < rhs.action;
        return state < rhs.state;
    }
};

class Solver {
    priority_queue<Node> q;
    set<Node> reached;
    unordered_map <int, Node> expandedNodes;
    function<int(Board)> heuristic;
    const int DX[4] = {-1, +1, 0, 0};
    const int DY[4] = {0, 0, +1, -1};
    const string DIR = "UDRL";

public:
    Solver(const function<int(Board)> &heuristic) : heuristic(heuristic) {}

    void expand(Node parent, int parentId) {

        pair<int, int> emptyCell = parent.state.getEmptyCell();

        for (int dir = 0; dir < 4; dir++) {
            int newX = emptyCell.first + DX[dir];
            int newY = emptyCell.second + DY[dir];
            if (newX < 0 || newX >= 3 || newY < 0 || newY >= 3) {
                continue;
            }

            Board newState = parent.state;
            newState.swapCells(emptyCell.first, emptyCell.second, newX, newY);

            Node newNode(
                newState,
                DIR[dir],
                parent.cost + 1,
                parentId,
                heuristic(newState)
            );

            if (!reached.count(newNode)) {
                reached.emplace(newNode);
                q.emplace(newNode);
            }
        }
    }

    void solve(Node initial, Node goal) {
        q.push(initial);
        
        while (!q.empty()) {
            Node current = q.top();
            q.pop();

            if (current == goal) {
                printSolution(current);
                return;
            }
            
            int currentId = expandedNodes.size() + 1;
            expand(current, currentId);
            expandedNodes.emplace(currentId, current);
        }
            
        assert(0);
    }

    void printSolution(Node node) {
        cout << "Number of expanded Nodes : " << expandedNodes.size() << '\n';
        cout << "Number of generated Nodes : " << reached.size() + q.size() << '\n'; 
        cout << "Cost(level) to reach the goal : " << node.cost << '\n';

        vector<Node> path;
        while (node.parentId != -1) {
            path.push_back(node);
            node = expandedNodes.at(node.parentId);
        }
        path.push_back(node);
        reverse(path.begin(), path.end());
        
        auto printNode = [&](Node node) {
            cout << "Node at level " << node.cost << '\n';
            cout << "Action to reach state: " << node.action << '\n';

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cout << node.state.getCell(i, j) << ' ';
                }
                cout << '\n';
            }
            cout << '\n';
        };

        for (auto node : path) {
            printNode(node);
        }
    }
};

int main() {
    Board goalBoard({1, 1, 2, 3, 3, 4, 5, 6, -1});
    unordered_map<int, pair<int, int>> pos;
    
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int cell = goalBoard.getCell(row, col);
            if (!pos.count(cell)) {
                pos[cell] = make_pair(row, col);
            }
        }
    }

    auto manhattanHueristic = [&](const Board &board) {
        int ret = 0;
        bool vis1 = false;
        bool vis3 = false;
        for (int col = 0; col < 3; col++) {
            for (int row = 0; row < 3; row++) {
                int cell = board.getCell(row, col);
                auto [x, y] = pos[cell];
                if (cell == 1) {
                    int x = 0, y = vis1 ? 1 : 0;
                    ret += abs(row - x) + abs(col - y);
                    vis1 = true;
                } else if (cell == 3) {
                    int x = 1, y = vis3 ? 1 : 0;
                    ret += abs(row - x) + abs(col - y);
                    vis3 = true;
                } else {
                    ret += abs(row - x) + abs(col - y);
                }
            }
        }
        return ret;
    };

    auto outOfPlaceHueristic = [&](const Board &board) {
        int ret = 0;
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                if (board.getCell(row, col) != goalBoard.getCell(row, col)) {
                    ++ret;
                }
            }
        }
        return ret;
    };

    auto heuristic = outOfPlaceHueristic;
    
    Solver solver(heuristic);
    Node goal(goalBoard, heuristic(goalBoard));
    Board randomBoard = Board();
    solver.solve(Node(randomBoard, heuristic(randomBoard)), goal); 
    
    return 0;
}
