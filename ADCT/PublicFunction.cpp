// PublicFunction.cpp: implementation of the CPublicFunction class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "PublicFunction.h"


#pragma warning(disable:4996)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
using namespace std;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPublicFunction::CPublicFunction()
{

}

CPublicFunction::~CPublicFunction()
{

}

CString CPublicFunction::GetCurDir()
{
	CString strRet;
	TCHAR strDir[MAX_PATH];
	int intLen = GetModuleFileName(NULL, strDir, MAX_PATH);
	strDir[intLen] = 0;
	strRet = (CString)strDir;
	int intTemp;
	int intPos = strRet.Find(_T("\\"));
	while (intPos > 0)
	{
		intTemp = intPos;
		intPos = strRet.Find(_T("\\"), intPos + 1);
	}
	if (intTemp > 0)
	{
		strRet = strRet.Mid(0, intTemp + 1);
	}
	return strRet;
}

CString CPublicFunction::HexStringOperation(CString sHex)
{
	CString sRet = "", sTemp;
	bool bContinue = false;
	for (int i = sHex.GetLength() - 1; i >= 0; i--)
	{
		bContinue = false;
		sTemp = sHex.Mid(i, 1);
		sTemp.MakeUpper();
		if (sTemp == "9")
		{
			sRet = "A" + sRet;
		}
		else if (sTemp == "A")
		{
			sRet = "B" + sRet;
		}
		else if (sTemp == "B")
		{
			sRet = "C" + sRet;
		}
		else if (sTemp == "C")
		{
			sRet = "D" + sRet;
		}
		else if (sTemp == "D")
		{
			sRet = "E" + sRet;
		}
		else if (sTemp == "E")
		{
			sRet = "F" + sRet;
		}
		else if (sTemp == "F")
		{
			sRet = "0" + sRet;
			bContinue = true;
		}
		else
		{
			sRet = IntToChar(atoi(sTemp) + 1, 0, 10) + sRet;
		}
		if (!bContinue)
		{
			if (i)
			{
				sRet = sHex.Left(i) + sRet;
			}
			break;
		}
	}
	if (bContinue)
	{
		sRet = "1" + sRet;
	}
	return sRet;
}


void CPublicFunction::IntToChar(int nIn, char *chOut, int nLen, int nHex)
{
	char chTemp[15];
	ZeroMemory(chTemp, 15);
	itoa(nIn, chTemp, nHex);
	int nTemp = strlen(chTemp);
	int i = 0;
	if (nLen > nTemp)
	{
		for (i = 0; i < nLen - nTemp; i++)
		{
			chOut[i] = '0';
		}

		strcpy(chOut + i, chTemp);
	}
	else if (nLen == 0)
	{
		strncpy(chOut, chTemp, nTemp);
		chOut[nTemp] = 0;
	}
	else
	{
		strncpy(chOut, chTemp, nLen);
		chOut[nLen] = 0;
	}
}

CString CPublicFunction::IntToChar(int nIn, int nLen, int nHex)
{
	char chTemp[15] = { 0 };
	IntToChar(nIn, chTemp, nLen, nHex);
	return chTemp;
}



int CPublicFunction::HexCStringToInt(CString sHex)
{
	switch (sHex.GetAt(0))
	{
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'a':
	case 'A':
		return 10;
	case 'b':
	case 'B':
		return 11;
	case 'c':
	case 'C':
		return 12;
	case 'd':
	case 'D':
		return 13;
	case 'e':
	case 'E':
		return 14;
	case 'F':
	case 'f':
		return 15;
	default:
		return 0;
	}
}

int CPublicFunction::CStringToInt(CString sIn)
{
	int nRet = 0;
	for (int i = 0; i < sIn.GetLength(); i++)
	{
		nRet = nRet * 16 + HexCStringToInt(sIn.Mid(i, 1));
	}
	return nRet;
}

int CPublicFunction::CStringTo10Int(CString sIn)
{
	int nRet = 0;
	for (int i = 0; i < sIn.GetLength(); i++)
	{
		nRet = nRet * 10 + HexCStringToInt(sIn.Mid(i, 1));
	}
	return nRet;
}

//CString CStringToPath(CString path)
//{
//	cout << path << endl;
//	for (size_t i = 0; i<path.size(); i++) {
//		if (ss[i] == '\\') {
//			ss.insert(i, string("\\"));
//			i++;
//		}
//	}
//	cout << path << endl;
//	return 0;
//}


wstring CPublicFunction::CharToWchar(const char* c, size_t m_encode = CP_ACP)
{
	std::wstring str;
	int len = MultiByteToWideChar(m_encode, 0, c, strlen(c), NULL, 0);
	wchar_t*	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(m_encode, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	str = m_wchar;
	delete m_wchar;
	return str;
}

string CPublicFunction::WcharToChar(const wchar_t * wp, size_t m_encode)
{
	std::string str;
	int len = WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	char	*m_char = new char[len + 1];
	WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	str = m_char;
	delete m_char;
	return str;
}
wchar_t * CPublicFunction::CharToWchart(char* code)
{

	int len1 = MultiByteToWideChar(CP_ACP, 0, code, -1, NULL, 0);
	wchar_t * pwtr = new wchar_t[len1];
	int len2 = MultiByteToWideChar(CP_ACP, 0, code, -1, pwtr, len1);
	setlocale(LC_ALL, "chs");
	wchar_t * output = pwtr;
	if (pwtr != NULL)
	{
		delete[] pwtr;
		pwtr = NULL;
	}
	return output;
}

//功能：二进制取反   
//   
//输入：const unsigned char *src  二进制数据   
//      int length                待转换的二进制数据长度   
//   
//输出：unsigned char *dst        取反后的二进制数据   
//   
//返回：0    success   
//   
//////////////////////////////////////////////////////   
int CPublicFunction::convert(unsigned char *dst, const unsigned char *src, int length)
{
	int i;
	for (i = 0; i < length; i++)
	{
		dst[i] = src[i] ^ 0xFF;
	}
	return 0;
}
//////////////////////////////////////////////////////////   
//   
//功能：十六进制转为十进制   
//   
//输入：const unsigned char *hex         待转换的十六进制数据   
//      int length                       十六进制数据长度   
//   
//输出：   
//   
//返回：int  rslt                        转换后的十进制数据   
//   
//思路：十六进制每个字符位所表示的十进制数的范围是0 ~255，进制为256   
//      左移8位(<<8)等价乘以256   
//   
/////////////////////////////////////////////////////////   
unsigned long CPublicFunction::HextoDec(const unsigned char *hex, int length)
{
	int i;
	unsigned long rslt = 0;
	for (i = 0; i < length; i++)
	{
		rslt += (unsigned long)(hex[i]) << (8 * (length - 1 - i));

	}
	return rslt;
}

/////////////////////////////////////////////////////////   
//   
//功能：十进制转十六进制   
//   
//输入：int dec                     待转换的十进制数据   
//      int length                  转换后的十六进制数据长度   
//   
//输出：unsigned char *hex          转换后的十六进制数据   
//   
//返回：0    success   
//   
//思路：原理同十六进制转十进制   
//////////////////////////////////////////////////////////   
int CPublicFunction::DectoHex(int dec, unsigned char *hex, int length)
{
	int i;
	for (i = length - 1; i >= 0; i--)
	{
		hex[i] = (dec % 256) & 0xFF;
		dec /= 256;
	}
	return 0;
}
/////////////////////////////////////////////////////////   
//   
//功能：求权   
//   
//输入：int base                    进制基数   
//      int times                   权级数   
//   
//输出：   
//   
//返回：unsigned long               当前数据位的权   
//   
//////////////////////////////////////////////////////////   
unsigned long CPublicFunction::power(int base, int times)
{
	int i;
	unsigned long rslt = 1;
	for (i = 0; i < times; i++)
		rslt *= base;
	return rslt;
}
/////////////////////////////////////////////////////////   
//   
//功能：BCD转10进制   
//   
//输入：const unsigned char *bcd     待转换的BCD码   
//      int length                   BCD码数据长度   
//   
//输出：   
//   
//返回：unsigned long               当前数据位的权   
//   
//思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100   
//      先求每个字符所表示的十进制值，然后乘以权   
//////////////////////////////////////////////////////////   
unsigned long  CPublicFunction::BCDtoDec(const unsigned char *bcd, int length)
{
	int i, tmp;
	unsigned long dec = 0;
	for (i = 0; i < length; i++)
	{
		tmp = ((bcd[i] >> 4) & 0x0F) * 10 + (bcd[i] & 0x0F);
		dec += tmp * power(100, length - 1 - i);
	}
	return dec;
}
/////////////////////////////////////////////////////////   
//   
//功能：十进制转BCD码   
//   
//输入：int Dec                      待转换的十进制数据   
//      int length                   BCD码数据长度   
//   
//输出：unsigned char *Bcd           转换后的BCD码   
//   
//返回：0  success   
//   
//思路：原理同BCD码转十进制   
//   
//////////////////////////////////////////////////////////   
int CPublicFunction::DectoBCD(int Dec, unsigned char *Bcd, int length)
{
	int i;
	int temp;
	for (i = length - 1; i >= 0; i--)
	{
		temp = Dec % 100;
		Bcd[i] = ((temp / 10) << 4) + ((temp % 10) & 0x0F);
		Dec /= 100;
	}
	return 0;
}

//下面函数会阻塞
//void Sleep(int time)//time*1000为秒数 
//{
//
//	clock_t   now = clock();
//
//	while (clock() - now < time);
//
//}

int CPublicFunction::GetComList(CStringArray *PortList)//获取可用com口支持256个
{
	CString  strCom, strComOpen;
	int  nCom = 0;
	int  count = 0;
	HANDLE    hCom;
	PortList->RemoveAll();
	do
	{
		nCom++;
		strCom.Format("COM%d", nCom);
		strComOpen.Format("\\\\.\\COM%d", nCom);
		hCom = CreateFile(strComOpen, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		if (INVALID_HANDLE_VALUE == hCom)
		{
			DWORD error = ::GetLastError();//取得错误信息 
		}
		else
		{
			PortList->Add(strCom);
			count++;
		}
		CloseHandle(hCom);
	} while (nCom < 256);
	return count;
}

CString CPublicFunction::UART_CRC16_Work(unsigned char CRC_Buf[], int CRC_Leni)
{
	int i, j;
	unsigned short CRC_Sumx,CRC_Temp;
	CString CRC_Rec;
	CRC_Sumx = 0xFFFF;

	for (i = 0; i < CRC_Leni; i++)
	{
		CRC_Sumx = (CRC_Buf[i]) ^ CRC_Sumx;//异或
		for (j = 0; j < 8; j++)
		{
			if (CRC_Sumx & 0x01)
			{
				CRC_Sumx >>= 1;
				CRC_Sumx ^= 0xa001;
			}
			else
			{
				CRC_Sumx >>= 1;
			}
		}
	}
	CRC_Temp = CRC_Sumx >> 8;
	CRC_Temp = CRC_Temp | (CRC_Sumx << 8);
	printf("%X\n", CRC_Temp);
	CRC_Rec.Format("%X", CRC_Temp);
	return (CRC_Rec);
}


int CPublicFunction::TStringToHex(char s[], unsigned char bits[]) {
	int i, n = 0;
	for (i = 0; s[i]; i += 2) {
		if (s[i] >= 'A' && s[i] <= 'F')
			bits[n] = s[i] - 'A' + 10;
		else bits[n] = s[i] - '0';
		if (s[i + 1] >= 'A' && s[i + 1] <= 'F')
			bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
		else bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
		++n;
	}
	return n;
}

BYTE * CPublicFunction::CStrToByte(int len, CString str)
{
	BYTE *Buf = new BYTE[len];
	CString StrSub;
	for (int iSrc = 0, iDst = 0; iDst < len; iSrc += 2, iDst++)
	{
		StrSub = str.Mid(iSrc, 2);
		Buf[iDst] = (BYTE)_tcstoul(StrSub, NULL, 16);//16是十六进制的意思
	}
	return Buf;
}

