#include <bits/stdc++.h>

using namespace std;

class Bitset{
public:
    vector<bool> bits;

    Bitset(){}//EMPTY constructor
    Bitset(const Bitset &other){this->bits = other.bits;}//copy constructor

    Bitset(int n){//make a Bitset og=f size n with all zeors
        for(int i = 0;i < n;i++)
            bits.push_back(0);
    }
    Bitset(string bit_set){
        for(int i = ( (int)bit_set.length() - 1);i >= 0;i--)
            bits.push_back(bit_set[i] - '0');
    }

    Bitset& operator=(const Bitset &other){
        this->bits = other.bits;
        return *this;
    }

    Bitset operator>>(int ammount){
        Bitset ret = *this;
        for(int i = 0;i < ammount;i++)
            ret.bits.erase(ret.bits.begin());
        return ret;
    }

    Bitset operator<<(int ammount){
        Bitset ret = *this;
        for(int i = 0;i < ammount;i++)
            ret.bits.insert(ret.bits.begin(),0);
        return ret;
    }

    Bitset operator&(Bitset &a){
        Bitset AND = Bitset( max( this->size(), a.size() ) );
        for(int i = 0;i < min(this->size(),a.size());i++)
            AND.bits[i] = this->bits[i] & a.bits[i];
        return AND;
    }

    Bitset operator|(Bitset &a){
        Bitset OR = Bitset(max(this->size(),a.size()));
        for(int i = 0;i < min(this->size(),a.size());i++)
            OR.bits[i] = this->bits[i] | a.bits[i];
        return OR;
    }

    Bitset operator~(){
        Bitset NOT = *this;
        for(int i = 0;i < this->size();i++)
            NOT.bits[i] = !this->bits[i];
        return NOT;
    }

    Bitset operator^(Bitset &a){
        Bitset XOR = Bitset(max(this->size(),a.size()));
        for(int i = 0;i < min(this->size(),a.size());i++)
            XOR.bits[i] = this->bits[i] ^ a.bits[i];
        return XOR;
    }

    int size(){ return (int)bits.size();}

    bool   get_bit_at(int i){return bits[i];}
    void   set_bit_at(int i){bits[i]  = 1;}
    void reset_bit_at(int i){bits[i]  = 0;}
    void  flip_bit_at(int i){bits[i]  = bits[i]^true;}

    friend ostream& operator<<(ostream& ,Bitset &);
    friend istream& operator>>(istream& ,Bitset &);
};

ostream& operator<<(ostream& sout,Bitset &a){
    for(int i = a.size() - 1;i >= 0;i--)
        sout << a.bits[i];
    return sout;
}

istream& operator>>(istream& sin,Bitset &a){
    string bits; sin >> bits;
    a = Bitset(bits);
    return sin;
}
/*
Bitset test(Bitset a){
    return ~a;
}

int main(int argc, char const *argv[]) {
    Bitset a,b;
    cin >> a >> b;
    cout << a << endl << b << endl;

    Bitset ans = a|b;
    cout << ans << endl;
    return 0;
}
*/
