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


#define CLOUD_SERVER_ADDRESS "xx.xxx.x.x"
#define FOG_SERVER_ADDRESS_1 "xx.xxx.x.x"
#define FOG_SERVER_ADDRESS_2 "xx.xxx.x.x"
#define FOG_SERVER_ADDRESS_3 "xx.xxx.x.x"
#define CLOUD_PORT 10248
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
    
    string Device = "sss";
    int parame_length = 512;
    
    char N_char[MSGSIZE],g_char[MSGSIZE],PKC_char[MSGSIZE];
    
    
    int sClient, sendNo;
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
    recv(sClient,N_char, MSGSIZE, 0);
    recv(sClient,g_char, MSGSIZE, 0);
    close(sClient);
    
    string N_str = InfoExtra(N_char);
    string g_str = InfoExtra(g_char);
    
    
    string Cloud_Parameter_path_str = "xxxxxx";
    
    fstream write_ptr;
    write_ptr.open(Cloud_Parameter_path_str,ios::out);
    write_ptr<<N_str<<endl;
    write_ptr<<g_str<<endl;
    write_ptr.close();
    
    mpz_t N;
    mpz_init(N);
    const char* N_ch = N_str.data();
    mpz_init_set_str(N,N_ch,10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param,N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing,param);
    const char* g_ch = g_str.data();
    element_t g,sk, pk;
    element_init_G1(g,pairing);
    element_init_G1(pk,pairing);
    element_init_Zr(sk,pairing);
    element_set_str(g,g_ch,10);
    
    mpz_t sk_t;
    mpz_init(sk_t);
    pbc_mpz_random(sk_t,N);
    element_set_mpz(sk,sk_t);
    element_pow_zn(pk,g,sk);
    
    char pk_char[MSGSIZE];
    element_snprint(pk_char, 2*parame_length, pk);
    string pk_str = ExtraG1(pk_char);
    append(pk_str, pk_char);
    
    char name_char[MSGSIZE];
    append(Device,name_char);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    sendNo=send(sClient,pk_char, MSGSIZE, 0);
    sendNo=send(sClient,name_char, MSGSIZE, 0);
    close(sClient);
    
    
    
    string Customer_path_str = "xxxxx";
    
    string sk_str = mpz_get_str(NULL,10,sk_t);
    write_ptr.open(Customer_path_str,ios::out);
    write_ptr<<sk_str<<endl;
    write_ptr<<pk_str<<endl;
    write_ptr.close();

    return 0;
}
