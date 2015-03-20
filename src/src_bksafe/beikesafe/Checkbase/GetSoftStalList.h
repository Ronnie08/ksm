#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <tchar.h>
#include <Aclapi.h> 
using namespace std;
#pragma comment (lib,"Advapi32.lib") 
#define MAX_KEY_LENGTH 1000
#define MAX_VALUE_NAME 16383
#define MAX_PATH2 1000
typedef struct SOFTLIST{
	TCHAR *softname[MAX_KEY_LENGTH];
	TCHAR *softversion[MAX_KEY_LENGTH];
	unsigned long count;
	DWORD Type;
}softlist;
SOFTLIST Soft_Stall()
{

	SOFTLIST softlist;

	TCHAR szSubKey[MAX_PATH2] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	HKEY hKey, hSubKey;
	LONG lRetVal, lRetVal2;
	DWORD nIndex = 0;
	DWORD nNameLen = MAX_PATH2;
	DWORD nDisplayLen = MAX_PATH2;
	DWORD nUninstallLen = MAX_PATH2;
	TCHAR szName[MAX_PATH2];
	TCHAR szDisplayName[MAX_PATH2];
	TCHAR szDisplayVersion[MAX_PATH2];
	TCHAR szUninstall[MAX_PATH2];
	lRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_ALL_ACCESS, &hKey);
	DWORD nAttribute = REG_BINARY|REG_DWORD|REG_SZ|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE;
	int i=0;

	if(ERROR_SUCCESS == lRetVal)
	{
		lRetVal = RegEnumKeyEx(hKey, nIndex, szName, &nNameLen, 0, NULL, NULL, 0);
		//printf("ÐòºÅ\tÈí¼þÃû³Æ\n");

		while(lRetVal != ERROR_NO_MORE_ITEMS)
		{
			nIndex++;
			if(_tcscmp(szName, _T("")) == 0) continue;
			_stprintf(szUninstall, _T("%s\\%s"), szSubKey, szName);
		//	memset(szDisplayName, 0, MAX_PATH2);
		//TCHAR szDisplayName[MAX_PATH2] = new TCHAR[MAX_PATH2];
			ZeroMemory (szDisplayName,MAX_PATH2);
			nDisplayLen = MAX_PATH2;
			lRetVal2 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szUninstall, 0, KEY_ALL_ACCESS, &hSubKey);
			if(lRetVal2 == ERROR_SUCCESS)
			{   
				
				RegQueryValueEx(hSubKey, _T("DisplayName"), 0, &nAttribute, (LPBYTE)szDisplayName, &nDisplayLen);
				
				RegQueryValueEx(hSubKey, _T("DisplayVersion"), 0, &nAttribute, (LPBYTE)szDisplayVersion, &nDisplayLen);
				
				if(_tcscmp(szDisplayName, _T("")) != 0)
				{

					TCHAR *str = new TCHAR[MAX_KEY_LENGTH];
					TCHAR *str1 = new TCHAR[MAX_KEY_LENGTH];
						_stprintf(str,_T("%s"),szDisplayName);
						_stprintf(str1,_T("%s"),szDisplayVersion);

						softlist.softname[i] = str;
						softlist.softversion[i] = str1;
					i++;
					//delete[] str;
					//delete[] str1;
				}
			}
			nNameLen = MAX_PATH2;
			ZeroMemory (szName, MAX_PATH2);
			lRetVal = RegEnumKeyEx(hKey, nIndex, szName, &nNameLen, 0, NULL, NULL, 0);
		}
	}
	//	else
	//	{
	//		     MessageBox(NULL,_T("´ò¿ª×¢²á±íÊ§°Ü..."), _T("ÕËºÅ¼ì²é"), MB_ICONINFORMATION);		
	//	}
	softlist.count = i;
	RegCloseKey(hKey);
	

	return softlist;

}