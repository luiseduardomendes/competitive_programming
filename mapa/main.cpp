/*Entrada

A primeira linha da entrada contém dois números N (1 ≤ N ≤ 500) e M (1 ≤ M ≤ 124750) que representam o número de cidades e o número de rodovias respectivamente. Cada uma das próximas M linhas é composta por três inteiros U, V (1 ≤ U, V ≤ N e U ≠ V) e C (1 ≤ C ≤ 500) que indiciam que existe uma rodovia de comprimento C que liga as cidades U e V.
Saída

A saída consiste em apenas uma linha contendo a soma do comprimento das rodovias selecionadas.
*/

#include <bits/stdc++.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>

#define MAX_LL 0x3f3f3f3f3f3f3f3f
#define MAX_INT 0x3f3f3f3f

using namespace std;

typedef long long ll;
typedef pair<ll, ll> p;

int N, M;
int t1, t2, t3;
vector<vector<ll>> cost;
vector<p> roads;

void djikstra(int N);

int main(){
    
    cin >> N >> M;

    cost.resize(N);
    for (int i = 0; i < N; i ++){
        cost[i].resize(N);
        for (int j = 0; j < N; j ++){
            cost[i][j] = MAX_LL;    
        }
        cost[i][i] = 0;
    }

    for (int i = 0; i < M; i ++){
        cin >> t1 >> t2 >> t3;   
        cost[t1-1][t2-1] = t3;
        cost[t2-1][t1-1] = t3;
        roads.push_back({t1-1, t2-1});
        //cost[i] = {t1,t2,t3};

    }

    
    for (auto c0 : cost){
        for (auto c : c0){
            cout << c << "\t";
        } 
        cout << endl;
    }
    

    djikstra(N);

    for (auto c0 : cost){
        for (auto c : c0){
            cout << c << "\t";
        } 
        cout << endl;
    }

    ll sum = 0;
    for (auto road: roads){
        cout << road.first+1 << " <-> " << road.second+1 << endl;
        sum += cost[road.first][road.second];
    }
    cout << sum << endl;

    return 0;
}

/*ll smallest_cost(vector<bool> visited){
    ll id = 0;
    ll value = 0x3f3f3f3f3f3f3f3f;
    for (int i = 0; i < M; i ++){
        if (cost[i][2] < value && !visited[cost[i][0]]){
            value = cost[i][2];
            id = cost[i][0];
        }
    }
    return id;
}*/

ll smallest_cost(vector<bool> visited){
    ll id = 0;
    ll value = MAX_LL;
    for (int i = 0; i < N; i ++){
        if (!visited[i]){
            for (int j = 0; j < N; j ++){
                if (cost[i][j]){
                    value = cost[i][j];
                    id = i;
                }
            }
        }
    }
    return id;
}

void djikstra(int N){
    vector<bool> visited;
    ll it;
    
    visited.resize(N);
    visited.assign(N, false);

    it = 0;
    
    cout << smallest_cost(visited) << endl;

    while (count(visited.begin(), visited.end(), false) > 0){
        visited[it] = true;

        for (int j = 0; j < N; j ++){
            for (ll i = 0; i < N; i ++){
                ll sum = cost[it][j] + cost[j][i];
                if (sum < cost[it][i]){
                    if (cost[it][i] != MAX_LL){
                        cout << it+1 << " <-> " << i+1 << endl;
                        auto it_pair = find(roads.begin(), roads.end(), make_pair(it, i));
                        if (it_pair != roads.end()) {
                            roads.erase(it_pair);
                        } else {
                            it_pair = find(roads.begin(), roads.end(), make_pair(i, it));
                            if (it_pair != roads.end()) {
                                roads.erase(it_pair);
                            }
                        }
                    }
                    cost[i][it] = sum;
                    cost[it][i] = sum;
                }
            }
        }

        it = smallest_cost(visited);
    }



}
