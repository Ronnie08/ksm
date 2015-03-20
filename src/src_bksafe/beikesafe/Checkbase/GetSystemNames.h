#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <tchar.h>
#include <Aclapi.h> 
using namespace std;
#pragma comment (lib,"Advapi32.lib") 
using namespace std;
#define MAX_KEY_LENGTH 1000
#define MAX_VALUE_NAME 16383
#define KEY_SET_NAME  _T("SAM\\SAM\\Domains\\Account\\Users\\Names")

typedef struct ACCOUNT{
	TCHAR *systemaccount[MAX_KEY_LENGTH];
	unsigned long count;
	DWORD Type;
}acc;

ACCOUNT AccountList() 
{ 
	

	HKEY hKey;
	ACCOUNT acc;

	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,KEY_SET_NAME,0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{

    //TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    TCHAR  achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
	TCHAR shareValue[MAX_VALUE_NAME];
	DWORD shareType;
	DWORD shareSize;
	shareType=REG_BINARY;	
	shareSize=wcslen(shareValue);	

 
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 
 
    // Enumerate the subkeys, until RegEnumKeyEx fails.
    
	  if (cSubKeys)
    {
	
		acc.count = cSubKeys;
		  //sprintf(acc.count, "%s", cSubKeys);

        for (i=0; i<cSubKeys; i++) 
        { 
            cbName = MAX_KEY_LENGTH;
			achValue[0] = '\0'; 
            retCode = RegEnumKeyEx(hKey, i,
                     achValue, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 
            if (retCode == ERROR_SUCCESS) 
            {
                
				//angellover
				TCHAR *str = new TCHAR[MAX_KEY_LENGTH];
				_stprintf(str,_T("%s"),achValue);
				acc.systemaccount[i] = str;	
				delete[] str;

            }

		}

	
	
    }	

	}
	else
	{
	// printf("打开注册表失败");
	     MessageBox(NULL,_T("打开注册表失败..."), _T("账号检查"), MB_ICONINFORMATION);		
	}
	return acc;
	RegCloseKey(hKey);
}
ACCOUNT Change_SAM_Access()
{ 

	ACCOUNT acc;
	DWORD dwRet; 
    LPWSTR SamName = _T("MACHINE\\SAM\\SAM"); 
    PSECURITY_DESCRIPTOR pSD = NULL; 
    PACL pOldDacl = NULL; 
    PACL pNewDacl = NULL; 
    EXPLICIT_ACCESS ea; 
    HKEY hKey = NULL; 

    // 获取SAM主键的DACL 
    dwRet = GetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, 
                NULL, NULL, &pOldDacl, NULL, &pSD); 
    if (dwRet != ERROR_SUCCESS) 
    { 
        printf("GetNamedSecurityInfo Error: %d\n", dwRet); 
        goto FreeAndExit; 
    } 

    // 创建一个ACE，允许Everyone完全控制对象，并允许子对象继承此权限 
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS)); 
    BuildExplicitAccessWithName(&ea, _T("Everyone"), KEY_ALL_ACCESS, SET_ACCESS, 
        SUB_CONTAINERS_AND_OBJECTS_INHERIT); 

    // 将新的ACE加入DACL 
    dwRet = SetEntriesInAcl(1, &ea, pOldDacl, &pNewDacl); 
    if (dwRet != ERROR_SUCCESS) 
    { 
        printf("SetEntriesInAcl Error: %d\n", dwRet); 
        goto FreeAndExit; 
    } 

    // 更新SAM主键的DACL 
    dwRet = SetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, 
                NULL, NULL, pNewDacl, NULL); 
    if (dwRet != ERROR_SUCCESS) 
    { 
        printf("SetNamedSecurityInfo Error: %d\n", dwRet); 
        goto FreeAndExit; 
    } 

    // 打开SAM的子键 
    dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SAM\\SAM\\Domains\\Account\\Users\\000001F4"), 
                0, KEY_ALL_ACCESS, &hKey); 
    if (dwRet != ERROR_SUCCESS) 
    { 
        printf("RegOpenKeyEx Error: %d\n", dwRet); 
        goto FreeAndExit; 
    } 
	acc=AccountList();
	   

FreeAndExit: 
    if (hKey) RegCloseKey(hKey); 
    if (pNewDacl) LocalFree(pNewDacl); 
    // 还原SAM主键的DACL 
    if (pOldDacl) SetNamedSecurityInfo(SamName, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, 
                        NULL, NULL, pOldDacl, NULL); 
    if (pSD) LocalFree(pSD); 
	return acc;
}


