// PublicFunction.h: interface for the CPublicFunction class.
//
//////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#pragma once
#endif 
#include "afxcoll.h"
#include <string>
using namespace std;
//extern void Sleep(int time);
class CPublicFunction  
{
public:
	CString GetCurDir();
	CString HexStringOperation(CString sHex);
	void IntToChar(int nIn, char *chOut, int nLen, int nHex);
	CString IntToChar(int nIn, int nLen, int nHex);
	int HexCStringToInt(CString sHex);
	int CStringToInt(CString sIn);
	int CStringTo10Int(CString sIn);
	wstring CharToWchar(const char * c, size_t m_encode);
	string WcharToChar(const wchar_t* wp, size_t m_encode = CP_ACP);
	wchar_t * CharToWchart(char * code);
	int convert(unsigned char * dst, const unsigned char * src, int length);
	unsigned long HextoDec(const unsigned char * hex, int length);
	int DectoHex(int dec, unsigned char * hex, int length);
	unsigned long power(int base, int times);
	unsigned long BCDtoDec(const unsigned char * bcd, int length);
	int DectoBCD(int Dec, unsigned char * Bcd, int length);
	int GetComList(CStringArray *PortList);
	CPublicFunction();
	virtual ~CPublicFunction();
	CString UART_CRC16_Work(unsigned char CRC_Buf[], int CRC_Leni);
	int TStringToHex(char s[], unsigned char bits[]);
	BYTE * CStrToByte(int len, CString str);
};
