
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
/*Paper 7 Cloud Setup-Device*/
/*初始化公共参数并公开*/

int main(int argc, const char* argv[]) {

	/*参数bit长度*/
	const int parame_length = 512;
	/*参数bit长度*/

	string N_str, g_str, p_str, q_str, sc_str, h_str, w_str, u_str, xi2_str, skd1_str, skd2_str;
	char N_char[MSGSIZE], g_char[MSGSIZE], h_char[MSGSIZE], w_char[MSGSIZE], u_char[MSGSIZE], skd1_char[MSGSIZE], skd2_char[MSGSIZE];

	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "xxx", "xxxx", "xxxx", "xxxx", 3306, NULL, 0)) {
		cout << "成功连接MYSQL!!! " << endl;
	}
	string select_sql = "xx";
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
	
	N_str = nextRow[2];
	g_str = nextRow[3];
	p_str = nextRow[0];
	q_str = nextRow[1];
	h_str = nextRow[4];
	w_str = nextRow[5];
	u_str = nextRow[6];
	xi2_str = nextRow[8];
	sc_str = nextRow[9];

	int index = 0;
	for (int i = 0; i < N_str.size(); i++) {
		N_char[index] = N_str[i];
		index++;
	}
	index = 0;
	for (int i = 0; i < g_str.size(); i++) {
		g_char[index] = g_str[i];
		index++;
	}
	index = 0;
	for (int i = 0; i < h_str.size(); i++) {
		h_char[index] = h_str[i];
		index++;
	}
	index = 0;
	for (int i = 0; i < w_str.size(); i++) {
		w_char[index] = w_str[i];
		index++;
	}
	index = 0;
	for (int i = 0; i < u_str.size(); i++) {
		u_char[index] = u_str[i];
		index++;
	}
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

	char szMessage[MSGSIZE];
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
	} //最多一个客户端连接
	cout << "Listen结束" << endl;
	cout << "等待client的连接请求" << endl;
	

	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);

	N_char[N_str.size()] = '!';
	g_char[g_str.size()] = '!';
	h_char[h_str.size()] = '!';
	w_char[w_str.size()] = '!';
	u_char[u_str.size()] = '!';

	send(sClient, N_char, MSGSIZE, NULL);
	send(sClient, g_char, MSGSIZE, NULL);
	send(sClient, h_char, MSGSIZE, NULL);
	send(sClient, w_char, MSGSIZE, NULL);
	send(sClient, u_char, MSGSIZE, NULL);
	closesocket(sClient);


	const char* ptr_N = N_str.data();
	const char* g_ptr = g_str.data();
	const char* sc_ptr = sc_str.data();
	const char* xi2_ptr = xi2_str.data();
	mpz_t N;
	mpz_init(N);
	mpz_init_set_str(N, ptr_N, 10);
	pbc_param_t param;
	pbc_param_init_a1_gen(param, N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);

	element_t g, sc,Rd,up,down;
	element_init_G1(g, pairing);
	element_init_Zr(sc, pairing);
	element_init_G1(Rd, pairing);
	element_init_G1(up, pairing);
	element_init_G1(down, pairing);
	element_set_str(g, g_ptr, 10);
	element_set_str(sc, sc_ptr, 10);
	element_pow_zn(up, g, sc);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, szMessage, MSGSIZE, NULL);
	closesocket(sClient);

	string down_str = InfoExtra(szMessage);
	const char* down_ptr = down_str.data();
	element_set_str(down, down_ptr, 10);
	element_div(Rd, up, down);
	char Rd_char[MSGSIZE];
	element_snprint(Rd_char, 2 * parame_length, Rd);
	string Rd_str = ExtraG1(Rd_char);
	append(Rd_str, Rd_char);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	send(sClient, Rd_char, MSGSIZE, NULL);
	closesocket(sClient);
	
	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, szMessage, MSGSIZE, NULL);
	closesocket(sClient);
	string Name_str = InfoExtra(szMessage);

	mpz_t x_t;
	mpz_init(x_t);
	pbc_mpz_random(x_t, N);

	element_t x, xi2, xAddr, one, theta;
	element_init_Zr(x, pairing);
	element_init_Zr(xi2, pairing);
	element_init_Zr(one, pairing);
	element_init_Zr(xAddr, pairing);
	element_init_G1(theta, pairing);
	element_set_mpz(x, x_t);
	element_set_str(xi2,xi2_ptr,10);
	element_set1(one);
	element_add(xAddr, x, xi2);
	//element_printf("xAddr: %B\n", xAddr);
	element_div(xAddr, one, xAddr);
	//element_printf("xAddr: %B\n", xAddr);
	element_pow_zn(theta, g, xAddr);

	element_snprint(skd2_char, 2 * parame_length, theta);
	string theta_str = ExtraG1(skd2_char);
	append(theta_str, skd2_char);
	string x_str = mpz_get_str(NULL, 10, x_t);
	append(x_str, skd1_char);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	send(sClient, skd1_char, MSGSIZE, NULL);
	send(sClient, skd2_char, MSGSIZE, NULL);
	closesocket(sClient);


	string INSERT_SQL_str = "INSERT INTO cloud_register values('" + Name_str + "','"  + Rd_str + "','" + theta_str + "');";
	const char* INSERT_SQL_char = INSERT_SQL_str.data();
	if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
		printf("SQL语句(插入)执行失败: %s\n", mysql_error(&ceshi));
		system("pause");
		return 0;
	}

	WSACleanup();
	system("pause");
	return 0;

}