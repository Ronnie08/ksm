#include "stdafx.h"
#include "iostream"
#include "windows.h"
#include "lm.h"
#include "assert.h"
#include "string"
#include "time.h"
#include <ctime>

using namespace std;
#pragma comment(lib,"netapi32.lib")

//判断帐户状态:数组各个值对应的意义如下
/*

序号 Value              Mean                                          Calendar Value
01   UF_SCRIPT      登录脚本执行                                      1   
02   UF_ACCOUNTDISABLE    用户帐户不可用                   2
03      
04   UF_HOMEDIR_REQUIRED    要求有用户目录              8
02   UF_LOCKOUT      帐户锁定                                          16
06   UF_PASSWD_NOTREQD    不要求用户密码                32
07   UF_PASSWD_CANT_CHANGE   不能更改密码            64

08

   以下为帐户类型:
09   UF_TEMP_DUPLICATE_ACCOUNT 域用户帐户           256
10   UF_NORMAL_ACCOUNT    普通帐户                            512
11
12   UF_INTERDOMAIN_TRUST_ACCOUNT域内可信任帐户   2048
13   UF_WORKSTATION_TRUST_ACCOUNT工作组可信任帐户 4096
14   UF_SERVER_TRUST_ACCOUNT   备份域控制器帐户 8192
15
16 
17   UF_DONT_EXPIRE_PASSWD   密码不过期                  65536

说明:用ToBinary(int,int)这个函数判断各个序号对应的值是否为真。
*/
int ToBinary(unsigned long n,int k)
{
int num[17];
int i=16;
//密码永不过期
unsigned long m=n;
if(k>17||k<1)
   k=1;
while(m!=0)
{
  
   num[i]=m%2;
   m=m/2;
   i--;

}
//num[i]='\0';
int t = num[17-k];
return t;
}
//将 time_t转换为TCHAR
TCHAR* FormatTime(time_t time1)
{
	struct tm tm1;
	TCHAR *szTime = new TCHAR[MAX_KEY_LENGTH];

	#ifdef WIN32
		tm1 = *localtime(&time1);
	#else
		localtime_r(&time1, &tm1 );
	#endif
		_stprintf( szTime, _T("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d"),
		tm1.tm_year+1900,tm1.tm_mon+1,  tm1.tm_mday,
		tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
	//	delete[] szTime;
		return szTime;
}


typedef struct GETACCOUNT
{
	TCHAR *name[MAX_KEY_LENGTH];						//用户名
	TCHAR *pass_expried_time[MAX_KEY_LENGTH];	//密码过期时间
	TCHAR *status[MAX_KEY_LENGTH];						//用户状态
	TCHAR *pass_change[MAX_KEY_LENGTH];			//密码是否可更改
	TCHAR *lasttimelogin[MAX_KEY_LENGTH];			//上次登陆时间
	DWORD  user_priv[MAX_KEY_LENGTH];			//用户权限，0=来宾
	TCHAR *commnet[MAX_KEY_LENGTH];			//用户描述
	int  totalcount;
}account;
GETACCOUNT GetAllUser()
{
		GETACCOUNT account;
		LPUSER_INFO_1 pBuf = NULL;
		LPUSER_INFO_1 pTmpBuf;

		DWORD dwLevel = 1;
		DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
		DWORD dwEntriesRead = 0;
		DWORD dwTotalEntries = 0;
		DWORD dwResumeHandle = 0;
		DWORD i;
		int dwTotalCount = 0;
		NET_API_STATUS nStatus;
		LPTSTR pszServerName = NULL;
		

		do
		{
		   nStatus = NetUserEnum(NULL,
			dwLevel,
			FILTER_NORMAL_ACCOUNT, // global users
			(LPBYTE*)&pBuf,
			dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);

		   //
		   // If the call succeeds,
		   //
		   if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
		   {
			if ((pTmpBuf = pBuf) != NULL)
			{
			//
			// Loop through the entries.
			//
			 for (i = 0; (i < dwEntriesRead); i++)
			 {
			  assert(pTmpBuf != NULL);

			  if (pTmpBuf == NULL)
			  {
			   fprintf(stderr, "An access violation has occurred\n");
			   break;
			  }
			  //
			  // Print the name of the user account.
			  //
		   /*
		   typedef struct _USER_INFO_1 {
			 LPWSTR    usri1_name;   //用户名
			 LPWSTR    usri1_password; //用户密码
			 DWORD     usri1_password_age; //当前密码使用时间(单位:秒)
			 DWORD     usri1_priv;   //用户权限 0 来宾; 1 普通用户; 2 管理员.
			 LPWSTR    usri1_home_dir; //
			 LPWSTR    usri1_comment; //备注,可能为NULL
			 DWORD     usri1_flags;   //
			 LPWSTR    usri1_script_path; //返回Unicode string,包含用户登录的脚本。可以为NULL
			}USER_INFO_1, *PUSER_INFO_1, *LPUSER_INFO_1;

		   */
		   //wprintf(L"Account No.%d\n",dwTotalCount+1);

		   LPUSER_INFO_3 bufptr=NULL;
		   LPUSER_INFO_2 buf=NULL;
		   NET_API_STATUS t3,t2;
		   t3=NetUserGetInfo(0,pTmpBuf->usri1_name,3,(LPBYTE*)&bufptr);
		   t2=NetUserGetInfo(0,pTmpBuf->usri1_name,2,(LPBYTE*)&buf);
		   //帐户名称:
		   //wprintf(L"\t Account Name: %s \n",bufptr->usri3_name);
		   TCHAR *str = new TCHAR[MAX_KEY_LENGTH];
   			_stprintf(str,_T("%s"),bufptr->usri3_name);
			account.name[dwTotalCount] = str;
		//	delete[] str;
			//帐户权限
			//unsigned long temp_priv = new unsigned long ;
			//	temp_priv = bufptr->usri3_priv;//
			DWORD str_priv;
			str_priv = bufptr->usri3_priv;
			//k = bufptr->usri3_priv;
			account.user_priv[dwTotalCount] = str_priv;

			//用户描述
			
			TCHAR *str_comment = new TCHAR[MAX_KEY_LENGTH];
			_stprintf(str_comment,_T("%s"),bufptr->usri3_comment);
			account.commnet[dwTotalCount] = str_comment;
			//delete[] str_comment;

			
		   //上次登录时间:单位,秒;计算起始时间为1970年1月1日00:00
		   if(bufptr->usri3_last_logon==0)
			//cout<<"\t Account Last Logon Time: "<<"上次登录时间未知"<<endl;
			account.lasttimelogin[dwTotalCount] = _T("未知");
		   else
		   {
			long m=bufptr->usri3_last_logon;
			time_t last;
			last = m;
			//cout<<"\t Account Last Logon Time: "<<ctime(&last);
		
			//将ctime类型转换为tchar类型
	//		time_temp = TimeToString(last);
	//	 	_stprintf(time_temp ,_T("%s"),TimeToString(last));
			TCHAR *time_temp = new TCHAR[MAX_KEY_LENGTH];
				time_temp = FormatTime(last);
			
			account.lasttimelogin[dwTotalCount] = time_temp;
		//	delete[] time_temp;
		   }

		   //密码过期时间:单位,秒;计算起始时间为1970年1月1日00:00. 如果为 0 ,表示永不过期
		   if(bufptr->usri3_password_expired==0||bufptr->usri3_password_expired==NULL)
			//cout<<"\t Password Expiried Time: "<<"密码永不过期"<<endl;

			account.pass_expried_time[dwTotalCount] = _T("永不过期");
		   else
		   {
			long n=bufptr->usri3_password_expired;
			time_t pwd;
			pwd = n;
			//cout<<"\t Password Expiried Time: "<<ctime(&pwd)<<endl;
			TCHAR *time_temp2 = new TCHAR[MAX_KEY_LENGTH];
			//将ctime类型转换为tchar类型
		//	time_temp2 = TimeToString(pwd);
		//	_stprintf(time_temp2,_T("%s"),TimeToString(pwd));
			time_temp2 = FormatTime(pwd);
			account.pass_expried_time[dwTotalCount] = time_temp2;
			//delete[] time_temp2;
		   }

		   //判断账户过期时间:如果过期时间为:65536*65536-1,表示永不过期
		/* if(buf->usri2_acct_expires==4294967295)
			cout<<"\t Account Expiried Time:"<<"帐户永不过期"<<endl;
		   else
		   {
			long k=buf->usri2_acct_expires;
			time_t acct;
			acct = k;
			cout<<"\t Account Expiried Time:"<<ctime(&acct)<<endl;
		   }
		   */
		   //判断帐户状态 usri2_flags:
		   /*
		   value       mean     int   
		   UF_SCRIPT      登录脚本执行   1   
		   UF_ACCOUNTDISABLE    用户帐户不可用   2
		      
		   UF_HOMEDIR_REQUIRED    要求有用户目录   8
		   UF_LOCKOUT      帐户锁定    16
		   UF_PASSWD_NOTREQD    不要求用户密码   32
		   UF_PASSWD_CANT_CHANGE   不能更改密码   64

		   UF_DONT_EXPIRE_PASSWD   密码不过期    65536

		   以下为帐户类型:
		   UF_TEMP_DUPLICATE_ACCOUNT 域用户帐户    256
		   UF_NORMAL_ACCOUNT    普通帐户    512

		   UF_INTERDOMAIN_TRUST_ACCOUNT域内可信任帐户   2048
		   UF_WORKSTATION_TRUST_ACCOUNT工作组可信任帐户 4096
		   UF_SERVER_TRUST_ACCOUNT   备份域控制器帐户 8192
		  
		   */
		  unsigned long n=buf->usri2_flags;
		   //帐户状态:

		   if(ToBinary(n,2))
		   // cout<<"\t Account Status : "<<"帐户未启用"<<endl;
			account.status[dwTotalCount] = _T("已禁用");
		   else
		   // cout<<"\t Account Status : "<<"帐户已启用"<<endl;
			account.status[dwTotalCount] = _T("已启用");
		   //密码是否可更改:
		   if(ToBinary(n,7))
			//cout<<"\t Password Change : "<<"密码不能更改"<<endl;
			account.pass_change[dwTotalCount] = _T("不可更改");
		   else
			//cout<<"\t Password Change : "<<"密码可以更改"<<endl;
			account.pass_change[dwTotalCount] = _T("可更改");
			  pTmpBuf++;
			  dwTotalCount++;
			 }
			}
		   }
		   else{
			fprintf(stderr, "A system error has occurred: %d\n", nStatus);
		   }
		   if (pBuf != NULL)
		   {
			NetApiBufferFree(pBuf);
			pBuf = NULL;
		   }
		}while(nStatus == ERROR_MORE_DATA);

		//fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);
			account.totalcount = dwTotalCount;

		return account;
}


