/*
Input
The input consists of multiples rows. The first row have an integer N (0 < N ≤ 200) representing the size of the forest. The following N rows have N integers Kij (0 < Kij ≤ 1000) each one representing one butterfly specie. The following N*2 rows, each row have 2 integers, representing the cells visited by Bino.

Output
Print out one row with an amount of different species which Bino collected. 
*/

#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <set>
#include <utility>
using namespace std;

typedef long long ll;
typedef pair<ll, ll> p;


int main(){
    ll N, tmp, tmp1;
    vector<vector<ll>> K;
    set<ll> C;
    cin >> N;

    K.resize(N);
    for (int i = 0; i < N; i ++){
        K[i].resize(N);
        for (int j = 0; j < N; j ++){
            cin >> tmp;    
            K[i][j] = tmp;
        }
    }

    for (int i = 0; i < 2*N; i ++){
        cin >> tmp >> tmp1;
        C.insert(K[tmp-1][tmp1-1]);
    }
    cout << C.size() << endl;

    return 0;
}