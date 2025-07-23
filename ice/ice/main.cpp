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

int min_blocks(set<int> blocks, int M){
    vector<ll> dp(M+1, MAX_INT);
    dp[0] = 0;

    for (ll i = 0; i <= M; i++){
        for (int block : blocks){
            // o if apenas otimiza, poderia funcionar sem, mas poderia dar TLE
            if (block <= i && dp[i-block] != MAX_INT){
                dp[i] = min(dp[i], dp[i-block] + 1);
            }
        }
    }

    return dp[M];
    /*
     * o algoritmo verifica primeiro tamanhos menores.
     * tamanho 1 = 1 ( 1 bloco )
     * tamanho 2 = 1 + 1 ( 2 blocos ) 
     *      aqui atualiza com dp[2] = dp[1] + 1
     * tamanho 2 = 2 ( 1 bloco )
     *      aqui atualiza com dp[2] = dp[0] + 1
     * tamanho 3 = 2 + 1 ( 2 blocos )
     *      aqui atualiza com dp[3] = dp[2] + 1
     * tamanho 3 = 3 ( 1 bloco )
     *      aqui atualiza com dp[3] = dp[0] + 1
     * e assim por diante
     */
}

int main(){
    ll T;
    clock_t start, end;
    
    start = clock();
    
    cin >> T;
    for (int i = 0; i < T; i ++){
        set<int> a;
        int M, N;
        int tmp;
        cin >> N >> M;
        for (int j = 0; j < N; j ++){
            cin >> tmp;
            a.insert(tmp);
        }
        cout << min_blocks(a, M) << endl;
        
    }  

    end = clock();

    cout << "Execution time: " << double(end - start) / CLOCKS_PER_SEC  << endl;

    return 0;
}
