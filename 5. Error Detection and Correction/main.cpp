#include "defs.h"

int main (){
    freopen("input.in","r",stdin);

    string data_string,generator_polynomial;
    int m;double p;

    //INPUT:
    cout << "enter data string: ";
    getline(cin,data_string);
    cout << data_string << endl;

    cout << "enter number of bytes in a row (m): ";
    cin >>  m;
    cout << m << endl;

    cout << "enter probability of each bit being toggled (p) in range [0,1]: ";
    cin  >> p;
    cout << p << endl;

    cout << "enter generator polynomial: ";
    cin  >> generator_polynomial;
    cout << generator_polynomial << endl;

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
    for(int i = 0;i < len;i += m)
        data_block.push_back(data_string.substr(i,m));

    cout << "data block (m characters per row) : " << endl;
    for(int i = 0;i < (int) data_block.size();i++){
        data_block[i] = string_to_bitstring(data_block[i]);
        cout <<  data_block[i] << endl;
    }

    cout << endl << "data block after adding check bits : " << data_string << endl;
    for(int i = 0;i < (int) data_block.size();i++){
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


    map<int,bool> is_corrupted;
    //transmissit with p curruption probability and recieve
    data_frame = transmit(data_frame,p,is_corrupted);

    cout << endl << "recieved frame:\n";
    print_corrupted_data(data_frame,is_corrupted);

    cout << endl << "result of CRC checksum matching: ";
    if(!CRC_checksum_matching(data_frame,generator_polynomial))
        cout << "error detected" << endl;
    else
        cout << "no error detected" << endl;


    //remove CRC_checksum
    int crc_checksum_len = ( (int) generator_polynomial.length() ) - 1;
    while(crc_checksum_len--) data_frame.pop_back();

    int row =  ( (int)data_frame.length() ) / hamming_code_length;
    data_block = deserialize_column_major(data_frame,row);

    cout << endl << "data block after removing CRC checksum bits: \n";
    print_corrupted_data(data_block,is_corrupted);

    cout << endl << "data block after removing check bits: \n";
    for(int i = 0;i < (int) data_block.size();i++){
        data_block[i] = hamming_correction(data_block[i],8*m);
        cout << data_block[i] << endl;
    }

    string output_frame("");
    for (int i = 0; i < (int) data_block.size(); i++)
        output_frame += data_block[i];

    output_frame = bitstring_to_string(output_frame);
    cout << endl << "output frame : " << output_frame << endl;

    return 0;
}
