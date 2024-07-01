
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
#define PORT_LOC 10248
#define MSGSIZE 2048


#define CLOUD_SERVER_ADDRESS "x.x.x.x"
#define PORT_CLOUD_SERVER 10248
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





int main(int argc, const char* argv[]) {

	const int parame_length = 512;
	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;
	if (mysql_real_connect(&ceshi, "xxx", "xxx", "xxx", "xxx", 3306, NULL, 0)) {
		cout << "成功连接MYSQL!!! " << endl;
	}
	string select_sql = "xxxxxx";
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
	if (nextRow == NULL) //尚未完成初始化
	{
		WSADATA wsaData;
		SOCKET sClient;
		SOCKADDR_IN server;
		int ret;
		WSAStartup(0x0202, &wsaData);
		sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		memset(&server, 0, sizeof(SOCKADDR_IN));
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT_CLOUD_SERVER);
		server.sin_addr.s_addr = inet_addr(CLOUD_SERVER_ADDRESS);

		while (connect(sClient, (sockaddr*)&server, sizeof(SOCKADDR)) == -1) {
		}
		char N_char[MSGSIZE], g_char[MSGSIZE], h_char[MSGSIZE], u_char[MSGSIZE], w_char[MSGSIZE];
		ret = recv(sClient, N_char, MSGSIZE, 0);
		ret = recv(sClient, g_char, MSGSIZE, 0);
		ret = recv(sClient, h_char, MSGSIZE, 0);
		ret = recv(sClient, u_char, MSGSIZE, 0);
		ret = recv(sClient, w_char, MSGSIZE, 0);
		closesocket(sClient);
		string N_str = InfoExtra(N_char);
		string g_str = InfoExtra(g_char);
		string h_str = InfoExtra(h_char);
		string u_str = InfoExtra(u_char);
		string w_str = InfoExtra(w_char);
		//参数初始化
		const char* ptr_N = N_str.data();
		const char* g_ptr = g_str.data();
		const char* h_ptr = h_str.data();
		mpz_t N;
		mpz_init(N);
		mpz_init_set_str(N, ptr_N, 10);
		pbc_param_t param;
		pbc_param_init_a1_gen(param, N);
		pairing_t pairing;
		pairing_init_pbc_param(pairing, param);
		
		mpz_t sk_t,xi1_t,in_xi1_t;
		mpz_init(sk_t);
		mpz_init(xi1_t);
		mpz_init(in_xi1_t);
		pbc_mpz_random(sk_t, N);
		pbc_mpz_random(xi1_t, N);
		mpz_invert(in_xi1_t, xi1_t,N);



		string sk_str, xi1_str;
		sk_str = mpz_get_str(NULL, 10, sk_t);
		xi1_str = mpz_get_str(NULL, 10, xi1_t);

		element_t g, sk, in_xi1, v, h,pkf;
		element_init_G1(g, pairing);
		element_init_Zr(sk, pairing);
		element_init_Zr(in_xi1, pairing);
		element_init_G1(v, pairing);
		element_init_G1(h, pairing);
		element_init_G1(pkf, pairing);

		element_set_str(g, g_ptr, 10);
		element_set_mpz(sk,sk_t);
		element_set_mpz(in_xi1, in_xi1_t);
		
		element_pow_zn(pkf, g, sk);
		element_pow_zn(v, h, in_xi1);

		char pkf_char[MSGSIZE], h_char[MSGSIZE], w_char[MSGSIZE],u_char[MSGSIZE],v_char[MSGSIZE];
		element_snprint(pkf_char, 2 * parame_length, pkf);
		element_snprint(h_char, 2 * parame_length, h);
		element_snprint(v_char, 2 * parame_length, v);
		string pkf_str = ExtraG1(pkf_char);
		string v_str = ExtraG1(v_char);

		string INSERT_SQL_str = "xxxxxxxxxxxxxxxxx";
		const char* INSERT_SQL_char = INSERT_SQL_str.data();
		if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
			printf("SQL语句(插入)执行失败: %s\n", mysql_error(&ceshi));
			system("pause");
			return 0;
		}

	}
	system("pause");
	return 0;

}