#pragma once
#include "stdafx.h"
#import "msado15.dll"  no_namespace rename("EOF", "adoEOF")

typedef struct 
{
	CString SQLAddress;
	CString Port;
	CString UserName;
	CString Password;
	CString Database;
	CString FTPAddress;
	CString FTPPort;
	CString FTPUserName;
	CString FTPPWD;
}SQLSettingInfo;

extern SQLSettingInfo sqlSettingInfo;

extern bool SQLIsUsing;
class SQLManager
{
public:
	SQLManager();
	~SQLManager();
	bool ConnSQL();
	bool CloseAll();
	_RecordsetPtr SQLCommand(CString command);

};