#include "stdafx.h"
#include "iostream"  
#include "string"  
#include "vector"
#include "SQLManager.h"


_ConnectionPtr m_pConnection;
_CommandPtr m_pCommand;
_RecordsetPtr m_pRecordSet;
bool SQLIsUsing = false;
SQLSettingInfo sqlSettingInfo;

SQLManager::SQLManager()
{

}


SQLManager::~SQLManager()
{
}
//bool SQLManager::Init()
//{
//	if (!SQLConfigDataSource(NULL, ODBC_ADD_DSN, "MySQL ODBC 5.1 Driver\0",
//		"DSN=BLETest\0"
//		"ADDRESS=120.198.40.186\0"
//		"Port=5432\0"
//		"UID=root\0"
//		"PWD=newttl!@#$1234\0"
//		"DATABASE=BLETest\0\0"
//	))
//	{
//		MessageBox(NULL, "注册数据源DSN失败!", "", NULL);
//		return false;
//	}
//	else
//	{
//		MessageBox(NULL, "注册数据源DSN成功!", "", NULL);
//		return true;
//	}
//}
bool SQLManager::ConnSQL()
{
	CoInitialize(NULL); //初始化COM环境           
	m_pConnection.CreateInstance("ADODB.Connection");
	//初始化Connection指针
	CString conn;
	conn.Format("Provider = SQLOLEDB.1; Password = %s; Persist Security Info = True; User ID = %s; Initial Catalog = %s; Data Source = %s", sqlSettingInfo.Password, sqlSettingInfo.UserName, sqlSettingInfo.Database, sqlSettingInfo.SQLAddress);
	//conn.Format(("Driver={MySQL ODBC 5.1 Driver};Server=%s;Database=%s;Port=%s;User=%s;Password=%s;"), "10.10.11.149","adct","3306","root","123456");//sqlSettingInfo.SQLAddress, sqlSettingInfo.Database, sqlSettingInfo.Port, sqlSettingInfo.UserName, sqlSettingInfo.Password);
	//AfxMessageBox((_bstr_t)conn);
	try
	{
		/*打开数据库“MySQL”，需要根据数据库的情况 */
		m_pConnection->ConnectionTimeout = 10;
		m_pConnection->CommandTimeout = 10;
		m_pConnection->Open((_bstr_t)conn, "", "", adModeUnknown);
		return true;
	}
	catch (_com_error &e)
	{

		AfxMessageBox("数据库连接异常");
		return 0;
	}

}

bool SQLManager::CloseAll()
{
	try
	{
		if (m_pRecordSet->GetState() == adStateOpen)
		{
			m_pRecordSet->Close();
		}
		if (m_pRecordSet)
		{
			m_pRecordSet.Release();
			m_pRecordSet = NULL;
		}
		if (m_pConnection->GetState() == adStateOpen)
		{
			m_pConnection->Close();
		}
		if (m_pConnection)
		{
			m_pConnection.Release();
			m_pConnection = NULL;
		}
	}
	catch (const std::exception&)
	{

	}
	
	return 0;
}

_RecordsetPtr SQLManager::SQLCommand(CString command)
{
	m_pRecordSet.CreateInstance(__uuidof(Recordset));
	try
	{
		
		m_pRecordSet = m_pConnection->Execute(_bstr_t(command), NULL, adCmdText);
	}
	catch (const std::exception&)
	{
	}
	
	return m_pRecordSet;
}
