//
//  main.cpp
//  Client
//
//  Created by 宋明洋.
//
//#ifdef _MSC_VER
//#ifdef _WIN64
//#include <WinSock2.h>
//#elif _WIN32
#include <sys/socket.h>
//#endif
//#endif

#include <fstream>
#include <iostream>
#include <gmpxx.h>
#include<unistd.h>
#include <gmp.h>
#include <stdio.h>
#include <string.h>
#include "/usr/local/include/pbc/pbc.h"
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <ctime>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <cmath>

#pragma comment(lib,"ws2_32.lib")
#include <assert.h>

using namespace std;

string binaryToHex(const string& binaryStr){
    string ret;
    static const char *hex = "0123456789ABCDEF";
    for(auto c:binaryStr){
        ret.push_back(hex[(c>>4)&0xf]);
        ret.push_back(hex[c&0xf]);
    }
    return ret;
}

unsigned char* str2hex(char *str) {
    unsigned char *ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    assert((str_len % 2) == 0);
    ret = (unsigned char *)malloc(str_len / 2);
    for (i = 0; i <str_len; i = i + 2) {
        sscanf(str + i, "%2hhx", &ret[i / 2]);
    }
    return ret;
}

string HexToStr(std::string str){
    std::string hex = str;
    long len = hex.length();
    std::string newString;
    for (long i = 0; i<len; i += 2)
    {
        std::string byte = hex.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    return newString;
}

string unsignedCharToHexString(unsigned char ch) {
    const char hex_chars[] = "0123456789abcdef";
    unsigned int highHalfByte = (ch>> 4) & 0x0f;
    unsigned int lowHalfByte = (ch &0x0f);
    string result;
    result += hex_chars[highHalfByte];
    result += hex_chars[lowHalfByte];
    return result;
}


#define CLOUD_SERVER_ADDRESS "x.x.x.x"
#define FOG_SERVER_ADDRESS_1 "x.x.x.x"
#define FOG_SERVER_ADDRESS_2 "x.x.x.x"
#define FOG_SERVER_ADDRESS_3 "x.x.x.x"
#define CLOUD_PORT 10110
#define MSGSIZE 2048
#define QUEUE_SIZE 25
string InfoExtra(char MSG[1024*2]){
    string res = "";
    for(int i=0;i<MSGSIZE;i++){
        if(MSG[i]=='!'){
            break;
        }
        else{
            res+=MSG[i];
        }
    }
    return res;
}
void append(string s, char* res){
    for(int i=0;i<s.size();i++){
        res[i] = s[i];
    }
    res[s.size()]='!';
}
string ExtraZr(char phia_char[MSGSIZE]){
    string phia_str;
    for(int i=0;i<MSGSIZE;i++){
        if(phia_char[i]<='9'&&phia_char[i]>='0'){
            phia_str += phia_char[i];
        }
        else{
            break;
        }
    }
    return phia_str;
    
}
string ExtraG1(char fya_char[MSGSIZE]){
    string fya_str;
    for(int i=0;i<MSGSIZE;i++){
        if((fya_char[i]<='9'&&fya_char[i]>='0')||fya_char[i]==']'||fya_char[i]=='['||fya_char[i]==' '||fya_char[i]==','){
            fya_str+=fya_char[i];
        }
        else{
            break;
        }
    }
    return fya_str;
}

int main(int argc, const char * argv[]) {
    
    string Customer = "xxx";
    int parame_length = 512;
    const int BlockSize = 1024*64;
    string filename="x";
    string myord="x";
    /*————————————读取云参数——————————*/
    string Cloud_pp_path = "xxxxx";
    fstream Read_ptr;
    string N_str, g_str;
    Read_ptr.open(Cloud_pp_path);
    getline(Read_ptr,N_str);
    getline(Read_ptr,g_str);
    Read_ptr.close();
    
    /*————————————读参数——————————*/
    string self_path = "xxxxx";
    string sk_str;
    Read_ptr.open(self_path);
    getline(Read_ptr,sk_str);
    Read_ptr.close();
    
    
    
    /*————————————参数初始化——————————*/
    mpz_t N;
    mpz_init(N);
    const char* N_char = N_str.data();
    const char* g_char = g_str.data();
    const char* sk_char = sk_str.data();
    mpz_init_set_str(N,N_char,10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param,N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing,param);
    element_t g,sk;
    element_init_G1(g,pairing);
    element_init_Zr(sk,pairing);
    
    element_set_str(g,g_char,10);
    element_set_str(sk,sk_char,10);


    char szMessage[MSGSIZE];
    
    int sClient;
    struct sockaddr_in addrSer, addrCli;
    addrSer.sin_family =AF_INET;
    addrSer.sin_port =  htons(CLOUD_PORT);
    addrSer.sin_addr.s_addr = inet_addr(CLOUD_SERVER_ADDRESS);
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    socklen_t addrlen = sizeof(struct sockaddr);
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    cout<<"连接成功"<<endl;
    recv(sClient,szMessage, MSGSIZE, 0);
    close(sClient);
    
    string blocknum_str = InfoExtra(szMessage);
   
    
    
    int blocknum = atoi(blocknum_str.c_str());
    for(int l=0;l<blocknum;l++){
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
        }
        recv(sClient,szMessage, MSGSIZE, 0);
        close(sClient);
        string cipher_len_str = InfoExtra(szMessage);
        int send_time = ceil(double(atoi(cipher_len_str.c_str()))/double(MSGSIZE));
        
        int Len = atoi(cipher_len_str.c_str());
        string BlockCiphertext;
        for(int i=0;i<send_time;i++){
            sClient = socket(AF_INET,SOCK_STREAM,0);
            if(sClient == -1){
                cout<<"客户端初始化步骤1失败"<<endl;
                return 0;
            }
            bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
            while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
            }
            recv(sClient,szMessage, MSGSIZE, 0);
            close(sClient);
            if(i==send_time-1){
                for (int j = 0; j< (Len - i*MSGSIZE); j++) {
                    BlockCiphertext += szMessage[j];
                }
            }
            else{
                for (int j = 0; j <MSGSIZE; j++) {
                    BlockCiphertext += szMessage[j];
                }
            }
        }
        
        cout<<BlockCiphertext.size()<<endl;
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
        }
        recv(sClient,szMessage, MSGSIZE, 0);
        close(sClient);
        string left_str = InfoExtra(szMessage);
        
        sClient = socket(AF_INET,SOCK_STREAM,0);
        if(sClient == -1){
            cout<<"客户端初始化步骤1失败"<<endl;
            return 0;
        }
        bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
        while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
        }
        recv(sClient,szMessage, MSGSIZE, 0);
        close(sClient);
        string right_str=InfoExtra(szMessage);
        
        //cout<<left_str<<endl;
        //cout<<right_str<<endl;
        const char* left_ptr = left_str.data();
        const char* right_ptr = right_str.data();
        element_t left,right;
        element_init_G1(left,pairing);
        element_init_G1(right,pairing);
        element_set_str(left,left_ptr,10);
        element_set_str(right,right_ptr,10);
        
        element_pow_zn(right, right, sk);
        element_div(left,left,right);
       
        element_snprint(szMessage, 2*parame_length,left);
        string EncSK_str = ExtraG1(szMessage);
        char* EncSK_char_ptr = new char[EncSK_str.size()+1];
        for(int i=0;i<EncSK_str.size();i++){
            EncSK_char_ptr[i] = EncSK_str[i];
        }
        EncSK_char_ptr[EncSK_str.size()] = '\0';
        unsigned char FinalEncSK_UC[32];
        SHA256_CTX ctx;
        SHA256_Init(&ctx);
        SHA256_Update(&ctx,EncSK_char_ptr,strlen(EncSK_char_ptr));
        SHA256_Final(FinalEncSK_UC,&ctx);
        string temp_hash;
        string hash_16;
        for(int i=0;i<32;i++){
            temp_hash+=FinalEncSK_UC[i];
        }
        hash_16 = binaryToHex(temp_hash);
        string FinalEncSK = hash_16.substr(0,64);
        unsigned char block_key[64];
        for(int i=0;i<64;i++){
            block_key[i] = FinalEncSK[i];
            cout<<block_key[i];
        }
        cout<<endl;
        AES_KEY key_e;
        AES_set_decrypt_key(block_key,128,&key_e);
        
       // cout<<BlockCiphertext<<endl;
        string ciphertext_block_str = HexToStr(BlockCiphertext);
        
        //cout<<ciphertext_block_str.size()<<endl;
        unsigned char* cipher_block= new unsigned char[BlockSize];
        for(int i=0;i<BlockSize;i++){
            cipher_block[i]=(unsigned char)ciphertext_block_str[i];
        }
        unsigned char* out_block = new unsigned char[BlockSize];
        for(int i=0; i<BlockSize/16;i++){
            AES_ecb_encrypt( cipher_block+i*16,out_block+i*16, &key_e, AES_DECRYPT);
        }

        string result;
        for(int i=0;i<BlockSize;i++){
            result+=out_block[i];
        }
        cout<<result<<endl;
        delete[] cipher_block;
        delete[] out_block;
    }
    
    
  
    return 0;
}
