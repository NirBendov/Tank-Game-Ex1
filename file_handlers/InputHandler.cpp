#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <vector>
using namespace std;

const string wall = "#";
const string damaged_wall = "-";
const string mine = "*";
const string shell = "o";
const string empty_space = " ";

vector<vector<char>> process(string fileName) {
    ifstream f(fileName);
    string s;

    getline(f, s);
    int *d = dims(s);
    int i = 0;
    vector<vector<char>> board;
    while(getline(f, s)) {
        if (i >= d[0]) {
            //handle error
        }
        if (s.length() >= d[1]) {
            // handle error
        }
        if (s.length() < d[1] - 1) {
            // handle error
        }
        vector<char> row;
        for (int j = 0; j < d[1]; ++j) {
            row.push_back(s[j]);
        }
        board.push_back(row);
        i++;
    }
    return board;
}

int *dims(string s) {
    int space = s.find(" ");
    int height = stoi(s.substr(0, space));
    int width = stoi(s.substr(space + 1));
    return (int[2]){height, width};
}
