
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
#define PORT_LOC 10110
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
	const int Blocknum = 9;
	string ord = "xx";
	string File_str = "xxxx";
	MYSQL ceshi;
	mysql_init(&ceshi);
	MYSQL_ROW row;

	
	std::string AccessKeyId = "xxxx";
	std::string AccessKeySecret = "xxxx";
	std::string Endpoint = "xxx";
	std::string BucketName = "xxxx";
	InitializeSdk();
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);


	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;//服务器地址
	SOCKADDR client_user; //客户端地址
	memset(&local, 0, sizeof(local));
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		cout << "sListent是错的" << endl;
	}

	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC);
	local.sin_addr.s_addr = htonl(INADDR_ANY);// 任何一个客户端IP都可以连接

	while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
	}
	cout << "Bind成功" << endl;
	while (listen(sListen, 20) == -1) {
	}
	cout << "Listen结束" << endl;
	cout << "等待client的连接请求" << endl;

	char szMessage[MSGSIZE];
	string blockNum_str = to_string(Blocknum);
	append(blockNum_str, szMessage);
	int nsize = sizeof(SOCKADDR);
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	send(sClient, szMessage, MSGSIZE, NULL);
	closesocket(sClient);


	for (int l = 1; l <= Blocknum; l++) {
		string ObjectName1 = "xxxxxx";
		string local_path1 = "xxxxx";
		DownloadObjectRequest request1(BucketName, ObjectName1, local_path1);
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
			system("pause");
			return -1;
		}
		string Hex_ciphertext;
		fstream read_ptr;
		read_ptr.open(local_path1);
		getline(read_ptr, Hex_ciphertext);
		read_ptr.close();
		char HexCipher_char[MSGSIZE], CipherLen_char[MSGSIZE];
		string CipherLen_str = to_string(Hex_ciphertext.size());
		append(CipherLen_str, CipherLen_char);
		
		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, CipherLen_char, MSGSIZE, NULL);
		closesocket(sClient);

		int send_time = ceil(double(atoi(CipherLen_str.c_str())) / double(MSGSIZE));
		int now_ord = 0;
		for (int i = 0; i<send_time; i++) {
			if (i == send_time - 1) {
				for (int j = 0; j<(atoi(CipherLen_str.c_str()) - i*MSGSIZE); j++) {
					HexCipher_char[j] = Hex_ciphertext[now_ord];
					now_ord++;
				}
			}
			else {
				for (int j = 0; j<MSGSIZE; j++) {
					HexCipher_char[j] = Hex_ciphertext[now_ord];
					now_ord++;
				}
			}
			listen(sListen, 1);
			sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
			send(sClient, HexCipher_char, MSGSIZE, NULL);
			closesocket(sClient);
		}
		
		ObjectName1.clear();
		local_path1.clear();
		ObjectName1 = "xxxxx";
		local_path1 = "xxxxx";
		DownloadObjectRequest request2(BucketName, ObjectName1, local_path1);
		auto outcome2 = client.ResumableDownloadObject(request2);
		if (outcome2.isSuccess()) {
			std::cout << "文件下载成功" << outcome2.result().Metadata().ContentLength() << std::endl;
		}
		else {
			/* 异常处理。*/
			std::cout << "Get cloud public parameter fail" <<
				",code:" << outcome2.error().Code() <<
				",message:" << outcome2.error().Message() <<
				",requestId:" << outcome2.error().RequestId() << std::endl;
			ShutdownSdk();
			system("pause");
			return -1;
		}
		string left_str, right_str;
		read_ptr.open(local_path1);
		getline(read_ptr, left_str);
		getline(read_ptr, right_str);
		read_ptr.close();
		char left_char[MSGSIZE], right_char[MSGSIZE];
		cout << left_str << endl;
		cout << right_str << endl;
		append(left_str, left_char);
		append(right_str, right_char);
		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, left_char, MSGSIZE, NULL);
		closesocket(sClient);

		listen(sListen, 1);
		sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
		send(sClient, right_char, MSGSIZE, NULL);
		closesocket(sClient);
	}


	WSACleanup();
	system("pause");
	return 0;

}