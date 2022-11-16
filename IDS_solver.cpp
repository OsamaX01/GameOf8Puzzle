#include <iostream>
#include <assert.h>
#include <vector>
#include <queue>
#include <set>
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

    int getCell(int i, int j) {
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

    Node(Board b) : state(b) {
        action = 0;
        cost = 0;
        parentId = -1;
    }

    Node(Board b, char a, int c, int p)
        : state(b), action(a), cost(c), parentId(p) {}

    bool operator==(const Node &rhs) const {
        return state == rhs.state;
    }

    bool operator!=(const Node &rhs) const {
        return !(state == rhs.state);
    }

    bool operator<(const Node &rhs) const  {
        return state < rhs.state;
    }
};

class Solver {
    int maxCost = 0;
    int numberOfAllGeneratedNodes = 0;
    int numberOfPastExpandedNodes = 0;
    bool foundSolution = false;
    unordered_map <int, Node> expandedNodes;

public:
    vector<Node> expand(Node parent, int parentId) {
        const int DX[] = {-1, +1, 0, 0};
        const int DY[] = {0, 0, +1, -1};
        const string DIR = "UDRL";

        pair<int, int> emptyCell = parent.state.getEmptyCell();
        vector<Node> ret;

        for (int dir = 0; dir < 4; dir++) {
            int newX = emptyCell.first + DX[dir];
            int newY = emptyCell.second + DY[dir];
            if (newX < 0 || newX >= 3 || newY < 0 || newY >= 3) {
                continue;
            }

            auto getNewState = [&]() {
                Board newState = parent.state;
                newState.swapCells(emptyCell.first, emptyCell.second, newX, newY);
                return newState;
            };

            Node newNode(
                getNewState(),
                DIR[dir],
                parent.cost + 1,
                parentId
            );

            if (parent.parentId == -1 || newNode != expandedNodes.at(parent.parentId)) {
                ret.emplace_back(newNode);
                numberOfAllGeneratedNodes++;
            }
        }
        
        return ret;
    }

    void dfs(const Node &current, const Node &goal) {
        if (current == goal) {
            if (!foundSolution) {
                printSolution(current);
                foundSolution = true;
            }
            return;
        }

        if (current.cost == maxCost) {
            return;
        }

        int currentId = expandedNodes.size() + 1;
        expandedNodes.emplace(currentId, current);
        vector<Node> childs = expand(current, currentId);

        for (auto child : childs) {
            dfs(child, goal);
        }
    }

    void solve(Node initial, Node goal) {
        while (!foundSolution) {
            maxCost++;
            dfs(initial, goal);
            clear();
        }
    }

    void clear() {
        numberOfPastExpandedNodes += expandedNodes.size();
        expandedNodes.clear();
    }

    void printSolution(Node node) {
        cout << "Number of expanded Nodes : " << numberOfPastExpandedNodes + expandedNodes.size() << '\n';
        cout << "Number of generated Nodes : " << numberOfAllGeneratedNodes << '\n'; 
        // Note: numberOfAllGeneratedNodes is common due all DFS calls
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
    Solver solver;
    Node goal(Board({1, 1, 2, 3, 3, 4, 5, 6, -1}));
    solver.solve(Node(Board()), goal);
}