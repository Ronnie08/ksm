#pragma once

#include <wtlhelper/whwindow.h>
//#include "bkmsgdefine.h"

#include "beikesafemsgbox.h"
#include "vulfix/BeikeVulfixHandler.h"
#include "bksafeuihandlerbase.h"
#include "Vulfix\ListViewCtrlEx.h"


class CBeikeSafeExamUIHandler;


class CBeikeSafeMainDlg
    : public CBkDialogImpl<CBeikeSafeMainDlg>
    , public CWHRoundRectFrameHelper<CBeikeSafeMainDlg>
    , public CBkNavigator
	, public CBkSafeUIHandlerMgr

{
public:
    CBeikeSafeMainDlg();
    ~CBeikeSafeMainDlg();

    void SetDefaultNavigate(LPCWSTR lpszNavigate);

    void EndDialog(UINT uRetCode);

 	void TabMainSelChange( int nTabOldItem, int nTabNewItem );

protected:

    CBeikeSafeExamUIHandler*         m_ExamHandler;

    CWHHtmlContainerWindow m_wndHtmlOnlineInfo;

//    CBeikeSafeSettingNavigator m_SettingCaller;

    BOOL m_bFirstPageChange;
    BOOL m_bPage0NeverShowed;
    BOOL m_bPage1NeverShowed;
    BOOL m_bPage3NeverShowed;
	BOOL m_bPage5NeverShowed;

    CString m_strDefaultNavigate;

    void OnBkBtnClose();
    void OnBkBtnMax();
    void OnBkBtnMin();
	void OnHtmlExport();
	BOOL _ExportResult( LPCTSTR szFileName, T_ExportInfoArray &arrExportInfo );

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	LRESULT OnDestroy();
	LRESULT OnTimer( UINT_PTR uIdEvent );
    LRESULT OnAppDefaultNavigate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);

    LRESULT OnAppEchoFinish(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
    BOOL OnBkTabMainSelChange(int nTabItemIDOld, int nTabItemIDNew);
    void OnSysCommand(UINT nID, CPoint point);
	
	LRESULT OnAppDelayExam(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnAppDelayNavigateIE(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	void InitVulFix();

	void	RunListCheckShow();		
	void InitCountCheck();			//0、初始化首页统计窗口
	void	InitAccountCheck();		//1、初始化账号框架
	void	InitShareCheck();	//2、初始化共享框架
	void	InitRunListCheck();		//3、初始化启动项框架
	void	 InitSoftListCheck();		//4、初始化安全软件安装
	void	InitKeyServicesCheck();	//5、初始化系统关键服务

	void CBeikeSafeMainDlg::InitExportResult();
	
	void CountCheckShow();
	void	AccountCheckShow();		//1、显示账号
	void	ShareCheckShow();		//2、显示共享


	void	SoftListCheckShow();	//4、显示关键安全软件安装
	void	KeyServicesShow();		//5、显示关键服务是否关闭


	BOOL OnExportResult();   //日志导出的类
	void OnBkSendExport();		//启动Outlook发送邮件
	
protected:
	// Vulfix 
	BOOL			m_bVulfixInited;
	BOOL			m_bVulfixRescanRequired;
	CEmbeddedView	m_viewVulfix;
	CEmbeddedView	m_viewCheck;



	BOOL			m_bIeFixInit;
	BOOL			m_bKClearInit;

public:

	CListViewCtrlEx  m_wndCountListView;	//首页
	CListViewCtrlEx  m_wndAccountListView;	//账号
	CListViewCtrlEx  m_wndInstanceListView;	//
	CListViewCtrlEx  m_wndShaerListView;	//共享
	CListViewCtrlEx  m_wndRunListView;		//启动项
	CListViewCtrlEx  m_wndSoftListView;		//安全软件
	CListViewCtrlEx  m_wndKeyServiceView;		//关键服务

	CListViewCtrlEx m_wndExportResultView;
	CListViewCtrlEx m_wndListCtrlVul;
	//定义全局变量
	TCHAR *FileName;

	CEdit m_wndGameDirEdit;
	CListBox m_wndDllListBox;
    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        //BK_NOTIFY_ID_COMMAND(IDCANCEL, OnBkBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBkBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MAX, OnBkBtnMax)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MIN, OnBkBtnMin)
        BK_NOTIFY_TAB_SELCHANGE(IDC_TAB_MAIN, OnBkTabMainSelChange)
		BK_NOTIFY_ID_COMMAND(1330, OnBkSendExport)
    BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX_DECLARE(CBeikeSafeMainDlg)
};
