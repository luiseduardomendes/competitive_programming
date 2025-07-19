#include <iostream>
#include <iomanip>

using namespace std;

int main(){ 
    double a, b;

    cin >> a >> b;

    cout << "MEDIA = " << fixed << setprecision(5) << (a*0.35 + b*0.75)/1.1 << endl;

    return 0;
}