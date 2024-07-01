
#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif

//#pragma comment(lib,"ws2_32.lib")
# include<iostream>
//#pragma comment(lib,"libssl.lib")
//#pragma comment(lib, "libcrypto.lib")
#include <fstream>
#include "mysql.h"
#include <alibabacloud/oss/OssClient.h>
#include "openssl/aes.h"
#include "openssl/sha.h"
# include "gmp.h"
# include <stdio.h>
# include <string.h>
# include "pbc.h"

#pragma comment(lib,"ws2_32.lib")
//# include <winsock.h>
//# include <OssClientImpl.h>
#include <vector>
#include <stdlib.h>
#include <assert.h>
//#include <WinSock2.h>
#define PORT_LOC 10249
#define MSGSIZE 2048

using namespace std;
using namespace AlibabaCloud::OSS;


string binaryToHex(const string& binaryStr) {
	string ret;
	static const char *hex = "0123456789ABCDEF";
	for (auto c : binaryStr) {
		ret.push_back(hex[(c >> 4) & 0xf]);
		ret.push_back(hex[c & 0xf]);
	}
	return ret;
}
string readFileIntoString(char * filename)
{
	ifstream ifile(filename);
	//将文件读入到ostringstream对象buf中
	ostringstream buf;
	char ch;
	while (ifile.get(ch))
		buf.put(ch);
	//返回与流对象buf关联的字符串
	return buf.str();
}
string HexToStr(std::string str)
{
	std::string hex = str;
	long len = hex.length();
	std::string newString;
	for (long i = 0; i< len; i += 2)
	{
		std::string byte = hex.substr(i, 2);
		char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
		newString.push_back(chr);
	}
	return newString;
}

string unsignedCharToHexString(unsigned char ch) {
	const char hex_chars[] = "0123456789abcdef";
	unsigned int highHalfByte = (ch >> 4) & 0x0f;
	unsigned int lowHalfByte = (ch & 0x0f);
	string result;
	result += hex_chars[highHalfByte];
	result += hex_chars[lowHalfByte];
	return result;
}
string InfoExtra(char MSG[1024 * 2]) {
	string res = "";
	for (int i = 0; i < MSGSIZE; i++) {
		if (MSG[i] == '!') {
			break;
		}
		else {
			res += MSG[i];
		}
	}
	return res;
}
string ExtraZr(char phia_char[MSGSIZE]) {
	string phia_str;
	for (int i = 0; i < MSGSIZE; i++) {
		if ((phia_char[i] <= '9'&&phia_char[i] >= '0')) {
			phia_str += phia_char[i];
		}
		else {
			break;
		}
	}
	return phia_str;
}
string ExtraG1(char fya_char[MSGSIZE]) {
	string fya_str;
	for (int i = 0; i < MSGSIZE; i++) {
		if ((fya_char[i] <= '9'&&fya_char[i] >= '0') || fya_char[i] == ']' || fya_char[i] == '[' || fya_char[i] == ',' || fya_char[i] == ' ') {
			fya_str += fya_char[i];
		}
		else {
			break;
		}
	}
	return fya_str;
}
void append(string s, char* res) {
	for (int i = 0; i < s.size(); i++) {
		res[i] = s[i];
	}
	res[s.size()] = '!';
}
/*Paper 7 Upload Fog-Device*/


int main(int argc, const char* argv[]) {

	string cipher_path = "xxxxx";
	/*参数bit长度*/
	const int parame_length = 512;
	/*参数bit长度*/
	const int BlockSize = 1024 * 64;
	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "xxx", "xxx", "xxx", "xxx", 3306, NULL, 0)) {
		cout << "成功连接MYSQL!!! " << endl;
	}
	string select_sql = "xxxx";
	const char* select_sql_char = select_sql.data();
	if (0 != mysql_query(&ceshi, select_sql_char))
	{
		printf("查询失败: %s\n", mysql_error(&ceshi));
		mysql_close(&ceshi);                         //关闭连接
		system("pause");
		return 0;
	}

	MYSQL_RES *res;                                         //SQL语句执行结果集
	MYSQL_FIELD *field;                                     //包含字段信息的结构指针
	MYSQL_ROW nextRow;                                      //存放查询sql语句字符串数组
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	
	string N_str = nextRow[11];
	string sk_str = nextRow[0];
	string u_str = nextRow[4];
	string v_str = nextRow[5];
	string h_str = nextRow[6];
	string w_str = nextRow[7];
	string g_str = nextRow[10];

	const char* N_ptr = N_str.data();
	const char* sk_ptr = sk_str.data();
	const char* g_ptr = g_str.data();
	mpz_t N;
	mpz_init(N);
	mpz_init_set_str(N, N_ptr, 10);
	pbc_param_t param;
	pbc_param_init_a1_gen(param, N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);

	element_t sk,g;
	element_init_Zr(sk, pairing);
	element_init_G1(g, pairing);
	element_set_str(sk, sk_ptr, 10);
	element_set_str(g, g_ptr, 10);
	
	std::string AccessKeyId = "xxxx";
	std::string AccessKeySecret = "xxxx";
	std::string Endpoint = "xxx";
	std::string BucketName = "xxxx";
	InitializeSdk();
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);


	/*———————————————— BEGIN ————————————————————*/
	/*—————————————TCP通信进参数传递—————————————————*/

	/*—————————  BEGIN  ——————————*/
	/*—————————TCP初始化——————————*/
	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;//服务器地址
	SOCKADDR client_user; //客户端地址
	memset(&local, 0, sizeof(local));

	char szMessage[MSGSIZE],File_char[MSGSIZE],ord_char[MSGSIZE];
	int ret;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "sListent是错的" << endl;
	}
	
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC);
	local.sin_addr.s_addr = htonl(INADDR_ANY);// 任何一个客户端IP都可以连接

	while(::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR))==SOCKET_ERROR) {
	}
	cout << "Bind成功" << endl;
	while (listen(sListen, 20) == -1) {
	} 
	cout << "Listen结束" << endl;
	cout << "等待client的连接请求" << endl;
	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, ord_char, MSGSIZE, NULL);
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, File_char, MSGSIZE, NULL); 
	recv(sClient, szMessage, MSGSIZE, NULL); //接收文件块数
	closesocket(sClient);
	string File_str = InfoExtra(File_char);
	string Num_str = InfoExtra(szMessage);
	string ord = InfoExtra(ord_char);

	int BlockNum = atoi(Num_str.c_str());
	char ckl_char[MSGSIZE], ckr_char[MSGSIZE],Len_char[MSGSIZE];
	fstream write_ptr;
	string All_cipher;
	for (int i = 0; i < BlockNum; i++) {//分块处理
		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		recv(sClient, szMessage, MSGSIZE, NULL);
		closesocket(sClient);
		string msg_str = InfoExtra(szMessage);
		const char* msg_ptr = msg_str.data();
		element_t msg,sign;
		element_init_G1(msg, pairing);
		element_init_G1(sign, pairing);
		element_set_str(msg, msg_ptr, 10);
		//element_printf("msg: %B\n", msg);
		element_pow_zn(sign, msg, sk);
		element_snprint(szMessage, 2 * parame_length, sign);
		string sign_str = ExtraG1(szMessage);
		append(sign_str, szMessage);
		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, szMessage, MSGSIZE, NULL);
		closesocket(sClient);

		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		recv(sClient, ckl_char, MSGSIZE, NULL);
		recv(sClient, ckr_char, MSGSIZE, NULL);
		closesocket(sClient);
		string ckl_str = InfoExtra(ckl_char);
		string ckr_str = InfoExtra(ckr_char);
		string write_path1 = cipher_path + File_str + "_" + to_string(i + 1) + "KS.txt";
		write_ptr.open(write_path1, ios::out);
		write_ptr << ckl_str << endl;
		write_ptr << ckr_str << endl;
		write_ptr.close();
		
		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		recv(sClient, Len_char, MSGSIZE, NULL);
		closesocket(sClient);
		string Len_str = InfoExtra(Len_char);
		int Len = atoi(Len_str.c_str());
		
		int times = ceil(double(Len) / double(MSGSIZE));
		
		string Ciphertext;
		for (int i = 0; i < times; i++) {
			
			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			recv(sClient, szMessage, MSGSIZE, NULL);
			closesocket(sClient);
			string ciphertext_str;
			if (i == times - 1) {
				for (int j = 0; j < (Len - i*MSGSIZE); j++) {
					ciphertext_str += szMessage[j];
				}
			}
			else {
				for (int j = 0; j < MSGSIZE; j++) {
					ciphertext_str += szMessage[j];
				}
			}
			Ciphertext += ciphertext_str;
		}
		All_cipher += Ciphertext;
		string write_path = cipher_path + File_str+"_"+to_string(i + 1) + ".txt";
		write_ptr.open(write_path, ios::out);
		write_ptr << Ciphertext << endl;
		write_ptr.close();

		string ObjectName1 = "xxxxx";
		std::shared_ptr<std::iostream> content1 = std::make_shared<std::fstream>(write_path, std::ios::in | std::ios::binary);
		PutObjectRequest request1(BucketName, ObjectName1, content1);
		auto outcome1 = client.PutObject(request1);
		if (!outcome1.isSuccess()) {
			std::cout << "Semantically secure ciphertext upload fail" <<
				",code:" << outcome1.error().Code() <<
				",message:" << outcome1.error().Message() <<
				",requestId:" << outcome1.error().RequestId() << std::endl;

			system("pause");
		}
		ObjectName1.clear();
		ObjectName1 = "xxxxx";
		std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(write_path1, std::ios::in | std::ios::binary);
		PutObjectRequest request(BucketName, ObjectName1, content);
		auto outcome = client.PutObject(request);
		if (!outcome.isSuccess()) {
			std::cout << "Semantically secure ciphertext upload fail" <<
				",code:" << outcome.error().Code() <<
				",message:" << outcome.error().Message() <<
				",requestId:" << outcome.error().RequestId() << std::endl;

			system("pause");
		}




	}
	//接收签名
	char ver_char[MSGSIZE],tau1_char[MSGSIZE], tau2_char[MSGSIZE], tau3_char[MSGSIZE], hash_char[MSGSIZE], phi1_char[MSGSIZE], phi2_char[MSGSIZE], phi3_char[MSGSIZE], phi4_char[MSGSIZE], phi5_char[MSGSIZE];
	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, tau1_char, MSGSIZE, NULL);
	recv(sClient, tau2_char, MSGSIZE, NULL);
	recv(sClient, tau3_char, MSGSIZE, NULL);
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, hash_char, MSGSIZE, NULL);
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, phi1_char, MSGSIZE, NULL);
	recv(sClient, phi2_char, MSGSIZE, NULL);
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, phi3_char, MSGSIZE, NULL);
	recv(sClient, phi4_char, MSGSIZE, NULL);
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, phi5_char, MSGSIZE, NULL);
	recv(sClient, ver_char, MSGSIZE, NULL);
	closesocket(sClient);

	string ver_str,tau1_str, tau2_str, tau3_str, hash_str, phi1_str, phi2_str, phi3_str, phi4_str, phi5_str;
	tau1_str = InfoExtra(tau1_char);
	tau2_str = InfoExtra(tau2_char);
	tau3_str = InfoExtra(tau3_char);
	hash_str = InfoExtra(hash_char);
	phi1_str = InfoExtra(phi1_char);
	phi2_str = InfoExtra(phi2_char);
	phi3_str = InfoExtra(phi3_char);
	phi4_str = InfoExtra(phi4_char);
	phi5_str = InfoExtra(phi5_char);
	ver_str = InfoExtra(ver_char);
	
	string write_path = cipher_path + File_str + "_Signature.txt";
	write_ptr.open(write_path, ios::out);
	write_ptr << tau1_str << endl;
	write_ptr << tau2_str << endl;
	write_ptr << tau3_str << endl;
	write_ptr << hash_str << endl;
	write_ptr << phi1_str << endl;
	write_ptr << phi2_str << endl;
	write_ptr << phi3_str << endl;
	write_ptr << phi4_str << endl;
	write_ptr << phi5_str << endl;
	write_ptr << ver_str << endl;
	write_ptr.close();







	element_t tau1, tau2, tau3, hash, phi1, phi2, phi3, phi4, phi5;
	element_init_G1(tau1, pairing);
	element_init_G1(tau2, pairing);
	element_init_G1(tau3, pairing);
	element_init_Zr(hash, pairing);
	element_init_Zr(phi1, pairing);
	element_init_Zr(phi2, pairing);
	element_init_Zr(phi3, pairing);
	element_init_Zr(phi4, pairing);
	element_init_Zr(phi5, pairing);
	const char* tau1_ptr = tau1_str.data();
	const char* tau2_ptr = tau2_str.data();
	const char* tau3_ptr = tau3_str.data();
	const char* hash_ptr = hash_str.data();
	const char* phi1_ptr = phi1_str.data();
	const char* phi2_ptr = phi2_str.data();
	const char* phi3_ptr = phi3_str.data();
	const char* phi4_ptr = phi4_str.data();
	const char* phi5_ptr = phi5_str.data();
	element_set_str(tau1, tau1_ptr, 10);
	element_set_str(tau2, tau2_ptr, 10);
	element_set_str(tau3, tau3_ptr, 10);
	element_set_str(hash, hash_ptr, 10);
	element_set_str(phi1, phi1_ptr, 10);
	element_set_str(phi2, phi2_ptr, 10);
	element_set_str(phi3, phi3_ptr, 10);
	element_set_str(phi4, phi4_ptr, 10);
	element_set_str(phi5, phi5_ptr, 10);

	

	const char* u_char = u_str.data();
	const char* v_char = v_str.data();
	const char* h_char = h_str.data();
	const char* w_char = w_str.data();
	element_t u, v, h, w;
	element_init_G1(u, pairing);
	element_init_G1(v, pairing);
	element_init_G1(h, pairing);
	element_init_G1(w, pairing);
	element_set_str(u, u_char, 10);
	element_set_str(v, v_char, 10);
	element_set_str(h, h_char, 10);
	element_set_str(w, w_char, 10);

	element_t xi1, xi2, xi3, xi4, xi5, temp1, zero;
	element_init_G1(xi1, pairing);
	element_init_G1(xi2, pairing);
	element_init_GT(xi3, pairing);
	element_init_G1(xi4, pairing);
	element_init_G1(xi5, pairing);
	element_init_G1(temp1, pairing);
	element_init_Zr(zero, pairing);

	element_pow_zn(xi1, u, phi1);
	element_set0(zero);
	element_sub(zero, zero, hash);
	element_pow_zn(temp1, tau1, zero);
	element_mul(xi1, xi1, temp1);

	element_pow_zn(xi2, v, phi2);
	element_pow_zn(temp1, tau2, zero);
	element_mul(xi2, xi2, temp1);

	element_t temp2, temp3, temp4;
	element_init_G1(temp2, pairing);
	element_init_G1(temp3, pairing);
	element_init_GT(temp4, pairing);
	pairing_apply(xi3, tau3, g, pairing);
	element_pow_zn(xi3, xi3, phi3);
	element_set0(zero);
	element_sub(zero, zero, phi1);
	element_sub(zero, zero, phi2);
	element_pow_zn(temp2, w, zero);
	element_set0(zero);
	element_sub(zero, zero, phi4);
	element_sub(zero, zero, phi5);
	element_pow_zn(temp3, g, zero);
	element_mul(temp2, temp2, temp3);
	pairing_apply(temp4, h, temp2, pairing);
	element_mul(xi3, xi3, temp4);
	pairing_apply(temp4, tau3, w, pairing);
	element_pow_zn(temp4, temp4, hash);
	element_mul(xi3, xi3, temp4);
	pairing_apply(temp4, g, g, pairing);
	element_pow_zn(temp4, temp4, hash);
	element_div(xi3, xi3, temp4);

	element_pow_zn(xi4, tau1, phi3);
	element_set0(zero);
	element_sub(zero, zero, phi4);
	element_pow_zn(temp2, u, zero);
	element_mul(xi4, xi4, temp2);


	element_pow_zn(xi5, tau2, phi3);
	element_set0(zero);
	element_sub(zero, zero, phi5);
	element_pow_zn(temp2, v, zero);
	element_mul(xi5, xi5, temp2);

	char tau1_char1[MSGSIZE], tau2_char1[MSGSIZE], tau3_char1[MSGSIZE], xi1_char[MSGSIZE], xi2_char[MSGSIZE], xi3_char[MSGSIZE], xi4_char[MSGSIZE], xi5_char[MSGSIZE];
	string tau1_str1, tau2_str1, tau3_str1, xi1_str, xi2_str, xi3_str, xi4_str, xi5_str;
	element_snprint(tau1_char1, 2 * parame_length, tau1);
	element_snprint(tau2_char1, 2 * parame_length, tau2);
	element_snprint(tau3_char1, 2 * parame_length, tau3);
	element_snprint(xi1_char, 2 * parame_length, xi1);
	element_snprint(xi2_char, 2 * parame_length, xi2);
	element_snprint(xi3_char, 2 * parame_length, xi3);
	element_snprint(xi4_char, 2 * parame_length, xi4);
	element_snprint(xi5_char, 2 * parame_length, xi5);
	tau1_str1 = ExtraG1(tau1_char1);
	tau2_str1 = ExtraG1(tau2_char1);
	tau3_str1 = ExtraG1(tau3_char1);
	xi1_str = ExtraG1(xi1_char);
	xi2_str = ExtraG1(xi2_char);
	xi3_str = ExtraG1(xi3_char);
	xi4_str = ExtraG1(xi4_char);
	xi5_str = ExtraG1(xi5_char);

	All_cipher += tau1_str1;
	All_cipher += tau2_str1;
	All_cipher += tau3_str1;
	All_cipher += xi1_str;
	All_cipher += xi2_str;
	All_cipher += xi3_str;
	All_cipher += xi4_str;
	All_cipher += xi5_str;


	element_t hh;
	element_init_G1(hh, pairing);
	char* hhh = new char[All_cipher.size()];
	for (int i = 0; i < All_cipher.size(); i++) {
		hhh[i] = All_cipher[i];
	}
	element_from_hash(hh, hhh, All_cipher.size());
	//element_printf("Hash: %B\n", hh);
	element_snprint(ver_char, 2 * parame_length, hh);
	string ver_str1 = ExtraG1(ver_char);
	///cout << ver_str1 << endl;
	///cout << ver_str << endl;
	if (ver_str.compare(ver_str1) == 0) {
		cout << "Signature verification valid!" << endl;
		//存储签名
		string ObjectName1 = "xxxxx";
		std::shared_ptr<std::iostream> content1 = std::make_shared<std::fstream>(write_path, std::ios::in | std::ios::binary);
		PutObjectRequest request1(BucketName, ObjectName1, content1);
		auto outcome1 = client.PutObject(request1);
		if (!outcome1.isSuccess()) {
			std::cout << "Semantically secure ciphertext upload fail" <<
				",code:" << outcome1.error().Code() <<
				",message:" << outcome1.error().Message() <<
				",requestId:" << outcome1.error().RequestId() << std::endl;

			system("pause");
		}
	}
	else { //签名验证失败，删除密文和key shares
		for (int i = 0; i < BlockNum; i++) {
			
			string ObjectName1 = "xxxxx";
			//std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(write_path1, std::ios::in | std::ios::binary);
			DeleteObjectRequest request(BucketName, ObjectName1);
			auto outcome = client.DeleteObject(request);
			if (!outcome.isSuccess()) {
				std::cout << "Semantically secure ciphertext upload fail" <<
					",code:" << outcome.error().Code() <<
					",message:" << outcome.error().Message() <<
					",requestId:" << outcome.error().RequestId() << std::endl;

				system("pause");
			}
			ObjectName1.clear();
			ObjectName1 = "xxxxx";
			DeleteObjectRequest request1(BucketName, ObjectName1);
			auto outcome1 = client.DeleteObject(request1);
			if (!outcome1.isSuccess()) {
				std::cout << "Semantically secure ciphertext upload fail" <<
					",code:" << outcome1.error().Code() <<
					",message:" << outcome1.error().Message() <<
					",requestId:" << outcome1.error().RequestId() << std::endl;

				system("pause");
			}

		}
	}


	



	WSACleanup();
	system("pause");
	return 0;

}