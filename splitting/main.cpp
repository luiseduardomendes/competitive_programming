#include <iostream>
#include <vector>
using namespace std;

typedef long long ll;


int main(){
    ll N, tmp;
    ll acc, prev_acc, tmp1, tmp2;
    ll sum;
    double mean;
    
    while(cin >> N){
        vector<ll> tmp_C;
        sum = 0;
        
        for (int i = 0; i < N; i ++){
            cin >> tmp;
            sum += tmp;
            tmp_C.push_back(tmp);
        }

        mean = sum / 2.;
        acc = 0;
        prev_acc = 0;
        auto it = tmp_C.begin();
        while (acc < mean){
            prev_acc = acc;
            acc += *(it++);
            
        }
        tmp1 =  abs(2*mean - 2*prev_acc);
        tmp2 =  abs(2*mean - 2*acc);
        cout << min(tmp1, tmp2) << endl;
    }

    return 0;
}
