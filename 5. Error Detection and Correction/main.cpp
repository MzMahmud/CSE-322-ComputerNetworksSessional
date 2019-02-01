#include <bits/stdc++.h>

//#include "Bitset.cpp"
#define CHECK(n,p) ( n &   (1<<p) )
#define   SET(n,p) ( n |=  (1<<p) )
#define RESET(n,p) ( n &= ~(1<<p) )
#define  FLIP(n,p) ( n ^=  (1<<p) )
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET_COL "\x1B[0m"

using namespace std;

string string_to_bitstring(string str){
    string bit_str("");
    for(int i = 0;i < (int)str.length();i++)
        bit_str += string(bitset<8>(str[i]).to_string());
    return bit_str;
}

int find_r(int m){
    // (m+r+1) <= 2^r
    int r = 1;
    while( (m+r+1) > (1<<r) ) r++;
    return r;
}
bool is_power_of_2 (int n){
    if(!n) return false;
    int p = (int)sizeof(n)*8 - 1;
    while(p--){
        if(CHECK(n,p)) {RESET(n,p);break;}
    }
    return !n;
}
string get_hamming_code(string bit_str,int m){
    int n = m + find_r(m);//total code length
    string hamming_code("");
    int ind = 1,bs_i = 0;
    while(ind <= n){
        if(!is_power_of_2(ind)){
            hamming_code += bit_str.substr(bs_i,1);
            bs_i++;
        }else hamming_code += string("0");//DEBUG: string("2");
        ind++;
    }

    for(int i = 1;i <= n;i++){
        if(!is_power_of_2(i)) continue;

        bool val = 0;
        for(int j = i + 1;j <= n;j++){
            if(i & j){//i has the same set bit as i
                bool bit = (bool) ( hamming_code[j-1] - '0');//j-1 because of zero base index
                val = val^bit;
            }
        }
        hamming_code[i-1] = (int)val + '0';//make to character
    }
    return hamming_code;
}

void print_hamming_code(string hamming_code){
    for(int i = 1;i <= hamming_code.length();i++){
        if(is_power_of_2(i))
            printf(GRN "%c" RESET_COL, hamming_code[i-1]);
        else
            printf("%c", hamming_code[i-1]);
    }
    cout << endl;
}

string hamming_correcsion(string hamming_code,int m){
    int n = m + find_r(m);//total code length

    int error_bit_index = 0;
    for(int i = 1;i <= n;i++){
        if(!is_power_of_2(i)) continue;

        bool val = 0;
        for(int j = i;j <= n;j++){
            if(i & j){//i has the same set bit as i
                bool bit = (bool) ( hamming_code[j-1] - '0');//j-1 because of zero base index
                val = val^bit;
            }
        }
        if(val)
            error_bit_index += i;
    }

    if(1 <= error_bit_index && error_bit_index <= n)
        hamming_code[error_bit_index - 1] = (hamming_code[error_bit_index - 1] == '0')?'1':'0';

    // print_hamming_code(hamming_code);
    string bit_str("");
    for(int i = 1;i <= n;i++){
        if(!is_power_of_2(i))
            bit_str += hamming_code.substr(i-1,1);
    }
    return bit_str;
}

string serialize_column_major(vector<string> data_block,int col){
    string serialized("");
    for(int i = 0;i < col;i++)
        for(int j = 0;j < data_block.size();j++)
            serialized += data_block[j].substr(i,1);
    return serialized;
}

vector<string> deserialize_column_major(string data_frame,int row){
    vector<string> data_block;

    for(int i = 0;i < row;i++)
        data_block.push_back(string(""));

    for(int i = 0;i < data_frame.size();i++)
        data_block[i%row] += data_frame.substr(i,1);

    return data_block;
}

// TODO: implement CRC Checksum
string CRC_checksum(string data_frame,string generator_polynomial){
    return generator_polynomial.substr(1,string::npos);
}



map<int,bool> is_corrupted;

// TODO: implement corrupsion simulation
string transmit(string data_frame,double p){
    srand(time(NULL));
    for(int i = 0;i < data_frame.size();i++){
        double random_number = rand()/(RAND_MAX + .0);
        if(random_number < p){
            data_frame[i] = (data_frame[i] == '0')?'1':'0';//flips bits
            is_corrupted[i] = true;
            //cout << "red " << i << endl;
        }
    }
    return data_frame;
}
void print_corrupted_data(string data){
    for(int i = 0;i < data.length();i++){
        if(is_corrupted[i])
            printf(RED "%c" RESET_COL,data[i]);
        else
            printf("%c",data[i]);
    }
    cout << endl;
}

void print_corrupted_data(vector<string> data_block){
    int row = (int) data_block.size();
    for(int i = 0;i < row;i++){
        for(int j = 0;j < data_block[i].length();j++){
            if(is_corrupted[j*row + i])
                printf(RED "%c" RESET_COL,data_block[i][j]);
            else
                printf("%c",data_block[i][j]);
        }
        cout << endl;
    }
    cout << endl;
}

// TODO: implement CRC_checksum_matching
bool CRC_checksum_matching(string data_frame,string generator_polynomial){
    return false;
}


int main (){
    freopen("input.in","r",stdin);

    string data_string,generator_polynomial;
    int m;double p;

    //INPUT:
    cout << "enter data string: ";
    getline(cin,data_string);

    cout << "enter number of bytes in a row (m): ";
    cin >> m;

    cout << "enter probability of each bit being toggled (p) in range [0,1]: ";
    cin >> p;

    cout << "enter generator polynomial: ";
    cin >> generator_polynomial;

    //Pad string
    cout << "\n\nInput data string         : " << data_string << "\n";

    int len = (int) data_string.length();
    int rem = len % m;
    if(rem){//not a multiple of m. Pad with '~'
        rem = m - rem;
        while(rem--) data_string += string("~");
    }
    cout << "data string after padding : " << data_string << "\n\n";

    len = (int) data_string.length();
    vector<string> data_block;
    for(int i = 0;i < len;i += m){
        //DEBUG: string sub_str = data_string.substr(i,m);
        //DEBUG: << sub_str << endl;
        data_block.push_back(data_string.substr(i,m));
    }

    cout << "data block (m characters per row) : " << endl;
    for(int i = 0;i < data_block.size();i++){
        data_block[i] = string_to_bitstring(data_block[i]);
        cout <<  data_block[i] << endl;
    }

    cout << endl << "data block after adding check bits : " << data_string << endl;
    for(int i = 0;i < data_block.size();i++){
        data_block[i] = get_hamming_code(data_block[i],8*m);//m here is in bytes.func expects bits
        print_hamming_code(data_block[i]);
    }

    int hamming_code_length = 8*m + find_r(8*m);
    string data_frame = serialize_column_major(data_block,hamming_code_length);
    cout << endl << "data bits after column-wise serialization :\n"
                 << data_frame << endl;

    cout << endl << "data bits after appending CRC Checksum :\n";
    string crc_checksum = CRC_checksum(data_frame,generator_polynomial);
    printf("%s", data_frame.c_str());
    printf(CYN "%s\n\n" RESET_COL, crc_checksum.c_str());
    data_frame = data_frame + crc_checksum;

    //transmissit with p curruption probability and recieve
    data_frame = transmit(data_frame,p);

    cout << endl << "recieved frame:\n";
    print_corrupted_data(data_frame);

    cout << endl << "result of CRC checksum matching: ";
    if(!CRC_checksum_matching(data_frame,generator_polynomial))
        cout << "error detected" << endl;
    else
        cout << "no error detected" << endl;

    // cout << "data_frame.length()  " << data_frame.length() << endl;
    //remove CRC_checksum
    int crc_checksum_len = ( (int) generator_polynomial.length() ) - 1;
    while(crc_checksum_len--) data_frame.pop_back();
    //
    // cout << "data_frame.length()  " << data_frame.length() << endl;
    // cout << data_frame << endl;

    int row =  ( (int)data_frame.length() ) / hamming_code_length;
    data_block = deserialize_column_major(data_frame,row);

    cout << endl << "data block after removing CRC checksum bits: \n";
    print_corrupted_data(data_block);
    cout << data_block.size() << endl;

    cout << endl << "data block after removing check bits: \n";
    for(int i = 0;i < data_block.size();i++){
        data_block[i] = hamming_correcsion(data_block[i],8*m);
        cout << data_block[i] << endl;
    }



    return 0;
}
