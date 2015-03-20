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
	void InitCountCheck();			//0����ʼ����ҳͳ�ƴ���
	void	InitAccountCheck();		//1����ʼ���˺ſ��
	void	InitShareCheck();	//2����ʼ��������
	void	InitRunListCheck();		//3����ʼ����������
	void	 InitSoftListCheck();		//4����ʼ����ȫ�����װ
	void	InitKeyServicesCheck();	//5����ʼ��ϵͳ�ؼ�����

	void CBeikeSafeMainDlg::InitExportResult();
	
	void CountCheckShow();
	void	AccountCheckShow();		//1����ʾ�˺�
	void	ShareCheckShow();		//2����ʾ����


	void	SoftListCheckShow();	//4����ʾ�ؼ���ȫ�����װ
	void	KeyServicesShow();		//5����ʾ�ؼ������Ƿ�ر�


	BOOL OnExportResult();   //��־��������
	void OnBkSendExport();		//����Outlook�����ʼ�
	
protected:
	// Vulfix 
	BOOL			m_bVulfixInited;
	BOOL			m_bVulfixRescanRequired;
	CEmbeddedView	m_viewVulfix;
	CEmbeddedView	m_viewCheck;



	BOOL			m_bIeFixInit;
	BOOL			m_bKClearInit;

public:

	CListViewCtrlEx  m_wndCountListView;	//��ҳ
	CListViewCtrlEx  m_wndAccountListView;	//�˺�
	CListViewCtrlEx  m_wndInstanceListView;	//
	CListViewCtrlEx  m_wndShaerListView;	//����
	CListViewCtrlEx  m_wndRunListView;		//������
	CListViewCtrlEx  m_wndSoftListView;		//��ȫ���
	CListViewCtrlEx  m_wndKeyServiceView;		//�ؼ�����

	CListViewCtrlEx m_wndExportResultView;
	CListViewCtrlEx m_wndListCtrlVul;
	//����ȫ�ֱ���
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
