#include <bits/stdc++.h>
#define SIZE 1000000
#define getBit(n,pos)((bool)(n&(1<<pos)))
//gets the pos th bit of n.Counting rightmost bit 0
#define setBit(n,pos)(n = n|(1<<pos))
//sets the pos th bit 1 of n and stores it to n.Counting rightmost bit 0

using namespace std;

int state[(SIZE/64)+2];
/*only the odd numbers state is stored.
 *so total size needed would be SIZE/64
 *as one number stores 32 bits.
 */
vector <unsigned int> nPrime;
void bitSieve(int n){
    nPrime.push_back(2);

    int pos,limit = sqrt(n*1.0)+2;
    for(int i = 3;i <= n;i+=2){
        pos = (i+1)/2;
        /*i = 2k - 1,so the state of i is in
         *position (i+1)/2
         *now the pos th bit is found in the
         *pos/32 numbers (pos%32) bit.
         *pos/32 is same as pos>>5
         *pos%32 is same as pos&31
         *this is done because bitwise operation is fast.
         */
        if( !getBit(state[pos>>5],(pos&31)) ){
            nPrime.push_back(i);
            if(i < limit){
                for(int j = i*i;j <= n;j+=(2*i)){
                    pos = (j+1)/2;
                    setBit(state[pos>>5],(pos&31));
                }
            }
        }
    }
}

int main(){
    bitSieve(1000000);
    for(int i = 0;i < nPrime.size();i++)
        cout << nPrime[i] << endl;
    //cout << endl << "#primes: " << nPrime.size();
    return 0;
}
