#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <tchar.h>
using namespace std;
#define MAX_KEY_LENGTH 1000
#define MAX_VALUE_NAME 16383
#define KEY_SET_SHARE "SYSTEM\\ControlSet001\\Services\\LanmanServer\\Shares\\Security"
typedef struct SHARE{
	TCHAR *sharename[MAX_KEY_LENGTH];
	unsigned long count;
	int Status[MAX_KEY_LENGTH];
	int anymous;
}share;
SHARE Check_Share_Authority_Key() 
{ 


	HKEY hKey;
	SHARE share;
	share.anymous = 0;

	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,_T(KEY_SET_SHARE),0,KEY_READ,&hKey) == ERROR_SUCCESS
		)
	{

		TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
		//DWORD    cbName;                   // size of name string 
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
		unsigned char shareValue[MAX_VALUE_NAME];
		DWORD shareType;
		DWORD shareSize=100;
		shareType=REG_BINARY;	
		//	shareSize=wcslen(shareValue);	


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

		// Enumerate the key values. 
		//列出子键
		if (cValues!=0) 
		{
			//       sprintf(share.count,"%s", cValues);
			share.count = cValues;

			for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
			{ 
				cchValue = MAX_VALUE_NAME; 
				shareSize=MAX_KEY_LENGTH;
				achKey[0] = '\0'; 
				retCode = RegEnumValue(hKey, i, 
					achValue, 
					&cchValue, 
					NULL, 
					NULL,
					NULL,
					NULL);

				if (retCode == ERROR_SUCCESS ) 
				{ 

					TCHAR *str = new TCHAR[MAX_KEY_LENGTH];		
					_stprintf(str,_T("%s"),achValue);
					share.sharename[i] = str;	
		
					LONG ret0=RegQueryValueEx(hKey,achValue,NULL,&shareType, shareValue, &shareSize);  //查询共享的键值


					if(ret0==ERROR_SUCCESS)
					{
						if(shareValue[8] == 'L')

						{	share.Status[i] = 1;
							share.anymous++;
						}
						else 

							share.Status[i] = 0;


					}
				//	delete[] str;
				} 

			}

		}
		else {
			share.count = 0;

		}

	}	
	//delete[] str;
	RegCloseKey(hKey);
	return share;

}

