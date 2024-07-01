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


#define CLOUD_SERVER_ADDRESS "x.x.x.x"
#define FOG_SERVER_ADDRESS_1 "x.x.x.x"
#define FOG_SERVER_ADDRESS_2 "x.x.x.x"
#define FOG_SERVER_ADDRESS_3 "x.x.x.x"
#define FOG_PORT 10249
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
    
    string Device = "Device1";
    int parame_length = 512;
    
    char Rd_char[MSGSIZE],u_char[MSGSIZE],v_char[MSGSIZE],h_char[MSGSIZE],w_char[MSGSIZE],pkf_char[MSGSIZE],x_char[MSGSIZE],theta_char[MSGSIZE];
    
    
    int sClient, sendNo;
    struct sockaddr_in addrSer, addrCli;
    addrSer.sin_family =AF_INET;
    addrSer.sin_port =  htons(FOG_PORT);
    addrSer.sin_addr.s_addr = inet_addr(FOG_SERVER_ADDRESS_1);
    
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
    recv(sClient,v_char, MSGSIZE, 0);
    recv(sClient,pkf_char, MSGSIZE, 0);
    close(sClient);
    
    
    string v_str = InfoExtra(v_char);
    string pkf_str = InfoExtra(pkf_char);
    
    
    string fog_Parameter_path_str = "xxxxx";
    
    fstream write_ptr;
    write_ptr.open(fog_Parameter_path_str,ios::out);
    write_ptr<<v_str<<endl;
    write_ptr<<pkf_str<<endl;
    write_ptr.close();
    
    
    string Rd_path_str ="xxxxx";
    
    string sk_str,Rd_str;
    fstream Read_ptr;
    Read_ptr.open(Rd_path_str);
    getline(Read_ptr,sk_str);
    getline(Read_ptr,Rd_str);
    Read_ptr.close();
    

    append(Rd_str,Rd_char);
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    sendNo=send(sClient,Rd_char, MSGSIZE, 0);
    close(sClient);
    
    
  
    return 0;
}
