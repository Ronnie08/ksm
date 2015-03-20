#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include "Checkbase\ListViewCtrlEx.h"
#include "GetSystemNames.h"
#include "GetShareList.h"
#include "wtlhelper/whwindow.h"
SHARE Check_Share_Authority_Key();
ACCOUNT Change_SAM_Access();
using namespace std;

void SystemNamesView(CListViewCtrlEx m_SystemNamesView){

	//读取系统账户名称

	ACCOUNT acc;
	acc=Change_SAM_Access();
	;
	for(int i=0;i<acc.count;i++)
	{	
		int nItem = m_SystemNamesView.Append((LPCTSTR(i)),  0, SUBITEM_TEXT);
		m_SystemNamesView.AppendSubItem(nItem, acc.a[i]);
		m_SystemNamesView.SetCheckState(nItem, TRUE);
		
	}	
	
}