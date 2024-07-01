
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
#define PORT_LOC 10311
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
/*Paper 7 Upload Fog-Device*/


int main(int argc, const char* argv[]) {

	string cipher_path = "xxxx";
	/*����bit����*/
	const int parame_length = 512;
	/*����bit����*/
	const int BlockSize = 1024 * 64;
	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "xxx", "x", "xxx", "xxx", 3306, NULL, 0)) {
		cout << "�ɹ�����MYSQL!!! " << endl;
	}
	string select_sql = "xxx";
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
	
	string N_str = nextRow[x];
	string xi1_str = nextRow[x];
	string u_str = nextRow[x];
	string v_str = nextRow[x];
	string h_str = nextRow[x];
	string w_str = nextRow[x];
	string g_str = nextRow[x];

	const char* N_ptr = N_str.data();
	
	const char* g_ptr = g_str.data();
	const char* xi1_ptr = xi1_str.data();
	
	mpz_t N;
	mpz_init(N);
	mpz_init_set_str(N, N_ptr, 10);
	pbc_param_t param;
	pbc_param_init_a1_gen(param, N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);

	element_t g,xi1;
	
	element_init_G1(g, pairing);
	element_init_Zr(xi1, pairing);
	element_set_str(g, g_ptr, 10);
	element_set_str(xi1, xi1_ptr, 10);
	
	


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

	char tau1_char[MSGSIZE],tau2_char[MSGSIZE],tau3_char[MSGSIZE];
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
	} 
	cout << "Listen����" << endl;
	cout << "�ȴ�client����������" << endl;

	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, tau1_char, MSGSIZE, NULL);
	closesocket(sClient);

	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, tau2_char, MSGSIZE, NULL); 
	recv(sClient, tau3_char, MSGSIZE, NULL); //�����ļ�����
	closesocket(sClient);
	string tau1_str = InfoExtra(tau1_char);
	string tau2_str = InfoExtra(tau2_char);
	string tau3_str = InfoExtra(tau3_char);
	cout << tau1_str << endl;
	cout << tau2_str << endl;
	cout << tau3_str << endl;

	element_t tau1, tau2, tau3;
	element_init_G1(tau1, pairing);
	element_init_G1(tau2, pairing);
	element_init_G1(tau3, pairing);
	
	const char* tau1_ptr = tau1_str.data();
	const char* tau2_ptr = tau2_str.data();
	const char* tau3_ptr = tau3_str.data();
	
	element_set_str(tau1, tau1_ptr, 10);
	element_set_str(tau2, tau2_ptr, 10);
	element_set_str(tau3, tau3_ptr, 10);
	

	
	element_pow_zn(tau1, tau1, xi1);
	element_mul(tau1, tau2,tau1);
	element_div(tau3, tau3, tau1);
	//element_printf("msg: %B\n", tau3);
	char theta_char[MSGSIZE];
	element_snprint(theta_char, 2 * parame_length, tau3);
	string theta_str = ExtraG1(theta_char);

	select_sql.clear();
	select_sql = "SELECT * FROM fog1_rd_theta WHERE theta = '" + theta_str + "';";
	const char* select_sql_char2 = select_sql.data();
	if (0 != mysql_query(&ceshi, select_sql_char2))
	{
		printf("��ѯʧ��: %s\n", mysql_error(&ceshi));
		mysql_close(&ceshi);                         //�ر�����
		system("pause");
		return 0;
	}                                      //��Ų�ѯsql����ַ�������
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);

	string Rd_str = nextRow[0];
	char Rd_char[MSGSIZE];
	append(Rd_str, Rd_char);
	listen(sListen, 1);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	send(sClient, Rd_char, MSGSIZE, NULL);
	closesocket(sClient);

	WSACleanup();
	system("pause");
	return 0;

}