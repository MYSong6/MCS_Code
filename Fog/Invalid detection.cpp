
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
#define CLOUD_SERVER_ADDRESS "x.x.x.x"
#define PORT_CLOUD_SERVER 10311
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
	const double T = 0.9;
	const int ND = 3;
	string customer = "x1";
	string doc_name = "xxx";
	string ver_ord = "x";
	string fog = "xxxx";
	int blocknum = 9;
	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	if (mysql_real_connect(&ceshi, "x", "x", "xx", "xxx", 3306, NULL, 0)) {
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
	
	
	string N_str = nextRow[x];
	string g_str = nextRow[x];
	string xi_str = newRow[x];

	select_sql.clear();
	select_sql = "xxxx";
	const char* select_sql_char1 = select_sql.data();
	if (0 != mysql_query(&ceshi, select_sql_char1))
	{
		printf("查询失败: %s\n", mysql_error(&ceshi));
		mysql_close(&ceshi);                         //关闭连接
		system("pause");
		return 0;
	}                                      //存放查询sql语句字符串数组
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);

	string pk_str = nextRow[1];


	const char* N_ptr = N_str.data();
	const char* g_ptr = g_str.data();
	const char* pk_ptr = pk_str.data();
	const char* xi_ptr = xi_str.data();
	mpz_t N;
	mpz_init(N);
	mpz_init_set_str(N, N_ptr, 10);
	pbc_param_t param;
	pbc_param_init_a1_gen(param, N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);

	element_t g,pk,xi;
	element_init_G1(g, pairing);
	element_init_G1(pk, pairing);
	element_init_Zr(pk, pairing);
	element_set_str(g, g_ptr, 10);
	element_set_str(pk, pk_ptr, 10);
	element_set_str(xi, xi_ptr, 10);

	
	std::string AccessKeyId = "xxxx";
	std::string AccessKeySecret = "xxxxx";
	std::string Endpoint = "xxxx";
	std::string BucketName = "xxxx";
	InitializeSdk();
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);

	int real_num = (ND-1)*blocknum;
	int sim = 0;
	char temp_char1[MSGSIZE], temp_char2[MSGSIZE];
	for (int i = 0; i < blocknum; i++) {
		string ObjectName1 = "xxxxx";
		string local_path1 = "xxxx";
		DownloadObjectRequest request2(BucketName, ObjectName1, local_path1);
		auto outcome2 = client.ResumableDownloadObject(request2);
		if (outcome2.isSuccess()) {
			//std::cout << "文件下载成功" << outcome2.result().Metadata().ContentLength() << std::endl;
		}
		else {
			/* 异常处理。*/
			std::cout << "Get cloud public parameter fail" <<
				",code:" << outcome2.error().Code() <<
				",message:" << outcome2.error().Message() <<
				",requestId:" << outcome2.error().RequestId() << std::endl;
			ShutdownSdk();
			return -1;
		}
		fstream read_ptr;
		read_ptr.open(local_path1);
		string cipher1;
		getline(read_ptr, cipher1);
		read_ptr.close();
		element_t h1;
		element_init_G1(h1, pairing);
		char* hh1 = new char[cipher1.size()];
		for (int j = 0; j < cipher1.size(); j++) {
			hh1[j] = cipher1[j];
		}
		element_from_hash(h1, hh1, cipher1.size());
		//element_printf("Hash: %B\n", hh);
		element_snprint(temp_char1, 2 * parame_length, h1);
		string h_str1 = ExtraG1(temp_char1);
		delete[] hh1;
		ObjectName1.clear();
		local_path1.clear();
		ObjectName1 = "xxxxx";
		local_path1 = "xxxx";
		DownloadObjectRequest request5(BucketName, ObjectName1, local_path1);
		auto outcome5 = client.ResumableDownloadObject(request5);
		if (outcome5.isSuccess()) {
			//std::cout << "文件下载成功" << outcome2.result().Metadata().ContentLength() << std::endl;
		}
		else {
			/* 异常处理。*/
			std::cout << "Get cloud public parameter fail" <<
				",code:" << outcome5.error().Code() <<
				",message:" << outcome5.error().Message() <<
				",requestId:" << outcome5.error().RequestId() << std::endl;
			ShutdownSdk();
			return -1;
		}
		read_ptr.open(local_path1);
		string left_str,right_str;
		getline(read_ptr, left_str);
		getline(read_ptr, right_str);
		read_ptr.close();
		element_t left, right;
		element_init_G1(left, pairing);
		element_init_G1(right, pairing);
		const char* left_ptr = left_str.data();
		const char* right_ptr = right_str.data();
		element_set_str(left, left_ptr, 10);
		element_set_str(right, right_ptr, 10);
		
		for (int j = 1; j <=ND; j++) {
			if (to_string(j) == ver_ord) {
				continue;
			}
			ObjectName1.clear();
			local_path1.clear();
			ObjectName1 = "sssss";
			local_path1 = "sssss";
			DownloadObjectRequest request3(BucketName, ObjectName1, local_path1);
			auto outcome3 = client.ResumableDownloadObject(request3);
			if (outcome3.isSuccess()) {
				//std::cout << "文件下载成功" << outcome3.result().Metadata().ContentLength() << std::endl;
			}
			else {
				real_num--;
				continue;
			}
			read_ptr.open(local_path1);
			string cipher2;
			getline(read_ptr, cipher2);
			read_ptr.close();

			
			element_t h2;
			element_init_G1(h2, pairing);
			char* hh2 = new char[cipher2.size()];
			for (int k = 0; k < cipher2.size(); k++) {
				hh2[k] = cipher2[k];
			}
			element_from_hash(h2, hh2, cipher2.size());
			//element_printf("Hash: %B\n", hh);
			element_snprint(temp_char2, 2 * parame_length, h2);
			string h_str2 = ExtraG1(temp_char2);
			delete[] hh2;
			if (h_str2.compare(h_str1) == 0) {//验证密钥是否一致
				//cout << "密钥验证" << endl;
				ObjectName1.clear();
				local_path1.clear();
				ObjectName1 = "ssssss";
				local_path1 = "sssss";
				DownloadObjectRequest request4(BucketName, ObjectName1, local_path1);
				auto outcome4 = client.ResumableDownloadObject(request4);
				if (outcome4.isSuccess()) {
				}
				else {
					continue;
				}
				read_ptr.open(local_path1);
				string left1_str, right1_str;
				getline(read_ptr, left1_str);
				getline(read_ptr, right1_str);
				read_ptr.close();
				element_t left1, right1;
				element_init_G1(left1, pairing);
				element_init_G1(right1, pairing);
				const char* left1_ptr = left1_str.data();
				const char* right1_ptr = right1_str.data();
				element_set_str(left1, left1_ptr, 10);
				element_set_str(right1, right1_ptr, 10);

				
				element_t fl2,fr2;
				
				element_init_GT(fl2, pairing);
				element_init_GT(fr2, pairing);
				
				element_div(left1, left1, left);
				element_div(right1, right1, right);
				pairing_apply(fl2, left1, g, pairing);
				pairing_apply(fr2, right1, pk, pairing);



				if (!element_cmp(fl2, fr2)) {
					sim++;
				}
			}
		}
	}
	double sd = double(sim) / double(real_num);
	if (sd >= T) { //验证通过 不做处理
		cout << "验证通过" << endl;
	}
	else {//验证失败，与雾交互 追踪
		//
		string ObjectName = "Signature/" + doc_name + "/" + ver_ord + "_Signature.txt";
		string local_path = "C:/Users/62555/Desktop/Paper7/Cloud_verify/" + doc_name + "_" + ver_ord + "_Signature.txt";
		DownloadObjectRequest request1(BucketName, ObjectName, local_path);
		auto outcome1 = client.ResumableDownloadObject(request1);
		if (outcome1.isSuccess()) {
			std::cout << "文件下载成功" << outcome1.result().Metadata().ContentLength() << std::endl;
		}
		else {
			/* 异常处理。*/
			std::cout << "Get cloud public parameter fail" <<
				",code:" << outcome1.error().Code() <<
				",message:" << outcome1.error().Message() <<
				",requestId:" << outcome1.error().RequestId() << std::endl;
			ShutdownSdk();
			return -1;
		}
		fstream  read_ptr;
		read_ptr.open(local_path);
		string tau1_str, tau2_str, tau3_str;
		char tau1_char[MSGSIZE], tau2_char[MSGSIZE], tau3_char[MSGSIZE];
		getline(read_ptr, tau1_str);
		getline(read_ptr, tau2_str);
		getline(read_ptr, tau3_str);
		read_ptr.close();
		append(tau1_str, tau1_char);
		append(tau3_str, tau3_char);

		const char* tau2_ptr = tau2_str.data();
		element_t tau2;
		element_init_G1(tau2, pairing);
		element_set_str(tau2, tau2_ptr, 10);
		element_pow_zn(tau2, tau2, xi);
		element_snprint(tau2_char, 2 * parame_length, tau2);
		string tau2_str = ExtraG1(tau2_char);
		append(tau2_str, tau2_char);


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
		send(sClient, tau1_char, MSGSIZE, 0);
		closesocket(sClient);

		sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		while (connect(sClient, (sockaddr*)&server, sizeof(SOCKADDR)) == -1) {
		}
		send(sClient, tau2_char, MSGSIZE, 0);
		send(sClient, tau3_char, MSGSIZE, 0);
		closesocket(sClient);

		char Rd_char[MSGSIZE];
		sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		while (connect(sClient, (sockaddr*)&server, sizeof(SOCKADDR)) == -1) {
		}
		recv(sClient, Rd_char, MSGSIZE, 0);
		closesocket(sClient);
		string Rd_str = InfoExtra(Rd_char);

		select_sql.clear();
		select_sql = "xxxxx";
		const char* select_sql_char2 = select_sql.data();
		if (0 != mysql_query(&ceshi, select_sql_char2))
		{
			printf("查询失败: %s\n", mysql_error(&ceshi));
			mysql_close(&ceshi);                         //关闭连接
			system("pause");
			return 0;
		}                                      //存放查询sql语句字符串数组
		res = mysql_store_result(&ceshi);
		nextRow = mysql_fetch_row(res);

		string device = nextRow[0];
		cout << device << endl;

	}
	WSACleanup();
	system("pause");
	return 0;

}