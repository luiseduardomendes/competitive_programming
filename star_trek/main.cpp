/*Input

The first input line consists of a single integer 
N (1 ≤ N ≤ 106), which represents the number of Stars. The second input line consists of 
N integers, such that the ith integer, 
Xi (1 ≤ Xi ≤ 106), represents the initial number of sheep in Star i.
Output

Output a line containing 
two integers, so that the first represents the number of Stars attacked by the mad brother and the second represents the total number of non-stolen sheep.
*/

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main(){
    long int N;
    long int temp;
    long int star = 1 - 1;
    long int stolen_sheeps = 0;
    long int total_sheeps = 0;
    vector<long int> X;
    vector<int> attacked;


    cin >> N; 

    for (int i = 1; i <= N; i ++){
        cin >> temp;
        total_sheeps += temp;
        X.push_back(temp);
        attacked.push_back(0);
    }

    do {
        if (X[star] % 2 == 0){
            if (X[star] != 0){
                X[star] --;
                stolen_sheeps ++;
                attacked[star] = 1;
            }
            star = star - 1;
        } else {
            if (X[star] != 0){
                X[star] --;
                stolen_sheeps ++;
                attacked[star] = 1;
            }
            star = star + 1;
        }
    } while (star >= 0 && star < N);
    
    cout << count(attacked.begin(), attacked.end(), 1) << " " << total_sheeps - stolen_sheeps << endl;
    
    
    return 0;
}