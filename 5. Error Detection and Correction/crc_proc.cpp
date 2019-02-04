#include "defs.h"

string XOR(string a,string b){
    string x_or;
    x_or = (a.length() > b.length())?a:b;
    for(int i = 0;i < (int) min(a.length(),b.length());i++){
        bool a_bit = (bool) (a[i] - '0');
        bool b_bit = (bool) (b[i] - '0');
        bool x_bit = a_bit^b_bit;
        x_or[i] = (char) ( (int) x_bit + '0');
    }
    return x_or;
}

string modulo_2_division(string a,string b){
    //append zero at the end of gen pol for division
    int b_len = (int) b.length();

    int extra = (int) a.length()
              - (int) b.length();

    for(int i = 0;i < extra;i++)
        b += string("0");

    while( (int) a.length() >= b_len){
        if(a[0] == '1') a = XOR(a,b);
        a.erase(0,1);//erase first char
        b.pop_back();//erase last char
    }
    return a;
}

string CRC_checksum(string data_frame,string generator_polynomial){
    int checksum_bit_len
        = ( (int) generator_polynomial.length() ) - 1;

    //append zero at the end of data
    for(int i = 0;i < checksum_bit_len;i++)
        data_frame += string("0");

    return modulo_2_division(data_frame,generator_polynomial);
}

bool CRC_checksum_matching(string data_frame,string generator_polynomial){
    string rem = modulo_2_division(data_frame,generator_polynomial);
    for(int i = 0;i < (int) rem.size();i++)
        if(rem[i] != '0') return false;
    return true;
}
