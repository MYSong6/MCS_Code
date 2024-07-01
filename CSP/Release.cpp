
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

int main(int argc, const char* argv[]) {

	/*参数bit长度*/
	const int parame_length = 512;
	/*参数bit长度*/

	string N_str, g_str, p_str, q_str, sc_str, h_str, w_str, u_str, xi2_str, skd1_str, skd2_str;
	char N_char[MSGSIZE], g_char[MSGSIZE], h_char[MSGSIZE], w_char[MSGSIZE], u_char[MSGSIZE], skd1_char[MSGSIZE], skd2_char[MSGSIZE];

	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "xxxx", "xxxx", "xxxx", "xxxx", 3306, NULL, 0)) {
		cout << "成功连接MYSQL!!! " << endl;
	}
	string select_sql = "xxxxx";
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

	send(sClient, N_char, MSGSIZE, NULL);
	send(sClient, g_char, MSGSIZE, NULL);
	closesocket(sClient);
	char name_char[MSGSIZE], cpk_char[MSGSIZE];

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, cpk_char, MSGSIZE, NULL);
	recv(sClient, name_char, MSGSIZE, NULL);
	closesocket(sClient);

	string name_str = InfoExtra(name_char);
	string cpk_str = InfoExtra(cpk_char);
	string INSERT_SQL_str = "xxxxx";
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