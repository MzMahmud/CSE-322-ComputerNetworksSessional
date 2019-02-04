#include "defs.h"

string string_to_bitstring(string str){
    string bit_str("");
    for(int i = 0;i < (int) str.length();i++)
        bit_str += string(bitset<8>(str[i]).to_string());
    return bit_str;
}

string bitstring_to_string(string bit_str){
    string str("");
    for(int i = 0;i < (int) bit_str.length();i += 8){
        char cstr[10];
        sprintf(cstr,"%c",stoi(bit_str.substr(i,8),nullptr,2));
        str += string(cstr);
    }
    return str;
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
        }else hamming_code += string("0");
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
    for(int i = 1;i <= (int) hamming_code.length();i++){
        if(is_power_of_2(i))
            //makes the text GREEN in terminal
            printf(GRN "%c" RESET_COL, hamming_code[i-1]);
        else
            printf("%c", hamming_code[i-1]);
    }
    cout << endl;
}

string hamming_correction(string hamming_code,int m){
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
        for(int j = 0;j < (int) data_block.size();j++)
            serialized += data_block[j].substr(i,1);
    return serialized;
}

vector<string> deserialize_column_major(string data_frame,int row){
    vector<string> data_block;
    for(int i = 0;i < row;i++)
        data_block.push_back(string(""));

    for(int i = 0;i < (int) data_frame.size();i++)
        data_block[i%row] += data_frame.substr(i,1);

    return data_block;
}

string transmit(string data_frame,double corruption_probability,map<int,bool>& is_corrupted){
    //done for generating random number
    random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<> dis(0.0, 1.0); //random number in [0,1]

    for(int i = 0;i < (int) data_frame.size();i++){
        double random_number = dis(gen);
        if(random_number < corruption_probability){
            data_frame[i] = (data_frame[i] == '0')?'1':'0';//flips bits
            is_corrupted[i] = true;
        }
    }
    return data_frame;
}

void print_corrupted_data(string data,map<int,bool>& is_corrupted){
    for(int i = 0;i < (int) data.length();i++){
        if(is_corrupted[i])
            //makes the color red in terminal
            printf(RED "%c" RESET_COL,data[i]);
        else
            printf("%c",data[i]);
    }
    cout << endl;
}

void print_corrupted_data(vector<string> data_block,map<int,bool>& is_corrupted){
    int row = (int) data_block.size();
    for(int i = 0;i < row;i++){
        for(int j = 0;j < (int) data_block[i].length();j++){
            //column-major order to linear index
            //(i,j) -> j*row + i
            if(is_corrupted[j*row + i])
                //makes the color red in terminal
                printf(RED "%c" RESET_COL,data_block[i][j]);
            else
                printf("%c",data_block[i][j]);
        }
        cout << endl;
    }
    cout << endl;
}
