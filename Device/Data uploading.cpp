//
//  main.cpp
//  Client
//
//  Created by 宋明洋 on 2022/1/25.
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


#define CLOUD_SERVER_ADDRESS "192.168.10.31"
#define FOG_SERVER_ADDRESS_1 "192.168.10.31"
#define FOG_SERVER_ADDRESS_2 "192.168.10.31"
#define FOG_SERVER_ADDRESS_3 "192.168.10.31"
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
    
    string Device = "xxx";
    int parame_length = 512;
    const int BlockSize = 1024*64;
    string filename="xxx";
    string myord="xx";
    /*————————————读取云参数——————————*/
    string Cloud_pp_path = "xxxxxx";
    fstream Read_ptr;
    string N_str, g_str,h_str,w_str,u_str;
    Read_ptr.open(Cloud_pp_path);
    getline(Read_ptr,N_str);
    getline(Read_ptr,g_str);
    getline(Read_ptr,h_str);
    getline(Read_ptr,w_str);
    getline(Read_ptr,u_str);
    Read_ptr.close();
    
    /*————————————读取雾参数——————————*/
    string fog_pp_path = "xxxxx";
    string v_str,pkf_str;
    Read_ptr.open(fog_pp_path);
    getline(Read_ptr,v_str);
    getline(Read_ptr,pkf_str);
    Read_ptr.close();
    
    /*————————————读取自身参数——————————*/
    string self_pp_path = "xxxxxx";
    string skd_str, Rd_str;
    Read_ptr.open(self_pp_path);
    getline(Read_ptr,skd_str);
    getline(Read_ptr,Rd_str);
    Read_ptr.close();
    
    /*————————————读取自身参数——————————*/
    string signKey_pp_path = "xxxxx";
    string x_str, theta_str;
    Read_ptr.open(signKey_pp_path);
    getline(Read_ptr,x_str);
    getline(Read_ptr,theta_str);
    Read_ptr.close();
    
    /*————————————读取customer参数——————————*/
    string Customer_pp_path = "xxxxxx";
    string cpk_str;
    Read_ptr.open(Customer_pp_path);
    getline(Read_ptr,cpk_str);
    Read_ptr.close();
    
    /*————————————参数初始化——————————*/
    mpz_t N;
    mpz_init(N);
    const char* N_char = N_str.data();
    const char* g_char = g_str.data();
    const char* u_ptr = u_str.data();
    const char* v_ptr = v_str.data();
    const char* h_ptr = h_str.data();
    const char* w_ptr = w_str.data();
    const char* pkf_ptr = pkf_str.data();
    const char* skd_ptr = skd_str.data();
    const char* Rd_ptr = Rd_str.data();
    const char* x_ptr = x_str.data();
    const char* theta_ptr = theta_str.data();
    const char* cpk_ptr = cpk_str.data();
    mpz_init_set_str(N,N_char,10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param,N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing,param);
    element_t g,pkc,u,v,h,w,pkf,skd,Rd,x,theta,cpk;
    element_init_G1(g,pairing);
    element_init_G1(pkc,pairing);
    element_init_G1(u,pairing);
    element_init_G1(v,pairing);
    element_init_G1(h,pairing);
    element_init_G1(w,pairing);
    element_init_G1(pkf,pairing);
    element_init_Zr(skd,pairing);
    element_init_G1(Rd,pairing);
    element_init_Zr(x,pairing);
    element_init_G1(theta,pairing);
    element_init_G1(cpk,pairing);
    
    element_set_str(g,g_char,10);
    element_set_str(u,u_ptr,10);
    element_set_str(v,v_ptr,10);
    element_set_str(h,h_ptr,10);
    element_set_str(w,w_ptr,10);
    element_set_str(pkf,pkf_ptr,10);
    element_set_str(skd,skd_ptr,10);
    element_set_str(Rd,Rd_ptr,10);
    element_set_str(x,x_ptr,10);
    element_set_str(theta,theta_ptr,10);
    element_set_str(cpk,cpk_ptr,10);
    
    string Plaintext_path = "xxxxxx";
    char ch;
    Read_ptr.open(Plaintext_path);
    int chNum=0,BlockNum;
    while(Read_ptr.get(ch)){
        chNum++;
    }
    Read_ptr.close();
    BlockNum=ceil(double(chNum)/double(BlockSize));
    string BlockNum_str = to_string(BlockNum);
    char szMessage[MSGSIZE],File_name[MSGSIZE],ord_char[MSGSIZE];
    append(BlockNum_str,szMessage);
    append(filename,File_name);
    append(myord,ord_char);
    
    int sClient;
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
    send(sClient,ord_char, MSGSIZE, 0);
    close(sClient);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    send(sClient,File_name, MSGSIZE, 0);
    send(sClient,szMessage, MSGSIZE, 0);
    close(sClient);
    
    Read_ptr.open(Plaintext_path);
    int times=0;
    int now_charNum=0;
    int current_char_num = 0;
    string DataBuff;
    mpz_t block_h;
    mpz_init(block_h);
    string All_cipher;
    while(Read_ptr.get(ch)){//分块加密和传输
        now_charNum++;
        DataBuff+=ch;
        current_char_num++;
        if(current_char_num==BlockSize||now_charNum==chNum){
            times++;
            cout<<times<<endl;
            /*——————计算块哈希————————*/
            char* DataBuff_char = new char[current_char_num+1];
            for(int i=0;i<current_char_num;i++){
                DataBuff_char[i] = DataBuff[i];
            }
            DataBuff_char[current_char_num]='\n';
            unsigned char SHA256OUT_Block[32];
            char* block_hash_char = new char[32];
            SHA256_CTX ctx1;
            SHA256_Init(&ctx1);
            SHA256_Update(&ctx1,DataBuff_char,strlen(DataBuff_char));
            SHA256_Final(SHA256OUT_Block,&ctx1);
            string temp_hash;
            for(int i=0;i<32;i++){
                block_hash_char[i]=SHA256OUT_Block[i];
                temp_hash+=SHA256OUT_Block[i];
            }
            element_t BlockHash;
            element_init_Zr(BlockHash,pairing);
            string hash_16 = binaryToHex(temp_hash);
            string Finalhash= hash_16.substr(0,64);
            const char* block_h_char= Finalhash.data();
            mpz_init_set_str(block_h,block_h_char,16);
            element_set_mpz(BlockHash,block_h);
            delete[] DataBuff_char;
            delete[] block_hash_char;
            
            //element_printf("BlockHash: %B\n",BlockHash);
            /*——————加密密钥生成————————*/
            element_t org,r,le,ri;
            element_init_G1(org,pairing);
            element_init_Zr(r,pairing);
            element_init_G1(le,pairing);
            element_init_G1(ri,pairing);
            element_random(r);
            element_pow_zn(ri,g,r);
            element_add(BlockHash,BlockHash,skd);
            element_pow_zn(le,g,BlockHash);
            element_mul(org,le,ri);
            element_mul(org,org,Rd);
            
            //element_printf("org: %B\n",org);
            
            element_snprint(szMessage, 2*parame_length, org);
            string org_str = ExtraG1(szMessage);
            append(org_str,szMessage);
            
            sClient = socket(AF_INET,SOCK_STREAM,0);
            if(sClient == -1){
                cout<<"客户端初始化步骤1失败"<<endl;
                return 0;
            }
            bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
            while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
            }
            send(sClient,szMessage, MSGSIZE, 0);
            close(sClient);
            
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
            
            string sign_str = InfoExtra(szMessage);
            const char* sign_ptr = sign_str.data();
            element_t sign,down;
            element_init_G1(sign,pairing);
            element_init_G1(down,pairing);
            element_set_str(sign,sign_ptr,10);
            element_pow_zn(down, pkf, r);
            element_div(sign,sign,down);
            
            element_printf("key: %B\n",sign);
            
            //element_printf("sign: %B\n",sign);
            /*————————key sharing————————*/
            element_t ckl,ckr,r1;
            element_init_G1(ckl,pairing);
            element_init_G1(ckr,pairing);
            element_init_Zr(r1,pairing);
            element_random(r1);
            element_pow_zn(ckr,pkc,r1);
            element_pow_zn(ckl,cpk,r1);
            element_mul(ckl,ckl,sign);
            
            /*————————发送key Shares——————————*/
            char ckl_char[MSGSIZE],ckr_char[MSGSIZE];
            element_snprint(ckl_char, 2*parame_length,ckl);
            element_snprint(ckr_char, 2*parame_length,ckr);
            string ckl_str=ExtraG1(ckl_char);
            string ckr_str=ExtraG1(ckr_char);
            append(ckl_str,ckl_char);
            append(ckr_str,ckr_char);
            sClient = socket(AF_INET,SOCK_STREAM,0);
            if(sClient == -1){
                cout<<"客户端初始化步骤1失败"<<endl;
                return 0;
            }
            bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
            while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
            }
            send(sClient,ckl_char, MSGSIZE, 0);
            send(sClient,ckr_char, MSGSIZE, 0);
            close(sClient);
            
            
            
            /*—————————data encryption————————*/
            element_snprint(szMessage, 2*parame_length,sign);
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
            temp_hash.clear();
            hash_16.clear();
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
            AES_set_encrypt_key(block_key,128,&key_e);

            
            unsigned char block_plaintext[BlockSize];
            for(int i=0;i<BlockSize;i++){
                block_plaintext[i] = DataBuff[i];
            }
            unsigned char block_ciphertext[BlockSize];
            for(int i=0; i<BlockSize/16;i++){
                AES_ecb_encrypt(block_plaintext+i*16,block_ciphertext+i*16, &key_e, AES_ENCRYPT);
            }
            string Hex_ciphertext;
            for(int i=0;i<BlockSize;i++){
                string temp_res = unsignedCharToHexString(block_ciphertext[i]);
                Hex_ciphertext += temp_res;
            }
            All_cipher+=Hex_ciphertext;
            
            /*——————————发送密文——————————————*/
            char HexCipher_char[MSGSIZE], CipherLen_char[MSGSIZE];
            string CipherLen_str = to_string(Hex_ciphertext.size());
            
            append(CipherLen_str,CipherLen_char);
            
            sClient = socket(AF_INET,SOCK_STREAM,0);
            if(sClient == -1){
                cout<<"客户端初始化步骤1失败"<<endl;
                return 0;
            }
            bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
            while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
            }
            send(sClient,CipherLen_char, MSGSIZE, 0);
            close(sClient);
            
            int send_time = ceil(double(atoi(CipherLen_str.c_str()))/double(MSGSIZE));
            int now_ord = 0;
            for(int i=0;i<send_time;i++){
                if(i==send_time-1){
                    for(int j =0;j<(atoi(CipherLen_str.c_str())-i*MSGSIZE);j++){
                        HexCipher_char[j] =Hex_ciphertext[now_ord];
                        now_ord++;
                    }
                }
                else{
                    for(int j =0; j<MSGSIZE; j++){
                        HexCipher_char[j] =Hex_ciphertext[now_ord];
                        now_ord++;
                    }
                }
                sClient = socket(AF_INET,SOCK_STREAM,0);
                if(sClient == -1){
                    cout<<"客户端初始化步骤1失败"<<endl;
                    return 0;
                }
                bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
                while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
                }
                send(sClient,HexCipher_char,MSGSIZE, 0);
                close(sClient);
            }
            DataBuff.clear();
            current_char_num=0;
        }
        
    }
    
    /*——————————签名——————————*/
    element_t alpha, beta,tau1,tau2,tau3,l1,l2,l3,l4,l5,xi1,xi2,xi3,xi4,xi5,aAddb;
    element_init_Zr(alpha,pairing);
    element_init_Zr(beta,pairing);
    element_init_G1(tau1,pairing);
    element_init_G1(tau2,pairing);
    element_init_G1(tau3,pairing);
    element_init_Zr(l1,pairing);
    element_init_Zr(l2,pairing);
    element_init_Zr(l3,pairing);
    element_init_Zr(l4,pairing);
    element_init_Zr(l5,pairing);
    element_init_G1(xi1,pairing);
    element_init_G1(xi2,pairing);
    element_init_GT(xi3,pairing);
    element_init_G1(xi4,pairing);
    element_init_G1(xi5,pairing);
    element_init_Zr(aAddb,pairing);
    
    element_random(alpha);
    element_random(beta);
    element_random(l1);
    element_random(l2);
    element_random(l3);
    element_random(l4);
    element_random(l5);
    element_pow_zn(tau1,u,alpha);
    element_pow_zn(tau2,v,beta);
    element_add(aAddb,alpha,beta);
    element_pow_zn(tau3,h,aAddb);
    element_mul(tau3, tau3, theta);
    element_pow_zn(xi1,u,l1);
    element_pow_zn(xi2,v,l2);
    
    element_t temp_e,zero;
    element_init_GT(temp_e, pairing);
    element_init_Zr(zero,pairing);
    element_set0(zero);
    element_sub(zero,zero,l1);
    element_sub(zero,zero,l2);
    
    pairing_apply(xi3,tau3,g,pairing);
    element_pow_zn(xi3,xi3,l3);
    pairing_apply(temp_e,h,w,pairing);
    element_pow_zn(temp_e,temp_e,zero);
    element_mul(xi3,xi3,temp_e);
    
    element_set0(zero);
    element_sub(zero,zero,l4);
    element_sub(zero,zero,l5);
    pairing_apply(temp_e,h,g,pairing);
    element_pow_zn(temp_e,temp_e,zero);
    element_mul(xi3,xi3,temp_e);
    
    element_set0(zero);
    element_sub(zero,zero,l4);
    element_t temp_e1;
    element_init_G1(temp_e1, pairing);
    element_pow_zn(temp_e1, u, zero);
    element_pow_zn(xi4,tau1,l3);
    element_mul(xi4,xi4,temp_e1);
    
    element_set0(zero);
    element_sub(zero,zero,l5);
    element_pow_zn(temp_e1, v, zero);
    element_pow_zn(xi5,tau2,l3);
    element_mul(xi5,xi5,temp_e1);
    
    char tau1_char[MSGSIZE],tau2_char[MSGSIZE],tau3_char[MSGSIZE],xi1_char[MSGSIZE],xi2_char[MSGSIZE],xi3_char[MSGSIZE],xi4_char[MSGSIZE],xi5_char[MSGSIZE];
    string tau1_str,tau2_str,tau3_str,xi1_str,xi2_str,xi3_str,xi4_str,xi5_str;
    element_snprint(tau1_char, 2*parame_length, tau1);
    element_snprint(tau2_char, 2*parame_length, tau2);
    element_snprint(tau3_char, 2*parame_length, tau3);
    element_snprint(xi1_char, 2*parame_length, xi1);
    element_snprint(xi2_char, 2*parame_length, xi2);
    element_snprint(xi3_char, 2*parame_length, xi3);
    element_snprint(xi4_char, 2*parame_length, xi4);
    element_snprint(xi5_char, 2*parame_length, xi5);
    tau1_str= ExtraG1(tau1_char);
    tau2_str= ExtraG1(tau2_char);
    tau3_str= ExtraG1(tau3_char);
    xi1_str= ExtraG1(xi1_char);
    xi2_str= ExtraG1(xi2_char);
    xi3_str= ExtraG1(xi3_char);
    xi4_str= ExtraG1(xi4_char);
    xi5_str= ExtraG1(xi5_char);

    
    All_cipher+=tau1_str;
    All_cipher+=tau2_str;
    All_cipher+=tau3_str;
    All_cipher+=xi1_str;
    All_cipher+=xi2_str;
    All_cipher+=xi3_str;
    All_cipher+=xi4_str;
    All_cipher+=xi5_str;
    
    //cout<<All_cipher.size()<<endl;
    char* DataBuff_char = new char[All_cipher.size()+1];
    for(int i=0;i<All_cipher.size();i++){
        DataBuff_char[i] = All_cipher[i];
    }
    DataBuff_char[All_cipher.size()]='\n';
    unsigned char SHA256OUT_Block[32];
    char* block_hash_char = new char[32];
    SHA256_CTX ctx1;
    SHA256_Init(&ctx1);
    SHA256_Update(&ctx1,DataBuff_char,strlen(DataBuff_char));
    SHA256_Final(SHA256OUT_Block,&ctx1);
    string temp_hash;
    for(int i=0;i<32;i++){
        block_hash_char[i]=SHA256OUT_Block[i];
        temp_hash+=SHA256OUT_Block[i];
    }
    element_t Hash;
    element_init_Zr(Hash,pairing);
    string hash_16 = binaryToHex(temp_hash);
    string Finalhash= hash_16.substr(0,64);
    const char* block_h_char= Finalhash.data();
    mpz_init_set_str(block_h,block_h_char,16);
    element_set_mpz(Hash,block_h);
    delete[] DataBuff_char;
    delete[] block_hash_char;
    
    element_t hh;
    element_init_G1(hh, pairing);
    char* hhh = All_cipher.data();
    element_from_hash(hh,hhh,All_cipher.size());
    //element_printf("Hash: %B\n", hh);
    
    element_t phi1,phi2,phi3,phi4,phi5;
    element_init_Zr(phi1, pairing);
    element_init_Zr(phi2, pairing);
    element_init_Zr(phi3, pairing);
    element_init_Zr(phi4, pairing);
    element_init_Zr(phi5, pairing);
    element_mul(phi1,Hash,alpha);
    element_add(phi1,phi1,l1);
    
    element_mul(phi2,Hash,beta);
    element_add(phi2,phi2,l2);
    
    element_mul(phi3,Hash,x);
    element_add(phi3,phi3,l3);
    
    element_mul(phi4,Hash,x);
    element_mul(phi4,phi4,alpha);
    element_add(phi4,phi4,l4);
    
    element_mul(phi5,Hash,x);
    element_mul(phi5,phi5,beta);
    element_add(phi5,phi5,l5);
    
    char phi1_char[MSGSIZE], phi2_char[MSGSIZE], phi3_char[MSGSIZE],  phi4_char[MSGSIZE],  phi5_char[MSGSIZE], hash_char[MSGSIZE],ver_char[MSGSIZE];
    string phi1_str,phi2_str,phi3_str,phi4_str,phi5_str,hash_str,ver_str;
    element_snprint(phi1_char, 2*parame_length, phi1);
    element_snprint(phi2_char, 2*parame_length, phi2);
    element_snprint(phi3_char, 2*parame_length, phi3);
    element_snprint(phi4_char, 2*parame_length, phi4);
    element_snprint(phi5_char, 2*parame_length, phi5);
    element_snprint(hash_char, 2*parame_length, Hash);
    element_snprint(ver_char, 2*parame_length, hh);
    phi1_str= ExtraZr(phi1_char);
    phi2_str= ExtraZr(phi2_char);
    phi3_str= ExtraZr(phi3_char);
    phi4_str= ExtraZr(phi4_char);
    phi5_str= ExtraZr(phi5_char);
    hash_str= ExtraZr(hash_char);
    ver_str = ExtraG1(ver_char);
    append(tau1_str,tau1_char);
    append(tau2_str,tau2_char);
    append(tau3_str,tau3_char);
    append(hash_str,hash_char);
    append(phi1_str,phi1_char);
    append(phi2_str,phi2_char);
    append(phi3_str,phi3_char);
    append(phi4_str,phi4_char);
    append(phi5_str,phi5_char);
    append(ver_str,ver_char);
    
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    send(sClient,tau1_char,MSGSIZE, 0);
    send(sClient,tau2_char,MSGSIZE, 0);
    send(sClient,tau3_char,MSGSIZE, 0);
    close(sClient);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    send(sClient,hash_char,MSGSIZE, 0);
    close(sClient);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    send(sClient,phi1_char,MSGSIZE, 0);
    send(sClient,phi2_char,MSGSIZE, 0);
    close(sClient);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    send(sClient,phi3_char,MSGSIZE, 0);
    send(sClient,phi4_char,MSGSIZE, 0);
    close(sClient);
    
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    send(sClient,phi5_char,MSGSIZE, 0);
    send(sClient,ver_char,MSGSIZE, 0);
    close(sClient);
    
    return 0;
}
