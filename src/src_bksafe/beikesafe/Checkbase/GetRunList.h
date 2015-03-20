#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <tchar.h>
using namespace std;

#define MAX_KEY_LENGTH 1000
#define MAX_VALUE_NAME 16383

typedef struct RUNLIST{
	TCHAR*a[MAX_KEY_LENGTH];
	TCHAR *b[MAX_KEY_LENGTH];
	unsigned long count;
}runlist;

RUNLIST RunList(LPCWSTR key_set) 
{ 
	RUNLIST runlist;
	HKEY hKey;

	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE , key_set,0,KEY_READ,&hKey) == ERROR_SUCCESS
		)
	{

		//TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
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

		DWORD i,retCode; 

		TCHAR  achValue[MAX_VALUE_NAME]; 
		DWORD cchValue = MAX_VALUE_NAME; 
		TCHAR shareValue[MAX_VALUE_NAME];
		DWORD shareType;
		DWORD shareSize;
		shareType=REG_BINARY;	
		shareSize = wcslen(shareValue);	


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


		// Enumerate the key values. 
		//�г��Ӽ�
		if (cValues!=0) 
		{
			//  sprintf(runlist.count, "%d", cValues);
			runlist.count = cValues;
			int k=0;

			for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
			{ 
				cchValue = MAX_VALUE_NAME; 
				shareSize=MAX_KEY_LENGTH;
				achValue[0] = '\0'; 
				retCode = RegEnumValue(hKey, i, 
					achValue, 
					&cchValue, 
					NULL, 
					NULL,
					NULL,
					NULL);

				if (retCode == ERROR_SUCCESS ) 
				{ 

					// _tprintf(TEXT("(%d) %s\t\t\t\t"), i+1, achValue); 
			//		char *aa = new char[255];
			//		char *bb = new char[255];



					shareValue[0] = '\0'; 

					LONG ret0=RegQueryValueEx(hKey,achValue,NULL,&shareType, (LPBYTE)shareValue, &shareSize);  //��ѯ������ļ�ֵ

					//	printf("%s\n",shareValue);
															//����+λ��
				//	sprintf(aa,"%s",achValue);
				//	sprintf(bb,"%s",shareValue);
		
				

					
					TCHAR *str = new TCHAR[MAX_KEY_LENGTH];
					TCHAR *str1 = new TCHAR[MAX_KEY_LENGTH];
					_stprintf(str,_T("%s"),achValue);
					_stprintf(str1,_T("%s"),shareValue);
					runlist.a[i] =  str  ;
					runlist.b[i] = str1;
					//
				//	delete[] str;
				//	delete[] str1;

				} 



			}


		}
		else

			runlist.count = 0;
	}
	else
	{
		MessageBox(NULL,_T("��ע���ʧ��..."), _T("������1"), MB_ICONINFORMATION);	
		//	exit(-1);
	}

	RegCloseKey(hKey);
	return runlist;
}
RUNLIST RunList1(LPCWSTR key_set) 
{ 
	RUNLIST runlist;
	HKEY hKey;

	if( RegOpenKeyEx( HKEY_CURRENT_USER , key_set,0,KEY_READ,&hKey) == ERROR_SUCCESS
		)
	{

		//TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
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

		DWORD i,retCode; 

		TCHAR  achValue[MAX_VALUE_NAME]; 
		DWORD cchValue = MAX_VALUE_NAME; 
		TCHAR shareValue[MAX_VALUE_NAME];
		DWORD shareType;
		DWORD shareSize;
		shareType=REG_BINARY;	
		shareSize = wcslen(shareValue);	


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


		// Enumerate the key values. 
		//�г��Ӽ�
		if (cValues!=0) 
		{
			//  sprintf(runlist.count, "%d", cValues);
			runlist.count = cValues;
			int k=0;

			for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
			{ 
				cchValue = MAX_VALUE_NAME; 
				shareSize=MAX_KEY_LENGTH;
				achValue[0] = '\0'; 
				retCode = RegEnumValue(hKey, i, 
					achValue, 
					&cchValue, 
					NULL, 
					NULL,
					NULL,
					NULL);

				if (retCode == ERROR_SUCCESS ) 
				{ 

					// _tprintf(TEXT("(%d) %s\t\t\t\t"), i+1, achValue); 
					//		char *aa = new char[255];
					//		char *bb = new char[255];
					shareValue[0] = '\0'; 

					LONG ret0=RegQueryValueEx(hKey,achValue,NULL,&shareType, (LPBYTE)shareValue, &shareSize);  //��ѯ����ļ�ֵ

					//	printf("%s\n",shareValue);
					//����+λ��
					//	sprintf(aa,"%s",achValue);
					//	sprintf(bb,"%s",shareValue);
					TCHAR *str  = new TCHAR[MAX_VALUE_NAME];
					TCHAR *str1 = new TCHAR[MAX_VALUE_NAME];

					_stprintf(str,_T("%s"),achValue);
					_stprintf(str1,_T("%s"),shareValue);
					runlist.a[i] =  str  ;
					runlist.b[i] = str1;

				} 



			}


		}
		else

			runlist.count = 0;
	}
	else
	{
		MessageBox(NULL,_T("��ע���ʧ��..."), _T("������2"), MB_ICONINFORMATION);	
		//	exit(-1);
	}

	RegCloseKey(hKey);
	return runlist;
}

