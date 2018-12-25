#pragma once
class WinRARClass
{
public:
	WinRARClass();
	~WinRARClass();
	void UnpackFile(const CString & strFilePath);
	static void DeleteDirectories(CString csPath);
	void FindWinRARPath();
};

