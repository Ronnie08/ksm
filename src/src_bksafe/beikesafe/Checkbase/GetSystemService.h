// GetSysService.cpp : Defines the entry point for the  application.
/*
//	author : YRH
//	time : 2012-11-2
*/
#include "Checkbase\Stdafx.h"
#include <windows.h>
#include <iostream>
#include<stdio.h>
using namespace std;
typedef struct SERVICELIST{
	TCHAR *servicename[MAX_KEY_LENGTH];
	TCHAR *servicepath[MAX_KEY_LENGTH];
	int servicestatus[MAX_KEY_LENGTH];
	unsigned long count;
}service;
SERVICELIST servicelist()
{
	SERVICELIST servicelist;
	SC_HANDLE SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); //��ϵͳ���������
	//	if(SCMan == NULL)
	//	{
	//		printf("OpenSCManager Eorror\n");
	//		return;
	//	}
	LPENUM_SERVICE_STATUS service_status; //����ϵͳ����Ľṹ
	DWORD cbBytesNeeded = NULL; 
	DWORD ServicesReturned = NULL;
	DWORD ResumeHandle = NULL;

	service_status = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, 1024 * 64);//�����ڴ棬ע���С


	//��ȡϵͳ����ļ���Ϣ
	bool ESS = EnumServicesStatus(SCMan, //ϵͳ������
		SERVICE_WIN32, //���������
		SERVICE_STATE_ALL,  //�����״̬
		(LPENUM_SERVICE_STATUS)service_status,  //���������ϵͳ����Ľṹ
		1024 * 64,  // �ṹ�Ĵ�С
		&cbBytesNeeded, //������������շ�������ķ���
		&ServicesReturned, //������������շ��ط��������
		&ResumeHandle); //���������������һ�ε��ñ���Ϊ0������Ϊ0����ɹ�
	if(ESS == NULL) 
	{
		printf("EnumServicesStatus Eorror\n");
		return servicelist;
	}
	//printf("��ǰ��������Ϊ:%d\n",ServicesReturned);
	servicelist.count = ServicesReturned;

	for(int i = 0; i < ServicesReturned; i++)
	{

		//printf("������: %s ", service_status[i].lpDisplayName);
		//  LPSTR
		//sprintf(servicelist.servicename[i],"%s",service_status[i].lpDisplayName);
		servicelist.servicename[i] = service_status[i].lpDisplayName;
		//printf("��������%s",servicelist.servicename[i]);
		//	printf("\n");

		//������   1
		//��ֹͣ   0
		//δ֪		2

		switch(service_status[i].ServiceStatus.dwCurrentState)// ����״̬
		{
		case SERVICE_CONTINUE_PENDING:
			//printf("״̬: ����");
			servicelist.servicestatus[i] = 0;
			break;
		case SERVICE_PAUSE_PENDING:
			//printf("״̬: ������ͣ ");
			servicelist.servicestatus[i] = 0;
			break;
		case SERVICE_PAUSED:
			//printf("״̬: ����ͣ ");
			servicelist.servicestatus[i] = 0;
			break;
		case SERVICE_RUNNING:
			//printf("״̬: ���� ");
			servicelist.servicestatus[i] = 1;
			break;
		case SERVICE_START_PENDING:
			//printf("״̬: ���� ");
			servicelist.servicestatus[i] = 1;
			break;
		case SERVICE_STOPPED:
			//printf("״̬: ֹͣ ");
			servicelist.servicestatus[i] = 0;
			break;
		default:
			//printf("״̬: δ֪ ");
			servicelist.servicestatus[i] = 2;
			break;
		}

		LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL; //������ϸ��Ϣ�ṹ
		SC_HANDLE service_curren = NULL; //��ǰ�ķ�����
		service_curren = OpenService(SCMan, service_status[i].lpServiceName, SERVICE_QUERY_CONFIG); //�򿪵�ǰ����
		lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 8 * 1024);      //�����ڴ棬 ���Ϊ8kb 

		if(NULL == QueryServiceConfig(service_curren, lpServiceConfig, 8 * 1024, &ResumeHandle))
		{
			printf("QueryServiceConfig Error\n");
			return servicelist;
		}
		//printf("·��: %s ", lpServiceConfig->lpBinaryPathName);//�����·��

		//sprintf(servicelist.servicepath[i],"%s",lpServiceConfig->lpBinaryPathName);
		servicelist.servicepath[i] = lpServiceConfig->lpBinaryPathName;
		CloseServiceHandle(service_curren);//�رյ�ǰ����ľ��
	}
	CloseServiceHandle(SCMan);//�رշ���������ľ��

	return servicelist;
}