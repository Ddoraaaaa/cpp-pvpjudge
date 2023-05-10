#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <utility>
#include <tuple>

using namespace std;

map<pair<int, int>, int> board;
int n = 6, m = 7;

void print_board() {
    for (int x = 1; x <= n; ++x) {
        for (int y = 1; y <= m; ++y) {
            cerr << board[{x, y}] << " ";
        }
        cerr << '\n';
    }
}

pair<int, int> read_move() {
    int x, y;
    cin >> y;
    for(x = 1; ; x++) {
        if(board[{x, y}] == 0) break;
    }
    board[{x, y}] = 2;
    return {x, y};
}

void make_move(pair<int, int> move) {
    int x, y;
    tie(x, y) = move;
    board[move] = 1;
    cout << y << endl;
}

int get_score(int x, int y) {
    for (int x0 = 1; x0 < x; ++x0) {
        if (board[{x0, y}] == 0) {
            return -1;
        }
    }
    if (board[{x, y}] != 0) {
        return -1;
    }
    int tot_score = 0;
    int d[5][2] = {{0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}};
    for (int i = 0; i < 5; ++i) {
        int dx = d[i][0], dy = d[i][1];
        int x0 = x, y0 = y;
        while (true) {
            int x1 = x0 + dx;
            int y1 = y0 + dy;
            if (x1 < 1 || x1 > n || y1 < 1 || y1 > m || board[{x1, y1}] == 0 ||
                (make_pair(x, y) != make_pair(x0, y0) && board[{x1, y1}] != board[{x0, y0}])) {
                break;
            }
            tot_score += 1;
            x0 = x1;
            y0 = y1;
        }
    }
    return tot_score;
}

pair<int, int> find_greedy() {
    int score = -1;
    int best_x = -1, best_y = -1;
    for (int x = 1; x <= n; ++x) {
        for (int y = 1; y <= m; ++y) {
            if (get_score(x, y) > score) {
                score = get_score(x, y);
                best_x = x;
                best_y = y;
            }
        }
    }

    return {best_x, best_y};
}

int main() {
    int player_turn;
    cin >> player_turn;
    cerr << player_turn << " is player1" << endl;
    if (player_turn == 2) {
        read_move();
    }

    while (true) {
        pair<int, int> next_move = find_greedy();
        make_move(next_move);
        // print_board();
        pair<int, int> op_move = read_move();
        if (op_move.first == -1) {
            break;
        }
    }

    return 0;
}
