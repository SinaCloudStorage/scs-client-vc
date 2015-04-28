
// scs-client.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "scs-client.h"
#include "scs-clientDlg.h"
#include <string>
#include "Log4cplusSwitch.h"
#include <conio.h>
#include "singleton.h"
#include "multi_http.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CscsclientApp

BEGIN_MESSAGE_MAP(CscsclientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CscsclientApp ����

CscsclientApp::CscsclientApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CscsclientApp ����

CscsclientApp theApp;


// CscsclientApp ��ʼ��

BOOL CscsclientApp::InitInstance()
{
	CWinApp::InitInstance();


	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

#ifdef RECORD_LOG
	std::wstring cur_dir = L"c:\\";
	WCHAR modulePath[MAX_PATH];
	if ( ::GetModuleFileName( NULL, modulePath, MAX_PATH ) )
	{
		cur_dir = modulePath;
		std::wstring::size_type p = cur_dir.rfind(L'\\');
		if ( p != std::wstring::npos )
		{
			cur_dir = cur_dir.substr( 0, p+1 );
		}
	}
	std::wstring temp = cur_dir + L"log4cplus.prop";
	LOG4CPLUS_INIT(temp.c_str());

//	AllocConsole();
#endif

	singleton_ex<multi_http>::create_instance();

	CscsclientDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	singleton_ex<multi_http>::destroy_instance();

#ifdef RECORD_LOG
//	FreeConsole();
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

