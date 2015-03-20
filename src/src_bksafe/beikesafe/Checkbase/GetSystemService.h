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
	SC_HANDLE SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); //打开系统服务控制器
	//	if(SCMan == NULL)
	//	{
	//		printf("OpenSCManager Eorror\n");
	//		return;
	//	}
	LPENUM_SERVICE_STATUS service_status; //保存系统服务的结构
	DWORD cbBytesNeeded = NULL; 
	DWORD ServicesReturned = NULL;
	DWORD ResumeHandle = NULL;

	service_status = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, 1024 * 64);//分配内存，注意大小


	//获取系统服务的简单信息
	bool ESS = EnumServicesStatus(SCMan, //系统服务句柄
		SERVICE_WIN32, //服务的类型
		SERVICE_STATE_ALL,  //服务的状态
		(LPENUM_SERVICE_STATUS)service_status,  //输出参数，系统服务的结构
		1024 * 64,  // 结构的大小
		&cbBytesNeeded, //输出参数，接收返回所需的服务
		&ServicesReturned, //输出参数，接收返回服务的数量
		&ResumeHandle); //输入输出参数，第一次调用必须为0，返回为0代表成功
	if(ESS == NULL) 
	{
		printf("EnumServicesStatus Eorror\n");
		return servicelist;
	}
	//printf("当前服务数量为:%d\n",ServicesReturned);
	servicelist.count = ServicesReturned;

	for(int i = 0; i < ServicesReturned; i++)
	{

		//printf("服务名: %s ", service_status[i].lpDisplayName);
		//  LPSTR
		//sprintf(servicelist.servicename[i],"%s",service_status[i].lpDisplayName);
		servicelist.servicename[i] = service_status[i].lpDisplayName;
		//printf("服务名：%s",servicelist.servicename[i]);
		//	printf("\n");

		//已启动   1
		//已停止   0
		//未知		2

		switch(service_status[i].ServiceStatus.dwCurrentState)// 服务状态
		{
		case SERVICE_CONTINUE_PENDING:
			//printf("状态: 挂起");
			servicelist.servicestatus[i] = 0;
			break;
		case SERVICE_PAUSE_PENDING:
			//printf("状态: 正在暂停 ");
			servicelist.servicestatus[i] = 0;
			break;
		case SERVICE_PAUSED:
			//printf("状态: 已暂停 ");
			servicelist.servicestatus[i] = 0;
			break;
		case SERVICE_RUNNING:
			//printf("状态: 运行 ");
			servicelist.servicestatus[i] = 1;
			break;
		case SERVICE_START_PENDING:
			//printf("状态: 启动 ");
			servicelist.servicestatus[i] = 1;
			break;
		case SERVICE_STOPPED:
			//printf("状态: 停止 ");
			servicelist.servicestatus[i] = 0;
			break;
		default:
			//printf("状态: 未知 ");
			servicelist.servicestatus[i] = 2;
			break;
		}

		LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL; //服务详细信息结构
		SC_HANDLE service_curren = NULL; //当前的服务句柄
		service_curren = OpenService(SCMan, service_status[i].lpServiceName, SERVICE_QUERY_CONFIG); //打开当前服务
		lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 8 * 1024);      //分配内存， 最大为8kb 

		if(NULL == QueryServiceConfig(service_curren, lpServiceConfig, 8 * 1024, &ResumeHandle))
		{
			printf("QueryServiceConfig Error\n");
			return servicelist;
		}
		//printf("路径: %s ", lpServiceConfig->lpBinaryPathName);//服务的路径

		//sprintf(servicelist.servicepath[i],"%s",lpServiceConfig->lpBinaryPathName);
		servicelist.servicepath[i] = lpServiceConfig->lpBinaryPathName;
		CloseServiceHandle(service_curren);//关闭当前服务的句柄
	}
	CloseServiceHandle(SCMan);//关闭服务管理器的句柄

	return servicelist;
}