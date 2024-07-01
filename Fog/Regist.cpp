
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
	//���ļ����뵽ostringstream����buf��
	ostringstream buf;
	char ch;
	while (ifile.get(ch))
		buf.put(ch);
	//������������buf�������ַ���
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
/*Paper 7 Fpg Setup-Device*/
/*��ʼ����������������*/

int main(int argc, const char* argv[]) {

	/*����bit����*/
	const int parame_length = 512;
	/*����bit����*/

	string N_str, g_str, xi_str,u_str,v_str,h_str,w_str,pkf_str;

	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "xxxx", "xxxx", "xxxx", "xxxx", 3306, NULL, 0)) {
		cout << "�ɹ�����MYSQL!!! " << endl;
	}
	string select_sql = "xxxxx";
	const char* select_sql_char = select_sql.data();
	if (0 != mysql_query(&ceshi, select_sql_char))
	{
		printf("��ѯʧ��: %s\n", mysql_error(&ceshi));
		mysql_close(&ceshi);                         //�ر�����
		system("pause");
		return 0;
	}

	MYSQL_RES *res;                                         //SQL���ִ�н����
	MYSQL_FIELD *field;                                     //�����ֶ���Ϣ�Ľṹָ��
	MYSQL_ROW nextRow;                                      //��Ų�ѯsql����ַ�������
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	
	N_str = nextRow[8];
	g_str = nextRow[7];
	xi_str = nextRow[1];
	u_str = nextRow[2];
	v_str = nextRow[3];
	h_str = nextRow[4];
	w_str = nextRow[5];
	pkf_str = nextRow[6];
	char v_char[MSGSIZE], pkf_char[MSGSIZE];
	append(v_str, v_char);
	append(pkf_str, pkf_char);
	/*�������������������������������� BEGIN ����������������������������������������*/
	/*��������������������������TCPͨ�Ž��������ݡ���������������������������������*/

	/*������������������  BEGIN  ��������������������*/
	/*������������������TCP��ʼ����������������������*/
	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;//��������ַ
	SOCKADDR client_user; //�ͻ��˵�ַ
	memset(&local, 0, sizeof(local));

	char szMessage[MSGSIZE];
	int ret;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "sListent�Ǵ��" << endl;
	}
	
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC);
	local.sin_addr.s_addr = htonl(INADDR_ANY);// �κ�һ���ͻ���IP����������

	while(::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR))==SOCKET_ERROR) {
	}
	cout << "Bind�ɹ�" << endl;

	while (listen(sListen, 20) == -1) {
	} //���һ���ͻ�������
	cout << "Listen����" << endl;
	cout << "�ȴ�client����������" << endl;
	

	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	send(sClient, v_char, MSGSIZE, NULL);
	send(sClient, pkf_char, MSGSIZE, NULL);
	closesocket(sClient);




	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, szMessage, MSGSIZE, NULL);
	closesocket(sClient);


	string Rd_str = InfoExtra(szMessage);

	string New_select_SQL_str = "xxxxx";
	const char* New_select_SQL_char = New_select_SQL_str.data();
	if (0 != mysql_query(&ceshi, New_select_SQL_char)) {
		printf("SQL���(��ѯ)ִ��ʧ��: %s\n", mysql_error(&ceshi));
	}
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
	if (nextRow == NULL) {
		cout << "�豸δע��" << endl;
		system("pause");
		return 0;
	}

	WSACleanup();
	system("pause");
	return 0;

}