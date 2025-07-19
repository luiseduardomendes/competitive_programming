/*
 *Input
 *The input contains several test cases. Each test case is composed by a line containing an integer N (1 ≤ N ≤ 40), the number of tiles in the path. The last test case is followed by a line containing a single zero.

 *Output
 *For each test case, print a line containing a single integer, the number of different step sequences.
 
 */

#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <utility>

using namespace std;

typedef long long ll;
typedef pair<ll, ll> p;

int main(){
    ll t1, t2;
    vector<ll> test_case;

    while (1){
        cin >> t1;
        if (t1 == 0) break;
        test_case.push_back(t1);
    }

    for (auto a : test_case){
        cout << a << endl;
    }

    

    return 0;
}