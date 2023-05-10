#include <bits/stdc++.h>
#include <sstream>
using namespace std;

int curPlayer = 1;
int a[6][8], cntMove=0;

bool checkPos(int x, int y) {
    if(x < 0 || x > 5 || y < 1 || y > 7 || a[x][y]!=curPlayer) {
        return false;
    } 
    return true;
}

bool checkRow(int x, int y) {
    bool check0=true, check1=true, check2=true;
    for(int i=0; i<4; i++) {
        check0&=checkPos(x, y+i);
        check1&=checkPos(x+i, y);
        check2&=checkPos(x+i, y+i);
    }
    return check0|check1|check2;
}

string check(int pos) {
    if(pos < 1 || pos > 7) {
        return "invalid";
    }
    int row = 6;
    while(row >=1 && a[row-1][pos] == 0) row--;
    if(row==6) return "invalid";
    a[row][pos]=curPlayer;
    cntMove++;
    for(int i=0; i<6; i++) {
        for(int j=1; j<=7; j++) {
            if(a[i][j]==curPlayer) {
                if(checkRow(i, j)) {
                    return "win";
                }
            }
        }
    }
    if(cntMove != 42) {
        return "valid";
    }
    else {
        return "draw";
    }
}

signed main() {

    ofstream fout("log.txt", ofstream::out);
    fout << "test" << endl;

    // time limit for each player is 1000ms
    cout << "time 1000" << endl;
    
    // print the original game state
    cout << "gamestate 0" << endl;

    // start game loop
    string s;
    stringstream strin;
    while(true) {
        // get player ID and number of lines
        // int lineCnt;
        // getline(cin, s); strin.str(s); strin >> curPlayer >> lineCnt;
        // cerr << curPlayer << "lmao " << lineCnt << endl;
        // if(lineCnt != 1) {
        //     return 0;
        // }
        int pos;
        strin.str(""); // ???
        strin.clear(); // this shouldnt be necessary
        getline(cin, s); strin.str(s);
        strin >> pos;
        string turnRes = check(pos);
        if(turnRes == "win") {
            cout << "win" << endl;
        } else if(turnRes == "draw") {
            cout << "draw" << endl;
        } else if(turnRes == "invalid") {
            cout << "lose" << endl;
        } else if(turnRes == "timeout") {
            cout << "lose" << endl;
        } else if(turnRes == "valid") {
            cout << "valid" << endl;
            cout << "gamestate 1" << endl;
            cout << pos << endl;
            fout << "player" << curPlayer << ": " <<pos << endl;
            curPlayer = 3 - curPlayer;
            continue;
        } 
        fout << "player" << curPlayer << ": " <<pos << endl;
        break;
    }
}