#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <set>
#include <string>
#include <ctime>
#include <map>

#define dbg(x) cout << #x ": " << x << endl
#define MAX_INT 0x3F3F3F3F
using namespace std;
typedef long long ll;


map<ll, int> visited;

int cost_func (set<int> a, int M){
    auto it = --a.end();
    int acc = 0;
    int remaining = M;
    /*
    for (auto k : a)
        cout << k << " ";
    cout << endl;*/
    do{
        int div = remaining / *it;

        //dbg(div);

        remaining -= (div *  *it);
        acc += div;
        
        it--;
    } while (remaining != 0);
    //cout << "acc: " << acc << endl;

    return acc;
}

ll calc_index(set<int> cur_set, set<ll> indexes_weight, set<int>a){
    int index = 0;
    ll acc = 0;
    /*for(auto element_a : a){
        if(cur_set.find(element_a) != cur_set.end()){
            acc += pow(2, index);   
        }
        
        index++;
    }*/

    for (auto index : indexes_weight){
        acc += index;
    }
    
    return acc;
}

int rec(set<int> cur_set, set<int>a, set<ll> indexes_weight, int M){
    /*
    cout << "cur_set: ";
    for (auto k : cur_set)
        cout << k << " ";
    cout << endl;

    cout << "indexes: ";
    for (auto k : indexes_weight)
        cout << k << " ";
    cout << endl;*/

    ll visited_index;
/*
 * 
 * 
 * 
 *  
 */

    if (cur_set.size() == a.size())
        visited_index = pow(2, a.size()) - 1;
    else
        visited_index = calc_index(cur_set, indexes_weight, a);

    //cout << visited_index << endl;

    if(visited.find(visited_index) != visited.end()) {
        return visited[visited_index];
    }

    int min_cost = cost_func(cur_set, M);

    visited.emplace(visited_index, min_cost);
    /*for (auto v : visited){
        cout << "< " << v.first << ", " << v.second << " >; ";
    }
    cout << endl;*/
    int tmp_cost;
    int index = 0;

    for (auto it : a){
        set<int> next_set;
        set<ll> next_index;
        //copy(cur_set.begin(), cur_set.end(), next_set);
        for (auto value : cur_set){
            next_set.insert(value);
        }
        for (auto weight : indexes_weight){
            next_index.insert(weight);
        }

        if (find(cur_set.begin(), cur_set.end(), it) == cur_set.end()){
            next_set.insert(it);
            next_index.insert(pow(2, index));

            //cout << "chegou aqui" << endl;
            
            tmp_cost = rec(next_set, a, next_index, M);
            if (tmp_cost < min_cost){
                min_cost = tmp_cost;
            }
        }
        index ++;
    }
    return min_cost;
}

int main(){
    ll T;
    clock_t start, end;

    start = clock();

    cin >> T;
    
    for (int i = 0; i < T; i ++){
        set<int> a, cur_set;
        set<ll> indexes_set;
        int M;
        int N;
        int tmp, tmp1, tmp2;
        cin >> tmp1 >> tmp2;
        N = (tmp1);
        M = (tmp2);
        for (int j = 0; j < N; j ++){
            cin >> tmp;
            a.insert(tmp);
        }
        //copy(a.begin(), a.end(), cur_set.begin());
        
        visited.clear();
        cur_set.insert(*a.begin());
        indexes_set.insert(1);
        
        
        int min = rec(cur_set, a, indexes_set, M);

        cout << min << endl;
        
    }  

    end = clock();

    cout << "Execution time: " << double(end - start) / CLOCKS_PER_SEC  << endl;

    return 0;
}
