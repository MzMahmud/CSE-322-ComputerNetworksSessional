#include <bits/stdc++.h>

using namespace std;

//bit checking macros
#define CHECK(n,p)  ( n &   (1<<p) )
#define   SET(n,p)  ( n |=  (1<<p) )
#define RESET(n,p)  ( n &= ~(1<<p) )
#define  FLIP(n,p)  ( n ^=  (1<<p) )

//color defination (ASCII control characters)
#define RED         "\x1B[31m"
#define GRN         "\x1B[32m"
#define YEL         "\x1B[33m"
#define BLU         "\x1B[34m"
#define MAG         "\x1B[35m"
#define CYN         "\x1B[36m"
#define WHT         "\x1B[37m"
#define RESET_COL   "\x1B[0m"

// Cyclic Redundancy Check functions
// implemented in crc_proc.cpp
string              XOR(string a,string b);
string              modulo_2_division(string a,string b);
string              CRC_checksum(string data_frame,string generator_polynomial);
bool                CRC_checksum_matching(string data_frame,string generator_polynomial);


// implemented in hamming_proc.cpp
string              string_to_bitstring(string str);
string              bitstring_to_string(string bit_str);
string              transmit(string data_frame,double corruption_probability,map<int,bool>& is_corrupted);
void                print_corrupted_data(string data,map<int,bool>& is_corrupted);
void                print_corrupted_data(vector<string> data_block,map<int,bool>& is_corrupted);

int                 find_r(int m);
bool                is_power_of_2 (int n);
string              get_hamming_code(string bit_str,int m);
void                print_hamming_code(string hamming_code);
string              hamming_correction(string hamming_code,int m);
string              serialize_column_major(vector<string> data_block,int col);
vector<string>      deserialize_column_major(string data_frame,int row);
