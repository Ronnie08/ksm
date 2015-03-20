#include "stdafx.h"
#include "beikesafe.h"
#include <fstream>
#include "beikesafemaindlg.h"
#include "Vulfix\BeikeVulfixEngine.h"
#include "Vulfix\BeikeVulfixHandler.h"
#include "beikesafeexamuihandler.h"
#include "Checkbase\GetSystemNames.h"
#include "Checkbase\GetShareList.h"
#include "Checkbase\GetRunList.h"
#include "Checkbase\GetSoftStalList.h"
#include "Checkbase\GetSystemService.h"
#include "Checkbase\GetComputerInfo.h"
#include "Checkbase\GetSharePath.h"
#include "Checkbase\GetSystemAccount.h"
#include "Checkbase\GetGuestStatus.h"
#include "Checkbase\KXmlWriter.h"
#include "wtlhelper/whwindow.h"
#include "Checkbase\ToSendMail.h"
#define		KEY_SET1    __T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run" )
#define		KEY_SET2   _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce" )
#define 	KEY_SET3   _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx" )
#define 	KEY_SET4    _T ("Software\\Microsoft\\Windows\\CurrentVersion\\Run" )
#define		KEY_SET5    _T( "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce" )

using namespace std;

SHARE Check_Share_Authority_Key();
ACCOUNT Change_SAM_Access();
RUNLIST RunList(LPCWSTR key_set);
SOFTLIST Soft_Stall();
SERVICELIST servicelist();
COMPUTERINFO computerinfo();
SHAREPATH GetSharePath();
GETACCOUNT GetAllUser();
int Guset_Check();
int Change_SAM_Access2();
//#include "beikesafefreevirusuihandler.h"
#include <Psapi.h>



#pragma comment(lib, "psapi.lib")

extern CVulEngine *theEngine;
BOOL	g_bkMsgBox = FALSE;

#define TIMER_ID_MAIN_DLG_ONCE 0x10000

#define MSG_BEGIN						WM_APP+0x100
#define MSG_APP_DELAY_EXAM				MSG_BEGIN+1
#define MSG_APP_DELAY_NAVIGATE_IE		MSG_BEGIN+2

//////////////////////////////////////////////////////////////////////////

/// 2010-10-9 修改消息宏的定义到CPP，以免编译的时候太慢

BEGIN_MSG_MAP_EX_IMP(CBeikeSafeMainDlg)
	MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
	CHAIN_MSG_MAP(CBkDialogImpl<CBeikeSafeMainDlg>)
	CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CBeikeSafeMainDlg>)

	MSG_WM_TIMER(OnTimer)
	CHAIN_MSG_MAP_MEMBER((*m_ExamHandler))
	CHAIN_MSG_MAP_MEMBER(m_viewVulfix.m_viewSoftVul)
	//CHAIN_MSG_MAP_MEMBER((*m_pFreeVirusHandle))		
	MSG_WM_INITDIALOG(OnInitDialog)
	MSG_WM_SYSCOMMAND(OnSysCommand)
	MSG_WM_DESTROY(OnDestroy)
	MESSAGE_HANDLER_EX(MSG_APP_DELAY_NAVIGATE_IE, OnAppDelayNavigateIE)

	REFLECT_NOTIFICATIONS_EX()
END_MSG_MAP_IMP()
//////////////////////////////////////////////////////////////////////////

CBeikeSafeMainDlg::CBeikeSafeMainDlg()
	: CBkDialogImpl<CBeikeSafeMainDlg>(IDR_BK_MAIN_DIALOG)  //IDR_BK_CHECK_DIALOG  IDR_BK_MAIN_DIALOG)
	, m_bPage0NeverShowed(TRUE)
	, m_bPage1NeverShowed(TRUE)
	, m_bPage5NeverShowed(TRUE)
	, m_bVulfixInited(FALSE)
	, m_bVulfixRescanRequired(FALSE)
	, m_bFirstPageChange(TRUE)
	, m_bIeFixInit(FALSE)
	, m_bKClearInit(FALSE)
{
	m_ExamHandler			= new CBeikeSafeExamUIHandler(this);
	//m_pFreeVirusHandle		= new CBeikeSafeFreeVirusUIHandler(this);
	AddUIHandler(&m_viewVulfix.m_viewSoftVul);
 	AddUIHandler(m_ExamHandler);
}

CBeikeSafeMainDlg::~CBeikeSafeMainDlg()
{
	delete m_ExamHandler;
	//delete m_pFreeVirusHandle;
}

LRESULT CBeikeSafeMainDlg::OnTimer( UINT_PTR uIdEvent )
{

	SetMsgHandled( FALSE );
	return TRUE;
}

void CBeikeSafeMainDlg::SetDefaultNavigate(LPCWSTR lpszNavigate)
{
	//m_strDefaultNavigate = lpszNavigate;
}

LRESULT CBeikeSafeMainDlg::OnAppDefaultNavigate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
// 	_Module.Navigate(m_strDefaultNavigate);
// 
// 	m_strDefaultNavigate.Empty();

	return 0;
}


LRESULT CBeikeSafeMainDlg::OnAppEchoFinish(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);


	return 0;
}
/*
//打开另存为窗口，发送邮件
TCHAR *GetFileName()
{
	// 过滤器
	HWND m_hWnd;
	TCHAR szFilter[] = _T(	"网页文件(*.html)\0*.html\0\0"	);
	CFileDialog FileDlg(FALSE, 0, 0, 
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_OVERWRITEPROMPT, 
		szFilter, m_hWnd); 
	// 初始化文件名，保存路径
	TCHAR szDefFile[MAX_PATH] = _T("");
	_tcscpy(szDefFile, BkString::Get(140));
	//用IP地址做默认文件名
	COMPUTERINFO filenamestr;
	filenamestr = computerinfo();
	int  ssn = 0;
	wchar_t *ip = new wchar_t[MAX_KEY_LENGTH];
	ssn = MultiByteToWideChar(CP_ACP , 0 , filenamestr.computerip , -1 , ip , 0);
	MultiByteToWideChar(CP_ACP , 0 , filenamestr.computerip , -1 , ip , ssn);
	_stprintf(szDefFile, _T("%s_%s"),szDefFile,ip);

	TCHAR szDefFilePath[MAX_PATH] = _T("");
	if(SHGetSpecialFolderPath(NULL, szDefFilePath, CSIDL_DESKTOPDIRECTORY, FALSE))
		FileDlg.m_ofn.lpstrInitialDir = szDefFilePath;
	TCHAR szDefExt[MAX_PATH] = _T("txt");
	FileDlg.m_ofn.lpstrFile = szDefFile;
	FileDlg.m_ofn.lpstrDefExt = szDefExt;
	FileDlg.m_ofn.hwndOwner = m_hWnd;
	FileDlg.m_ofn.nFilterIndex = 1;
	INT_PTR nRet = FileDlg.DoModal(m_hWnd);
	if (IDOK != nRet)
		return NULL ;
	return szDefFile;

}
*/


//初始化漏洞扫描窗口
void CBeikeSafeMainDlg::InitVulFix()
{
	m_viewVulfix.Create(GetViewHWND(), NULL, WS_CHILD|WS_CLIPCHILDREN, 0, 3000);
	ATLVERIFY( m_viewVulfix.Load(IDR_BK_VULDLG_MAIN ) ); //IDR_BK_CHECK_DIALOG  IDR_BK_VULDLG_MAIN
	m_viewVulfix.m_viewSoftVul.SetMainDlg( this );
	m_viewVulfix.Init(m_hWnd);
}


//0、初始化首页窗口
void CBeikeSafeMainDlg::InitCountCheck()
{	

	 m_wndCountListView.Create( 
		GetViewHWND(), NULL,NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 600, NULL);
	
		INT nRow = 0;
		//账号列
		m_wndCountListView.InsertColumn(nRow++, BkString::Get(10011), LVCFMT_CENTER, 56);
		m_wndCountListView.InsertColumn(nRow++, BkString::Get(10025), LVCFMT_CENTER, 200);
		m_wndCountListView.InsertColumn(nRow++, BkString::Get(10026), LVCFMT_CENTER, 200);
		m_wndCountListView.InsertColumn(nRow++, BkString::Get(10021), LVCFMT_CENTER, 300);

	m_wndCountListView.SetObserverWindow(m_hWnd);	
}
/*
//定义一个字符串匹配函数
int stringcmp(TCHAR *str)
{

	TCHAR *soft_name = _T("Chinasec");

	int name_size = wcslen(str);
	TCHAR *name_temp = new TCHAR;
	//获取str的前9位，将其赋值给name_temp
	int mstb =0;
	if(name_size >9)
	{
		for(int j=0;j<name_size;j++)

		{
			//_stprintf(name_temp[j],_T("%s"),str[j]);
			name_temp[j] = str[j];
		}
		if(_tcscmp(soft_name,name_temp))
			return 1;
		else return 0;
	}
	else return 0;
}
//检查桌面安全是否安装
int checkchinasec()
{
	SOFTLIST softlist;
	int status = 0;
	softlist = Soft_Stall();
	//1为已安装，0为未安装

	//for(int i=0;i<softlist.count;i++)
	while(softlist.count)
	{
		softlist.count--;
		TCHAR *p = softlist.softname[softlist.count];

		int chinasec_status = stringcmp(p);
		if(chinasec_status == 1)
		{
			status = chinasec_status;
			break;
		}
		
	}
	return status;
}

//检查Telnet、Remote Register是否开启

int *KeyServiceCheck()
{
	TCHAR *servicename[2] = {_T("Telnet"),_T("Remote Registry")};
	int *result=new int[2];
	SERVICELIST ser_list = servicelist();
	TCHAR *name_temp = new TCHAR;
	for(int i=0;i<ser_list.count;i++)
	{
		name_temp = ser_list.servicename[i];
		
		if(_tcscmp(servicename[0],name_temp))
			result[0] = ser_list.servicestatus[i];
		if(_tcscmp(servicename[1],name_temp))
			result[1] = ser_list.servicestatus[i];
		
	}
	return result;
}

*/
//0、显示计算机基本信息
void CBeikeSafeMainDlg::CountCheckShow()

{
	COMPUTERINFO info = computerinfo();

	//wchar_t computer[]
	
	wchar_t *computername = new wchar_t[MAX_KEY_LENGTH];
	wchar_t *computerip = new wchar_t[MAX_KEY_LENGTH];

	int k = 0;
	k = MultiByteToWideChar(CP_ACP , 0 , info.computername , -1 , computername ,0);
	 MultiByteToWideChar(CP_ACP , 0 , info.computername , -1 , computername ,k);
	int j = 0;
	
	j = MultiByteToWideChar(CP_ACP , 0 , info.computerip , -1 , computerip , 0);
	 MultiByteToWideChar(CP_ACP , 0 , info.computerip , -1 , computerip , j);
	
	 int nItem1 = m_wndCountListView.Append(_T("1"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem1, _T("计算机名："));
	m_wndCountListView.AppendSubItem(nItem1, ( LPCTSTR )computername);
		m_wndCountListView.SetCheckState(nItem1, TRUE);
	
	int nItem2 = m_wndCountListView.Append(_T("2"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem2, _T("计算机IP："));
	m_wndCountListView.AppendSubItem(nItem2, ( LPCTSTR )computerip);
		m_wndCountListView.SetCheckState(nItem2, TRUE);
	
	int nItem3 = m_wndCountListView.Append(_T("3"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem3, _T("域名："));
	if(wcslen(info.domain) !=0)
		m_wndCountListView.AppendSubItem(nItem3, ( LPCTSTR )info.domain);
	else
		m_wndCountListView.AppendSubItem(nItem3, _T("未加入任何域"));
	m_wndCountListView.SetCheckState(nItem3, TRUE);

	int nItem4 = m_wndCountListView.Append(_T("4"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem4, _T("账号检查："));
	int gueststatus = Change_SAM_Access2();
	if (gueststatus == 0)	//已禁用
		m_wndCountListView.AppendSubItem(nItem4, _T("OK"));
	else if(gueststatus == 1)//未禁用
		m_wndCountListView.AppendSubItem(nItem4, _T("Guest未禁用"));
	else
		m_wndCountListView.AppendSubItem(nItem4, _T("Wrong"));
	
	m_wndCountListView.AppendSubItem(nItem4, _T("guest需禁用"));
	m_wndCountListView.SetCheckState(nItem4, TRUE);


	//显示系统个版本信息
	int nItem9 = m_wndCountListView.Append(_T("5"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem9, _T("操作系统："));
	m_wndCountListView.AppendSubItem(nItem9, ( LPCTSTR )info.osversion);
	m_wndCountListView.SetCheckState(nItem9, TRUE);

	int nItem5 = m_wndCountListView.Append(_T("6"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem5, _T("共享检查："));
	SHARE sharetemp = Check_Share_Authority_Key();
	if(sharetemp.anymous != 0 )

		m_wndCountListView.AppendSubItem(nItem5, _T("存在匿名共享"));
	
	else
		m_wndCountListView.AppendSubItem(nItem5, _T("OK"));
	
	m_wndCountListView.SetCheckState(nItem5, TRUE);
/*
	int nItem6 = m_wndCountListView.Append(_T("7"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem6, _T("安全软件检查："));
	// int chinasecstatus = checkchinasec();
	 //if(chinasecstatus == 1)

		m_wndCountListView.AppendSubItem(nItem6, _T("OK"));
	 //else
	//	m_wndCountListView.AppendSubItem(nItem6, _T("未安装桌面安全软件"));
	m_wndCountListView.AppendSubItem(nItem6, _T("必装软件：桌面安全、防毒软件，可选：联软软件"));
	m_wndCountListView.SetCheckState(nItem6, TRUE);

	int nItem7 = m_wndCountListView.Append(_T("8"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem7, _T("系统补丁检查:"));
	m_wndCountListView.AppendSubItem(nItem7, _T(""));
	m_wndCountListView.SetCheckState(nItem7, TRUE);

	int nItem8 = m_wndCountListView.Append(_T("9"), 0, SUBITEM_TEXT);
	m_wndCountListView.AppendSubItem(nItem8, _T("其他安全检查："));

	int *servicecheck = KeyServiceCheck();

	if( servicecheck[1] ==1 )
		m_wndCountListView.AppendSubItem(nItem8, _T("Remote Register已开启"));
	else
		m_wndCountListView.AppendSubItem(nItem8, _T("OK"));
	m_wndCountListView.AppendSubItem(nItem8, _T("关键服务、启动项等"));
	m_wndCountListView.SetCheckState(nItem8, TRUE);
*/	
}
//1、初始化基本检查系统账号窗口
void CBeikeSafeMainDlg::InitAccountCheck()
{	

		m_wndAccountListView.Create( 
		GetViewHWND(), NULL,NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 2001, NULL);
	{
		INT nRow = 0;
		//账号列
		m_wndAccountListView.InsertColumn(nRow++, BkString::Get(10011), LVCFMT_CENTER, 56);
		m_wndAccountListView.InsertColumn(nRow++, BkString::Get(10012), LVCFMT_CENTER, 130);
		m_wndAccountListView.InsertColumn(nRow++, BkString::Get(10031), LVCFMT_CENTER, 160);
		m_wndAccountListView.InsertColumn(nRow++, BkString::Get(10013), LVCFMT_CENTER, 80);
		m_wndAccountListView.InsertColumn(nRow++, BkString::Get(10028), LVCFMT_CENTER, 150);

		m_wndAccountListView.InsertColumn(nRow++, BkString::Get(10030), LVCFMT_CENTER, 100);
		m_wndAccountListView.InsertColumn(nRow++, BkString::Get(10029), LVCFMT_CENTER, 100);
}
	
	m_wndAccountListView.SetObserverWindow(m_hWnd);	
}
//1、显示账号列表
void CBeikeSafeMainDlg::AccountCheckShow()

{

	//通过读取注册表的方式
/*	ACCOUNT acc;
	acc=Change_SAM_Access();
	CString number;

	for( int i=0; i<acc.count; i++)
	{	

		number.Format(TEXT("%d"),i+1);		//成功将int转换为LPCTSTR
	
	
		
		int nItem = m_wndAccountListView.Append(number,  0, SUBITEM_TEXT);

		m_wndAccountListView.AppendSubItem(nItem, (LPCTSTR)acc.systemaccount[i]);

		m_wndAccountListView.SetCheckState(nItem, TRUE);
	}

	// MessageBox((LPCTSTR)account, _T("垃圾清理"), MB_ICONINFORMATION);		

	*/

	//通过系统API的方式
	GETACCOUNT account;
	account = GetAllUser();
	CString number;
	
	for(int i=0; i<account.totalcount; i++)
	{
	number.Format(TEXT("%d"),i+1);		//成功将int转换为LPCTSTR
	int nItem = m_wndAccountListView.Append(number,  0, SUBITEM_TEXT);

	m_wndAccountListView.AppendSubItem(nItem, (LPCTSTR)account.name[i]);
	m_wndAccountListView.AppendSubItem(nItem, (LPCTSTR)account.commnet[i]);
/*

	if( account.user_priv[i] == 0)
			m_wndAccountListView.AppendSubItem(nItem, _T("来宾用户"));
	else if(account.user_priv[i] == 1)
			m_wndAccountListView.AppendSubItem(nItem, _T("普通用户"));
	else if(account.user_priv[i] == 2)
			m_wndAccountListView.AppendSubItem(nItem, _T("管理员"));
	else
			m_wndAccountListView.AppendSubItem(nItem, _T("Wrong"));

*/
	//	int k = (int)account.user_priv[i];方式等同于if else
//	switch(account.user_priv[i])
//	{
//
//	
//	case 0:
//			m_wndAccountListView.AppendSubItem(nItem, _T("来宾用户"));
//			break;
//	case 1:
//			m_wndAccountListView.AppendSubItem(nItem, _T("普通用户"));
//			break;
//	case 2:
//			m_wndAccountListView.AppendSubItem(nItem, _T("管理员"));
//			break;
//	default:
//			m_wndAccountListView.AppendSubItem(nItem, _T("Wrong"));
//			break;
//		
//	}


	m_wndAccountListView.AppendSubItem(nItem, (LPCTSTR)account.status[i]);
	m_wndAccountListView.AppendSubItem(nItem, (LPCTSTR)account.lasttimelogin[i]);

	m_wndAccountListView.AppendSubItem(nItem, (LPCTSTR)account.pass_change[i]);
	m_wndAccountListView.AppendSubItem(nItem, (LPCTSTR)account.pass_expried_time[i]);
	m_wndAccountListView.SetCheckState(nItem, TRUE);

	}

}
//2、初始化基本检查共享检查窗口
void CBeikeSafeMainDlg::InitShareCheck()
{	

		m_wndShaerListView.Create( 
		GetViewHWND(), NULL,NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 2010, NULL);
	{
		INT nRow = 0;
		//账号列
		m_wndShaerListView.InsertColumn(nRow++, BkString::Get(10011), LVCFMT_CENTER, 56);
		m_wndShaerListView.InsertColumn(nRow++, BkString::Get(10014), LVCFMT_CENTER, 165);
		m_wndShaerListView.InsertColumn(nRow++, BkString::Get(10015), LVCFMT_CENTER, 250);
		m_wndShaerListView.InsertColumn(nRow++, BkString::Get(10016), LVCFMT_CENTER, 100);
}
	
	m_wndShaerListView.SetObserverWindow(m_hWnd);	
}
//2、显示共享列表
void CBeikeSafeMainDlg::ShareCheckShow()

{
	SHARE share;
	SHAREPATH sharepath;
	share= Check_Share_Authority_Key();
	sharepath = GetSharePath();
	CString number;

	for(int i=0;i<share.count;i++)
	{	
		number.Format(TEXT("%d"),i+1);		//成功将int转换为LPCTSTR
		//char *p=acc.a[i];
	
	
		int nItem = m_wndShaerListView.Append(number,  0, SUBITEM_TEXT);
		
		m_wndShaerListView.AppendSubItem(nItem, (LPCTSTR)share.sharename[i]);
		m_wndShaerListView.AppendSubItem(nItem, (LPCTSTR)sharepath.path[i]);
		if(share.Status[i] == 1)
		{
			//m_wndShaerListView.SetSubItemColor(nItem, 3, 0XFF0033, 0);
			m_wndShaerListView.AppendSubItem(nItem, _T("是"));
		}
		else
			m_wndShaerListView.AppendSubItem(nItem, _T("否"));
		m_wndShaerListView.SetCheckState(nItem, TRUE);
		
	}
	// MessageBox((LPCTSTR)account, _T("垃圾清理"), MB_ICONINFORMATION);		
}

//3、初始化基本检查启动项窗口
void CBeikeSafeMainDlg::InitRunListCheck()
{	

		m_wndRunListView.Create( 
		GetViewHWND(), NULL,NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 2002, NULL);
	{
		INT nRow = 0;
		//账号列
		m_wndRunListView.InsertColumn(nRow++, BkString::Get(10011), LVCFMT_CENTER, 56);
		m_wndRunListView.InsertColumn(nRow++, BkString::Get(10017), LVCFMT_CENTER, 165);
		m_wndRunListView.InsertColumn(nRow++, BkString::Get(10018), LVCFMT_CENTER, 300);
		//m_wndRunListView.InsertColumn(nRow++, BkString::Get(10016), LVCFMT_CENTER, 100);
}
	
		m_wndRunListView.SetObserverWindow(m_hWnd);	
	
}

//3、显示启动项列表
void CBeikeSafeMainDlg::RunListCheckShow()

{


	//RUNLIST runlist1,runlist2,runlist3,runlist4,runlist5;
	RUNLIST runlist1, runlist4;
	runlist1 = RunList(   KEY_SET1 ) ;
//	runlist2 = RunList(   KEY_SET2 ) ;
//	runlist3 = RunList(   KEY_SET3  ) ;
	runlist4 = RunList1(  KEY_SET4 ) ;
//	runlist5 = RunList1(  KEY_SET5) ;
	CString number;
	int listnumber=1;

	for(int i=0;i<runlist1.count;i++)
	{	

		number.Format(TEXT("%d"),listnumber);		//成功将int转换为LPCTSTR
		//char *p=acc.a[i];
	
	
		int nItem = m_wndRunListView.Append(number,  0, SUBITEM_TEXT);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist1.a[i]);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist1.b[i]);
		m_wndRunListView.SetCheckState(nItem, TRUE);
		listnumber++;
	}
/*	
	for(int i=0;i<runlist2.count;i++)
	{	
		number.Format(TEXT("%d"),listnumber);		
	
		int nItem = m_wndRunListView.Append(number,  0, SUBITEM_TEXT);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist2.a[i]);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist2.b[i]);
		m_wndRunListView.SetCheckState(nItem, TRUE);
		listnumber++;
	}
		for(int i=0;i<runlist3.count;i++)
	{	
		number.Format(TEXT("%d"),listnumber);		
		//number.Format(TEXT("%d"),runlist2.count +i+1);
		int nItem = m_wndRunListView.Append(number,  0, SUBITEM_TEXT);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist3.a[i]);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist3.b[i]);
		m_wndRunListView.SetCheckState(nItem, TRUE);
		listnumber++;
	}
	*/
		for(int i=0;i<runlist4.count;i++)
	{	
		number.Format(TEXT("%d"),listnumber);	
		int nItem = m_wndRunListView.Append(number,  0, SUBITEM_TEXT);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist4.a[i]);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist4.b[i]);
		m_wndRunListView.SetCheckState(nItem, TRUE);
		listnumber++;
	}
/*
		for(int i=0;i<runlist5.count;i++)
	{	
		number.Format(TEXT("%d"),listnumber);	
		int nItem = m_wndRunListView.Append(number,  0, SUBITEM_TEXT);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist5.a[i]);
		m_wndRunListView.AppendSubItem(nItem, (LPCTSTR)runlist5.b[i]);
		m_wndRunListView.SetCheckState(nItem, TRUE);
		listnumber++;
	}
*/	
	//MessageBox(_T("MSDN"), _T("垃圾清理"), MB_ICONINFORMATION);		
}


//4、初始化基本检查安全软件安装窗口
void CBeikeSafeMainDlg::InitSoftListCheck()
{	

		m_wndSoftListView.Create( 
		GetViewHWND(), NULL,NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 2003, NULL);
	{
		INT nRow = 0;
		//账号列
		m_wndSoftListView.InsertColumn(nRow++, BkString::Get(10011), LVCFMT_CENTER, 56);
		m_wndSoftListView.InsertColumn(nRow++, BkString::Get(10019), LVCFMT_CENTER, 300);
		m_wndSoftListView.InsertColumn(nRow++, BkString::Get(10020), LVCFMT_CENTER, 165);
		m_wndSoftListView.InsertColumn(nRow++, BkString::Get(10021), LVCFMT_CENTER, 165);
		//m_wndRunListView.InsertColumn(nRow++, BkString::Get(10016), LVCFMT_CENTER, 100);
}
	
		m_wndSoftListView.SetObserverWindow(m_hWnd);	
}

//


//4、显示软件安装
void CBeikeSafeMainDlg::SoftListCheckShow()

{
	SOFTLIST softlist;
	softlist = Soft_Stall();
	CString number;


	
	for(int i=0;i<softlist.count;i++)
	{	
		number.Format(TEXT("%d"),i+1);	


		int nItem = m_wndSoftListView.Append(number,  0, SUBITEM_TEXT);
		
		m_wndSoftListView.AppendSubItem(nItem,(LPCTSTR)softlist.softname[i]);
		m_wndSoftListView.AppendSubItem(nItem,(LPCTSTR)softlist.softversion[i]);
		m_wndSoftListView.SetCheckState(nItem, TRUE);
		
		
	}
	
}



//5、初始化基本检查关键服务窗口
void CBeikeSafeMainDlg::InitKeyServicesCheck()
{	

		m_wndKeyServiceView.Create( 
		GetViewHWND(), NULL,NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 2004, NULL);
	{
		INT nRow = 0;
		//账号列
		m_wndKeyServiceView.InsertColumn(nRow++, BkString::Get(10011), LVCFMT_CENTER, 56);
		m_wndKeyServiceView.InsertColumn(nRow++, BkString::Get(10022), LVCFMT_CENTER, 300);
		m_wndKeyServiceView.InsertColumn(nRow++, BkString::Get(10023), LVCFMT_CENTER, 200);
		m_wndKeyServiceView.InsertColumn(nRow++, BkString::Get(10021), LVCFMT_CENTER, 100);
		//m_wndRunListView.InsertColumn(nRow++, BkString::Get(10016), LVCFMT_CENTER, 100);
}
	
		m_wndKeyServiceView.SetObserverWindow(m_hWnd);	
}

//5、显示关键服务是否关闭
void CBeikeSafeMainDlg::KeyServicesShow()

{
	SERVICELIST serlist= servicelist();

	for(int i=0;i<serlist.count;i++)
	{	

		CString number;
		number.Format( TEXT("%d" ),i+1);	

		int nItem = m_wndKeyServiceView.Append(number,  0, SUBITEM_TEXT);
		m_wndKeyServiceView.AppendSubItem(nItem, (LPCTSTR)serlist.servicename[i]);
		m_wndKeyServiceView.AppendSubItem(nItem, (LPCTSTR)serlist.servicepath[i]);
		if( serlist.servicestatus[i] == 0 )
			m_wndKeyServiceView.AppendSubItem( nItem, _T( "已停止" ) );
		else if( serlist.servicestatus[i] == 1 )
			m_wndKeyServiceView.AppendSubItem( nItem, _T( "已启动" ) );
		else 
			m_wndKeyServiceView.AppendSubItem( nItem, _T( "未知" ) );
		m_wndKeyServiceView.SetCheckState(nItem, TRUE);

	}
	// MessageBox((LPCTSTR)account, _T("垃圾清理"), MB_ICONINFORMATION);		
}

BOOL CBeikeSafeMainDlg::OnExportResult()
{
	TCHAR szFilter[] = _T(	"网页文件(*.html)\0*.html\0\0"	);
	CFileDialog FileDlg(FALSE, 0, 0, 
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_OVERWRITEPROMPT, 
		szFilter, m_hWnd); 
	// 初始化文件名，保存路径
	TCHAR szDefFile[MAX_PATH] = _T("");
	_tcscpy(szDefFile, BkString::Get(140));
	//用IP地址做默认文件名
	COMPUTERINFO filenamestr;
	filenamestr = computerinfo();
	int  ssn = 0;
	wchar_t *ip = new wchar_t[MAX_KEY_LENGTH];
	ssn = MultiByteToWideChar(CP_ACP , 0 , filenamestr.computerip , -1 , ip , 0);
	MultiByteToWideChar(CP_ACP , 0 , filenamestr.computerip , -1 , ip , ssn);
	_stprintf(szDefFile, _T("%s_%s"),szDefFile,ip);

	TCHAR szDefFilePath[MAX_PATH] = _T("");
	if(SHGetSpecialFolderPath(NULL, szDefFilePath, CSIDL_DESKTOPDIRECTORY, FALSE))
		FileDlg.m_ofn.lpstrInitialDir = szDefFilePath;
	TCHAR szDefExt[MAX_PATH] = _T("txt");
	FileDlg.m_ofn.lpstrFile = szDefFile;
	FileDlg.m_ofn.lpstrDefExt = szDefExt;
	FileDlg.m_ofn.hwndOwner = m_hWnd;
	FileDlg.m_ofn.nFilterIndex = 1;
	INT_PTR nRet = FileDlg.DoModal(m_hWnd);
	if (IDOK != nRet)
		return FALSE ;

	FileName = new TCHAR[MAX_KEY_LENGTH];
	_stprintf(FileName, _T("%s"), szDefFile);

	CAtlFile file;	

	if(SUCCEEDED(file.Create(szDefFile, GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS)))
	{

	CString strDate;
	SYSTEMTIME st;
	GetLocalTime(&st);
	strDate.Format(_T("%04d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);

	CString strTmp;



	//首先：写入文件头、body信息
	LPCTSTR sz_Header = BkString::Get(10032);
	LPCTSTR sz_BodyBegin = BkString::Get(10033);
	LPCTSTR sz_BodyEnd = BkString::Get(10034);
	LPCTSTR sz_TableBegin = BkString::Get(10035);
	LPCTSTR sz_TableEnd = BkString::Get(10036);
	LPCTSTR sz_End = BkString::Get(10039);



	//各自选取各自表头

	strTmp.Format(sz_Header, strDate);				
	strTmp.Append(sz_BodyBegin);

	//插入首行大标题
	LPCTSTR sz_HeadLine = BkString::Get(10050);
	strTmp.Append(sz_HeadLine);




	//第一步：插入计算机信息
	LPCTSTR sz_FirstLine =BkString::Get(10051);
	strTmp.Append(sz_FirstLine);
	strTmp.Append(sz_TableBegin);
	LPCTSTR sz_TableHeader_info = BkString::Get(10037);
	LPCTSTR sz_TableContent_info = BkString::Get(10038);
	COMPUTERINFO info_temp;
	info_temp = computerinfo();
	wchar_t *computername = new wchar_t[MAX_KEY_LENGTH];
	wchar_t *computerip = new wchar_t[MAX_KEY_LENGTH];

	int k = 0;
	k = MultiByteToWideChar(CP_ACP , 0 , info_temp.computername , -1 , computername ,0);
	MultiByteToWideChar(CP_ACP , 0 , info_temp.computername , -1 , computername ,k);
	int j = 0;

	j = MultiByteToWideChar(CP_ACP , 0 , info_temp.computerip , -1 , computerip , 0);
	MultiByteToWideChar(CP_ACP , 0 , info_temp.computerip , -1 , computerip , j);
	strTmp.Append(sz_TableHeader_info);		//表格头
	strTmp.AppendFormat(sz_TableContent_info, _T("1"),_T("计算机名："), (LPCTSTR)computername, _T(""));
	strTmp.AppendFormat(sz_TableContent_info, _T("2"), _T("计算机IP："), (LPCTSTR)computerip, _T(""));
	strTmp.AppendFormat(sz_TableContent_info, _T("3"), _T("域名："), info_temp.domain, _T(""));
	strTmp.AppendFormat(sz_TableContent_info, _T("4"), _T("操作系统版本"), info_temp.osversion, _T(""));
	delete[] computername;
	delete[] computerip;
	strTmp.Append(sz_TableEnd);

	//第二步：插入账号信息
	LPCTSTR sz_SecondLine =BkString::Get(10052);
	strTmp.Append(sz_SecondLine);
	strTmp.Append(sz_TableBegin);

	LPCTSTR sz_TableHeader_account = BkString::Get(10040);
	LPCTSTR sz_TableContent_account = BkString::Get(10041);

	GETACCOUNT account2;
	account2 = GetAllUser();
	CString number;
	strTmp.Append(sz_TableHeader_account);			//表格头
	for(int i=0; i<account2.totalcount; i++)
	{
		number.Format(TEXT("%d"),i+1);		//成功将int转换为LPCTSTR
		strTmp.AppendFormat(sz_TableContent_info, number, account2.name[i], account2.commnet[i], account2.status[i]);
	}
	strTmp.Append(sz_TableEnd);


	//第三步：插入共享信息
	LPCTSTR sz_ThirdLine =BkString::Get(10053);
	strTmp.Append(sz_ThirdLine);
	strTmp.Append(sz_TableBegin);
	LPCTSTR sz_TableHeader_share = BkString::Get(10042);
	LPCTSTR sz_TableContent_share = BkString::Get(10043);

	SHARE share2;
	SHAREPATH sharepath2;
	share2 = Check_Share_Authority_Key();
	sharepath2 = GetSharePath(); 
	CString number2;
	strTmp.Append(sz_TableHeader_share);			//表格头
	for(int i=0;i<share2.count;i++)
	{	
		number2.Format(TEXT("%d"),i+1);		//成功将int转换为LPCTSTR
		if(share2.Status[i] == 1)

			strTmp.AppendFormat(sz_TableContent_share, number2, share2.sharename[i], sharepath2.path[i], _T("是"));

		else
			strTmp.AppendFormat(sz_TableContent_share, number2, share2.sharename[i], sharepath2.path[i], _T("否"));
	}
	strTmp.Append(sz_TableEnd);

	//第四步：插入启动项信息
	LPCTSTR sz_FourthLine =BkString::Get(10054);
	strTmp.Append(sz_FourthLine);
	strTmp.Append(sz_TableBegin);
	LPCTSTR sz_TableHeader_run = BkString::Get(10044);
	LPCTSTR sz_TableContent_run = BkString::Get(10045);
	strTmp.Append(sz_TableHeader_run);			//表格头

	RUNLIST runlist2, runlist5;
	runlist2 = RunList(   KEY_SET1 ) ;
	runlist5 = RunList1(  KEY_SET4 ) ;

	CString number3;
	int listnumber2=1;
	for(int i=0;i<runlist2.count;i++)
	{
		number3.Format(TEXT("%d"),listnumber2);		//成功将int转换为LPCTSTR
		strTmp.AppendFormat(sz_TableContent_run, number3, runlist2.a[i], runlist2.b[i]);
		listnumber2++;
	}
	for(int i=0;i<runlist5.count;i++)
	{
		number3.Format(TEXT("%d"),listnumber2);		//成功将int转换为LPCTSTR
		strTmp.AppendFormat(sz_TableContent_run, number3, runlist5.a[i], runlist5.b[i]);

	}
	strTmp.Append(sz_TableEnd);

	//第五步：插入软件列表信息
	LPCTSTR sz_FifthLine =BkString::Get(10055);
	strTmp.Append(sz_FifthLine);
	strTmp.Append(sz_TableBegin);
	LPCTSTR sz_TableHeader_soft = BkString::Get(10046);
	LPCTSTR sz_TableContent_soft = BkString::Get(10047);
	strTmp.Append(sz_TableHeader_soft);			//表格头

	SOFTLIST softlist2;
	softlist2 = Soft_Stall();
	CString number4;
	for(int i=0;i<softlist2.count;i++)
	{	
		number4.Format(TEXT("%d"),i+1);	
		strTmp.AppendFormat(sz_TableContent_soft, number4, softlist2.softname[i], softlist2.softversion[i], _T(""));
	}
	strTmp.Append(sz_TableEnd);

	//第六步：插入服务项信息
	LPCTSTR sz_SixLine =BkString::Get(10056);
	strTmp.Append(sz_SixLine);
	strTmp.Append(sz_TableBegin);

	LPCTSTR sz_TableHeader_service = BkString::Get(10048);
	LPCTSTR sz_TableContent_service = BkString::Get(10049);
	strTmp.Append(sz_TableHeader_service);			//表格头
	CString number5;
	SERVICELIST serlist2= servicelist();
	for(int i=0;i<serlist2.count;i++)
	{	
		number5.Format( TEXT("%d" ),i+1);	

		if( serlist2.servicestatus[i] == 0 )
			strTmp.AppendFormat(sz_TableContent_service, number5, serlist2.servicename[i], serlist2.servicepath[i], _T("已停止"));
		else if( serlist2.servicestatus[i] == 1 )
			strTmp.AppendFormat(sz_TableContent_service, number5, serlist2.servicename[i], serlist2.servicepath[i], _T("已启动"));
		else
			strTmp.AppendFormat(sz_TableContent_service, number5, serlist2.servicename[i], serlist2.servicepath[i], _T("未知"));
	}
	strTmp.Append(sz_TableEnd);
	//将文件头、文件尾、表格信息

	//安全部Logo
	LPCTSTR sz_Table_Logo = BkString::Get(10057);
	strTmp.Append(sz_Table_Logo);


	//最后：插入body、HTML文件尾

	strTmp.Append(sz_BodyEnd);
	strTmp.Append(sz_End);


	USES_CONVERSION;
	DWORD dwWritten = 0;
	CStringA p = CT2CA(strTmp, CP_UTF8);
	file.Write(p, p.GetLength());
	file.Close();
	return TRUE;
	}
	return FALSE;
}

void CBeikeSafeMainDlg::OnBkSendExport()
{
	//发送邮件
	CSendFileTo sendmail;
	TCHAR *FileName_temp = new TCHAR[MAX_PATH];
	_stprintf(FileName_temp, _T("%s"), FileName);
	//_stprintf(FileName_temp, _T("%s%s"), FileName_temp, _T(".html"));
	char *temp_str = new char[MAX_PATH];
	int p = WideCharToMultiByte(CP_ACP, 0, FileName_temp, -1, NULL, 0, NULL, NULL );
	WideCharToMultiByte(CP_ACP, 0, FileName_temp, -1, temp_str, p, NULL, NULL );

	std::string AttachFileName = std::string(temp_str);
	COMPUTERINFO temp_info = computerinfo();

	//char *temp_name = temp_info.computername;
	CString Date_str;
	SYSTEMTIME nowtime;
	GetLocalTime(&nowtime);
	Date_str.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), nowtime.wYear, nowtime.wMonth, nowtime.wDay,nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
	TCHAR *temptime = new TCHAR[MAX_PATH];
		temptime = Date_str.GetBuffer();
	char *ch_temp_time =new char[MAX_PATH];

	int k = WideCharToMultiByte(CP_ACP, 0, temptime, -1, NULL, 0, NULL, NULL );
	WideCharToMultiByte(CP_ACP, 0, temptime, -1, ch_temp_time, k, NULL, NULL );
	std::string Subject = "计算机安全检查结果日志";
	char *SendAddress = "security@yulong.com";
	char *SendName = "信息安全部";

	char *szNoteText1 = "计算机安全检查日志";
	char *szNoteText = new char[MAX_PATH];
	sprintf(szNoteText, "%s,发送时间：%s",szNoteText1,ch_temp_time);
	if(sendmail.SendMailAttach(SendAddress,SendName,AttachFileName, Subject, szNoteText) != TRUE)
		MessageBox(_T("请再导出一次，然后发送"), _T("发送报告"), MB_ICONINFORMATION);			

	delete[] temp_str;
	delete[] FileName_temp;
	delete[] FileName;
	delete[] temptime;
	delete[] ch_temp_time;
	delete[] szNoteText;
}








BOOL CBeikeSafeMainDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{

	SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_BEIKESAFE)));
	SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_SMALL)), FALSE);

	_Module.SetActiveWindow(m_hWnd);

	InitVulFix();
	InitCountCheck();		//0、初始化首页统计
	InitAccountCheck();		//1、初始化账号框架
	InitShareCheck();	//2、初始化共享框架
	InitRunListCheck();		//3、初始化启动项框架
	InitSoftListCheck();		//4、初始化安全软件安装
	InitKeyServicesCheck();	//5、初始化系统关键服务
	
	CountCheckShow();
	AccountCheckShow();		//1、显示账号
	ShareCheckShow();		//2、显示共享

	RunListCheckShow();	//3、显示启动项列表
	SoftListCheckShow();	//4、显示关键安全软件安装
	KeyServicesShow();		//5、显示关键服务是否关闭
	//(*m_ExamHandler).InitCtrl();


	SetTabCurSel(IDC_TAB_MAIN, 0);

	m_wndHtmlOnlineInfo.Create(GetViewHWND(), IDC_IE_ONLINE_INFO, FALSE, RGB(0xFB, 0xFC, 0xFD));
	
	//(*m_pFreeVirusHandle).Init(); 

	//延迟加载
	SetTimer( TIMER_ID_MAIN_DLG_ONCE, 10, NULL );

	return TRUE;
}

void CBeikeSafeMainDlg::OnBkBtnClose()
{
	EndDialog(IDCANCEL);
}


LRESULT CBeikeSafeMainDlg::OnDestroy()
{
	
	SetMsgHandled(FALSE);

	return 0;
}

void CBeikeSafeMainDlg::OnBkBtnMax()
{
	if (WS_MAXIMIZE == (GetStyle() & WS_MAXIMIZE))
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE | HTCAPTION, 0);
	}
	else
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE | HTCAPTION, 0);
	}
}

void CBeikeSafeMainDlg::OnBkBtnMin()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE | HTCAPTION, 0);
}



void CBeikeSafeMainDlg::EndDialog(UINT uRetCode)
{

	__super::EndDialog(uRetCode);
}

void CBeikeSafeMainDlg::TabMainSelChange( int nTabOldItem, int nTabNewItem )
{
	SetTabCurSel(IDC_TAB_MAIN, nTabNewItem);
	OnBkTabMainSelChange( nTabOldItem, nTabNewItem );
}

BOOL CBeikeSafeMainDlg::OnBkTabMainSelChange(int nTabItemIDOld, int nTabItemIDNew)
{

	DEBUG_TRACE(L"Tab Change %d, %d\r\n", nTabItemIDOld, nTabItemIDNew);

	BOOL bRet = FALSE;

	if (m_bFirstPageChange)
	{
		if (0 != nTabItemIDNew)
		{
			PostMessage(MSG_APP_DELAY_EXAM, FALSE);
		}

		m_bFirstPageChange = FALSE;
	}



	bRet = TRUE;
	switch (nTabItemIDNew)
	{
	case 0:
		//首页
		if (m_bPage0NeverShowed)
		{
			PostMessage(MSG_APP_DELAY_NAVIGATE_IE);
			PostMessage(MSG_APP_DELAY_EXAM, TRUE);

			m_bPage0NeverShowed = FALSE;
		}

		break;

	case 1:
		//基本检查
		//显示结果
	//	SystemNamesView(m_wndAccountListView);
		break;

	case 2:	

  		//修复漏洞
		//m_viewVulfix.ShowWindow(SW_SHOW);
	m_viewVulfix.m_viewSoftVul.InitEnv();
		/*
		if(!m_bVulfixInited || m_bVulfixRescanRequired)
		{
			BOOL toRescan = TRUE;
			if(m_bVulfixRescanRequired && theEngine && theEngine->m_isRepairing)
			{
				toRescan = FALSE;
			}			
			m_bVulfixInited = TRUE;
			m_bVulfixRescanRequired = FALSE;
			if(toRescan)
				m_viewVulfix.m_viewSoftVul.InitEnv();
		}
		*/
		
		break;
	case 3:
		
			//报告生成
	
			if( OnExportResult() == TRUE)
			{
		
	
			MessageBox(_T("日志保存成功"), _T("导出报告"), MB_ICONINFORMATION);			

			SetItemVisible( 11005, TRUE);
			}
			else
			{
			SetItemVisible( 11005, FALSE);
			}
			

		

					
		break;
	case  4:
		//系统优化
     	


	case  5:
		//垃圾清理

	   //  MessageBox(_T("开发中，敬请期待..."), _T("垃圾清理"), MB_ICONINFORMATION);		


	case  6:
		//监控防御
	
		
	case  7:
		//软件管理
		
		break;
	}

	return TRUE;
}







void CBeikeSafeMainDlg::OnSysCommand(UINT nID, CPoint point)
{
	SetMsgHandled(FALSE);

	switch (nID & 0xFFF0)
	{
	case SC_CLOSE:
		SetMsgHandled(TRUE);
		if( !g_bkMsgBox )		//已经弹出msgbox，不关闭窗口
			EndDialog(IDCANCEL);
		break;
	case SC_RESTORE:
		{
			DWORD dwStyle = GetStyle();
			//dwStyle = dwStyle & (WS_MAXIMIZE | WS_MINIMIZE);
			if (WS_MINIMIZE == (dwStyle & WS_MINIMIZE))
				break;

			if (WS_MAXIMIZE == (dwStyle & WS_MAXIMIZE))
			{
				SetItemAttribute(IDC_BTN_SYS_MAX, "skin", "maxbtn");
				break;
			}
		}
	case SC_MAXIMIZE:
		SetItemAttribute(IDC_BTN_SYS_MAX, "skin", "restorebtn");
		break;
	}
}

LRESULT CBeikeSafeMainDlg::OnAppDelayNavigateIE(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	CString	strURL;
// 	if ( SUCCEEDED(CAppPath::Instance().GetLeidianDataPath(strURL)) )
// 		strURL.Append(_T("\\html\\error.html"));

	m_wndHtmlOnlineInfo.Show2(L"http://www.update.microsoft.com/windowsupdate/v6/default.aspx?ln=zh-cn",strURL);

	return 0;
}

