#include <stdio.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <tchar.h>

#ifdef _UNICODE
#endif
using namespace std;
#pragma comment(lib, "wsock32.lib")
#define  KEY_SET_INFO  _T("SYSTEM\\ControlSet001\\Services\\Tcpip\\Parameters")
#define  KEY_NAME	_T("Domain")
#define MAX_VALUE_NAME 16383
CString GetOSVersionInfo()
{
	int a=0,b=0,i=0,j=0;
	_asm
	{
		pushad
			mov ebx,fs:[0x18] ; get self pointer from TEB
			mov eax,fs:[0x30] ; get pointer to PEB / database
			mov ebx,[eax+0A8h] ; get OSMinorVersion
			mov eax,[eax+0A4h] ; get OSMajorVersion
			mov j,ebx
			mov i,eax
			popad
	}
	if((i==5)&&(j==0))
	{
		return " Windows 2000";
	}
	else if((i==5)&&(j==1))
	{  
		return "Windows XP";
	}
	else if((i==5)&&(j==2))
	{
		return "Windows server 2003";
	}
	else if((i==6)&&(j==0))
	{
		return "Windows Vista";
	}
	else if((i==6)&&(j==1))
	{
		return "Windows 7";
	}
}
typedef struct COMPUTERINFO{
	char *computername;
	char *computerip;
	TCHAR *domain;
	CString osversion;
}info;
TCHAR *getdoamin();
COMPUTERINFO computerinfo()
{
	COMPUTERINFO info ;
	//info ={ "loverangel", "127.0.0.1", _T("yulong","xp") };
	//info.computername =new char;
	//info.computerip = new char;
	//info.domain = new TCHAR;
	//info.osversion = new CString;
	char *buf = new char[MAX_KEY_LENGTH]; 

	struct hostent *ph = 0;
	WSADATA w; WSAStartup(0x0101, &w); //这一行必须在使用任何SOCKET函数前写！

	if(gethostname(buf, 256))
	{ 
		//fprintf(stderr, "gethostname() fault %d\n", GetLastError());buf
		info.computername =  "Error";
	}
	else
	{


		info.computername = buf  ;

	}
	ph = gethostbyname(buf);
	//printf("本机IP:%s\n",  inet_ntoa(*((struct in_addr *)ph->h_addr_list[0])));
	//	sprintf(buf,"%s",inet_ntoa(*((struct in_addr *)ph->h_addr_list[0]));
	//CString ipbuf;
	//ipbuf.Format(()*((struct in_addr *)ph->h_addr_list[0]));
	char *ipbuf = new char[MAX_KEY_LENGTH];
	sprintf(ipbuf,"%s",inet_ntoa(*((struct in_addr *)ph->h_addr_list[0])));
	info.computerip = ipbuf;
	WSACleanup();

	info.domain = getdoamin();
	info.osversion = GetOSVersionInfo();
	//delete  info.computername;
	//delete  info.computerip;
	//delete  info.domain;
	//delete  info.osversion;
	//delete[] buf;
	//delete[]  ipbuf;

	return info;
}
TCHAR *getdoamin()
{
	HKEY hkey;
	TCHAR *domain;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,KEY_SET_INFO,0,KEY_READ,&hkey)==ERROR_SUCCESS)          //打开域名存储位置
	{
		// printf("打开键值成功\n");


		TCHAR shareValue[MAX_VALUE_NAME] ;
		DWORD shareType;
		DWORD shareSize = 100;
		shareType=REG_SZ;
	//	shareSize= strlen(shareValue);	


		//读取域信息

		LONG ret0=RegQueryValueEx(hkey,KEY_NAME,NULL,&shareType, (LPBYTE)shareValue, &shareSize);  
		if(ret0==ERROR_SUCCESS)
		{	

			if(shareSize==1)                   //Domain的REG_SZ值为空时，代表未加入任何域
			{	
				//printf("未加入任何域\n");      //此时shareSize值为1
				domain = _T("");
			}
			else
			{
				//printf("已加入公司域：");
				//printf("%s\n",shareValue);
				domain = shareValue;
			}

		}
		else
		{	

		     MessageBox(NULL,_T("查询注册表失败..."), _T("读取域信息"), MB_ICONINFORMATION);		
		}


		RegCloseKey(hkey);


	}
	else
	{	

	     MessageBox(NULL,_T("打开注册表失败..."), _T("读取域信息"), MB_ICONINFORMATION);		
	}
	//delete  domain;
	return domain;

}