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

//�ж��ʻ�״̬:�������ֵ��Ӧ����������
/*

��� Value              Mean                                          Calendar Value
01   UF_SCRIPT      ��¼�ű�ִ��                                      1   
02   UF_ACCOUNTDISABLE    �û��ʻ�������                   2
03      
04   UF_HOMEDIR_REQUIRED    Ҫ�����û�Ŀ¼              8
02   UF_LOCKOUT      �ʻ�����                                          16
06   UF_PASSWD_NOTREQD    ��Ҫ���û�����                32
07   UF_PASSWD_CANT_CHANGE   ���ܸ�������            64

08

   ����Ϊ�ʻ�����:
09   UF_TEMP_DUPLICATE_ACCOUNT ���û��ʻ�           256
10   UF_NORMAL_ACCOUNT    ��ͨ�ʻ�                            512
11
12   UF_INTERDOMAIN_TRUST_ACCOUNT���ڿ������ʻ�   2048
13   UF_WORKSTATION_TRUST_ACCOUNT������������ʻ� 4096
14   UF_SERVER_TRUST_ACCOUNT   ������������ʻ� 8192
15
16 
17   UF_DONT_EXPIRE_PASSWD   ���벻����                  65536

˵��:��ToBinary(int,int)��������жϸ�����Ŷ�Ӧ��ֵ�Ƿ�Ϊ�档
*/
int ToBinary(unsigned long n,int k)
{
int num[17];
int i=16;
//������������
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
//�� time_tת��ΪTCHAR
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
	TCHAR *name[MAX_KEY_LENGTH];						//�û���
	TCHAR *pass_expried_time[MAX_KEY_LENGTH];	//�������ʱ��
	TCHAR *status[MAX_KEY_LENGTH];						//�û�״̬
	TCHAR *pass_change[MAX_KEY_LENGTH];			//�����Ƿ�ɸ���
	TCHAR *lasttimelogin[MAX_KEY_LENGTH];			//�ϴε�½ʱ��
	DWORD  user_priv[MAX_KEY_LENGTH];			//�û�Ȩ�ޣ�0=����
	TCHAR *commnet[MAX_KEY_LENGTH];			//�û�����
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
			 LPWSTR    usri1_name;   //�û���
			 LPWSTR    usri1_password; //�û�����
			 DWORD     usri1_password_age; //��ǰ����ʹ��ʱ��(��λ:��)
			 DWORD     usri1_priv;   //�û�Ȩ�� 0 ����; 1 ��ͨ�û�; 2 ����Ա.
			 LPWSTR    usri1_home_dir; //
			 LPWSTR    usri1_comment; //��ע,����ΪNULL
			 DWORD     usri1_flags;   //
			 LPWSTR    usri1_script_path; //����Unicode string,�����û���¼�Ľű�������ΪNULL
			}USER_INFO_1, *PUSER_INFO_1, *LPUSER_INFO_1;

		   */
		   //wprintf(L"Account No.%d\n",dwTotalCount+1);

		   LPUSER_INFO_3 bufptr=NULL;
		   LPUSER_INFO_2 buf=NULL;
		   NET_API_STATUS t3,t2;
		   t3=NetUserGetInfo(0,pTmpBuf->usri1_name,3,(LPBYTE*)&bufptr);
		   t2=NetUserGetInfo(0,pTmpBuf->usri1_name,2,(LPBYTE*)&buf);
		   //�ʻ�����:
		   //wprintf(L"\t Account Name: %s \n",bufptr->usri3_name);
		   TCHAR *str = new TCHAR[MAX_KEY_LENGTH];
   			_stprintf(str,_T("%s"),bufptr->usri3_name);
			account.name[dwTotalCount] = str;
		//	delete[] str;
			//�ʻ�Ȩ��
			//unsigned long temp_priv = new unsigned long ;
			//	temp_priv = bufptr->usri3_priv;//
			DWORD str_priv;
			str_priv = bufptr->usri3_priv;
			//k = bufptr->usri3_priv;
			account.user_priv[dwTotalCount] = str_priv;

			//�û�����
			
			TCHAR *str_comment = new TCHAR[MAX_KEY_LENGTH];
			_stprintf(str_comment,_T("%s"),bufptr->usri3_comment);
			account.commnet[dwTotalCount] = str_comment;
			//delete[] str_comment;

			
		   //�ϴε�¼ʱ��:��λ,��;������ʼʱ��Ϊ1970��1��1��00:00
		   if(bufptr->usri3_last_logon==0)
			//cout<<"\t Account Last Logon Time: "<<"�ϴε�¼ʱ��δ֪"<<endl;
			account.lasttimelogin[dwTotalCount] = _T("δ֪");
		   else
		   {
			long m=bufptr->usri3_last_logon;
			time_t last;
			last = m;
			//cout<<"\t Account Last Logon Time: "<<ctime(&last);
		
			//��ctime����ת��Ϊtchar����
	//		time_temp = TimeToString(last);
	//	 	_stprintf(time_temp ,_T("%s"),TimeToString(last));
			TCHAR *time_temp = new TCHAR[MAX_KEY_LENGTH];
				time_temp = FormatTime(last);
			
			account.lasttimelogin[dwTotalCount] = time_temp;
		//	delete[] time_temp;
		   }

		   //�������ʱ��:��λ,��;������ʼʱ��Ϊ1970��1��1��00:00. ���Ϊ 0 ,��ʾ��������
		   if(bufptr->usri3_password_expired==0||bufptr->usri3_password_expired==NULL)
			//cout<<"\t Password Expiried Time: "<<"������������"<<endl;

			account.pass_expried_time[dwTotalCount] = _T("��������");
		   else
		   {
			long n=bufptr->usri3_password_expired;
			time_t pwd;
			pwd = n;
			//cout<<"\t Password Expiried Time: "<<ctime(&pwd)<<endl;
			TCHAR *time_temp2 = new TCHAR[MAX_KEY_LENGTH];
			//��ctime����ת��Ϊtchar����
		//	time_temp2 = TimeToString(pwd);
		//	_stprintf(time_temp2,_T("%s"),TimeToString(pwd));
			time_temp2 = FormatTime(pwd);
			account.pass_expried_time[dwTotalCount] = time_temp2;
			//delete[] time_temp2;
		   }

		   //�ж��˻�����ʱ��:�������ʱ��Ϊ:65536*65536-1,��ʾ��������
		/* if(buf->usri2_acct_expires==4294967295)
			cout<<"\t Account Expiried Time:"<<"�ʻ���������"<<endl;
		   else
		   {
			long k=buf->usri2_acct_expires;
			time_t acct;
			acct = k;
			cout<<"\t Account Expiried Time:"<<ctime(&acct)<<endl;
		   }
		   */
		   //�ж��ʻ�״̬ usri2_flags:
		   /*
		   value       mean     int   
		   UF_SCRIPT      ��¼�ű�ִ��   1   
		   UF_ACCOUNTDISABLE    �û��ʻ�������   2
		      
		   UF_HOMEDIR_REQUIRED    Ҫ�����û�Ŀ¼   8
		   UF_LOCKOUT      �ʻ�����    16
		   UF_PASSWD_NOTREQD    ��Ҫ���û�����   32
		   UF_PASSWD_CANT_CHANGE   ���ܸ�������   64

		   UF_DONT_EXPIRE_PASSWD   ���벻����    65536

		   ����Ϊ�ʻ�����:
		   UF_TEMP_DUPLICATE_ACCOUNT ���û��ʻ�    256
		   UF_NORMAL_ACCOUNT    ��ͨ�ʻ�    512

		   UF_INTERDOMAIN_TRUST_ACCOUNT���ڿ������ʻ�   2048
		   UF_WORKSTATION_TRUST_ACCOUNT������������ʻ� 4096
		   UF_SERVER_TRUST_ACCOUNT   ������������ʻ� 8192
		  
		   */
		  unsigned long n=buf->usri2_flags;
		   //�ʻ�״̬:

		   if(ToBinary(n,2))
		   // cout<<"\t Account Status : "<<"�ʻ�δ����"<<endl;
			account.status[dwTotalCount] = _T("�ѽ���");
		   else
		   // cout<<"\t Account Status : "<<"�ʻ�������"<<endl;
			account.status[dwTotalCount] = _T("������");
		   //�����Ƿ�ɸ���:
		   if(ToBinary(n,7))
			//cout<<"\t Password Change : "<<"���벻�ܸ���"<<endl;
			account.pass_change[dwTotalCount] = _T("���ɸ���");
		   else
			//cout<<"\t Password Change : "<<"������Ը���"<<endl;
			account.pass_change[dwTotalCount] = _T("�ɸ���");
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


