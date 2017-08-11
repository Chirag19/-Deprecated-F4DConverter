
// F4DConverter.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "F4DConverter.h"
#include "MainFrm.h"

#include "F4DConverterDoc.h"
#include "F4DConverterView.h"

#include <map>
#include <vector>
#include <sstream>
#include <locale>
#include <algorithm>

#include "converter/LogWriter.h"
#include "converter/ConverterManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CF4DConverterApp

BEGIN_MESSAGE_MAP(CF4DConverterApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CF4DConverterApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()

std::map<std::wstring, std::wstring> arguments;

// arguments for conversion configuration
std::wstring inputFolder(L"Input Folder");
std::wstring outputFolder(L"Output Folder");
std::wstring logPath(L"Log File Path");
std::wstring indexCreation(L"Create Indices");
std::wstring idPrefix(L"idPrefix");
std::wstring idSuffix(L"idSuffix");
// arguments for processing parameters
std::wstring occlusionCulling(L"Occlusion Culling");
std::wstring unitScaleFactor(L"Unit Scale Factor");

bool extractArguments(std::wstring& cmdLine)
{
	std::wstring token;
	std::wistringstream iss(cmdLine);
	std::vector<std::wstring> tokens;
	while (getline(iss, token, L' '))
	{
		tokens.push_back(token);
	}

	size_t tokenCount = tokens.size();
	for (size_t i = 0; i < tokenCount; i++)
	{
		if (tokens[i].substr(0, 1) == std::wstring(L"-"))
		{
			if (i == tokenCount - 1 || tokens[i + 1].substr(0, 1) == std::wstring(L"-"))
			{
				return false;
			}

			if (tokens[i] == std::wstring(L"-inputFolder"))
			{
				arguments[inputFolder] = tokens[i + 1];
				i++;
				continue;
			}
			if (tokens[i] == std::wstring(L"-outputFolder"))
			{
				arguments[outputFolder] = tokens[i + 1];
				i++;
				continue;
			}
			if (tokens[i] == std::wstring(L"-log"))
			{
				arguments[logPath] = tokens[i + 1];
				i++;
				continue;
			}

			if (tokens[i] == std::wstring(L"-indexing"))
			{
				arguments[indexCreation] = tokens[i + 1];
				i++;
				continue;
			}

			if (tokens[i] == std::wstring(L"-idPrefix"))
			{
				arguments[idPrefix] = tokens[i + 1];
				i++;
				continue;
			}

			if (tokens[i] == std::wstring(L"-idSuffix"))
			{
				arguments[idSuffix] = tokens[i + 1];
				i++;
				continue;
			}

			if (tokens[i] == std::wstring(L"-oc"))
			{
				arguments[occlusionCulling] = tokens[i + 1];
				i++;
				continue;
			}

			if (tokens[i] == std::wstring(L"-usf"))
			{
				arguments[unitScaleFactor] = tokens[i + 1];
				i++;
				continue;
			}

			// TODO(khj 20170305) : LOG invalid arguments
			return false;
		}
		else
		{
			// TODO(khj 20170305) : LOG invalid arguments
			return false;
		}
	}

	if (arguments.find(outputFolder) == arguments.end())
		return false;

	if (arguments.find(inputFolder) == arguments.end() && arguments.find(indexCreation) == arguments.end())
		return false;

	if (arguments.find(inputFolder) != arguments.end())
	{
		if (arguments.find(logPath) == arguments.end())
			return false;
	}

	if (arguments.find(indexCreation) != arguments.end())
	{
		if (arguments[indexCreation] != std::wstring(L"Y") &&
			arguments[indexCreation] != std::wstring(L"y") &&
			arguments[indexCreation] != std::wstring(L"N") &&
			arguments[indexCreation] != std::wstring(L"n"))
			return false;
	}

	if (arguments.find(occlusionCulling) != arguments.end())
	{
		if (arguments[occlusionCulling] != std::wstring(L"Y") &&
			arguments[occlusionCulling] != std::wstring(L"y") &&
			arguments[occlusionCulling] != std::wstring(L"N") &&
			arguments[occlusionCulling] != std::wstring(L"n"))
			return false;
	}

	if (arguments.find(unitScaleFactor) != arguments.end())
	{
		try
		{
			double scaleFactor = std::stod(arguments[unitScaleFactor]);

			if (scaleFactor < 0.001)
				return false;
		}
		catch(const std::invalid_argument& error)
		{
			std::string errorMessage = error.what();
			return false;
		}
		catch(const std::out_of_range& error)
		{
			std::string errorMessage = error.what();
			return false;
		}
	}

	return true;
}
// CF4DConverterApp ����

CF4DConverterApp::CF4DConverterApp()
{
	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("F4DConverter.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CF4DConverterApp ��ü�Դϴ�.

CF4DConverterApp theApp;


// CF4DConverterApp �ʱ�ȭ

BOOL CF4DConverterApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CF4DConverterDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CF4DConverterView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	std::wstring cmdLine(this->m_lpCmdLine);

	bool isValidParams = false;
	if (cmdLine.length() != 0)
	{
		if (!(isValidParams = extractArguments(cmdLine)))
			return FALSE;
	}

	if (arguments.find(logPath) != arguments.end())
	{
		LogWriter::getLogWriter()->start();
		LogWriter::getLogWriter()->setFullPath(arguments[logPath]);
	}

	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (isValidParams)
	{
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
		CConverterManager::getConverterManager()->setIsCliMode(true);
		if (arguments.find(inputFolder) != arguments.end())
		{
			CConverterManager::getConverterManager()->setConversionOn(true);
			CConverterManager::getConverterManager()->setInputFolder(arguments[inputFolder]);
			CConverterManager::getConverterManager()->setOutputFolder(arguments[outputFolder]);

			if (arguments.find(occlusionCulling) != arguments.end())
			{
				if (arguments[occlusionCulling] == std::wstring(L"Y") ||
					arguments[occlusionCulling] == std::wstring(L"y"))
					CConverterManager::getConverterManager()->setOcclusionCulling(true);
				else
					CConverterManager::getConverterManager()->setOcclusionCulling(false);
			}
			else
				CConverterManager::getConverterManager()->setOcclusionCulling(false);

			if (arguments.find(unitScaleFactor) != arguments.end())
			{
				CConverterManager::getConverterManager()->setUnitScaleFactor(std::stod(arguments[unitScaleFactor]));
			}
		}
		else
			CConverterManager::getConverterManager()->setConversionOn(false);

		if (arguments.find(indexCreation) != arguments.end())
		{
			if (arguments[indexCreation] == std::wstring(L"Y") ||
				arguments[indexCreation] == std::wstring(L"y"))
			{
				CConverterManager::getConverterManager()->setIndexCreation(true);
				CConverterManager::getConverterManager()->setOutputFolder(arguments[outputFolder]);
			}
			else
			{
				CConverterManager::getConverterManager()->setIndexCreation(false);
			}
		}
		else
			CConverterManager::getConverterManager()->setIndexCreation(false);

		if (arguments.find(idPrefix) != arguments.end())
		{
			CConverterManager::getConverterManager()->setIdPrefix(arguments[idPrefix]);
		}

		if (arguments.find(idSuffix) != arguments.end())
		{
			CConverterManager::getConverterManager()->setIdSuffix(arguments[idSuffix]);
		}
	}
	else
		CConverterManager::getConverterManager()->setIsCliMode(false);


	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	if (m_pMainWnd == NULL && isValidParams)
		this->OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL);

	if (m_pMainWnd == NULL)
	{
		if (LogWriter::getLogWriter()->isStarted())
		{
			LogWriter::getLogWriter()->addContents(std::wstring(ERROR_FLAG), false);
			LogWriter::getLogWriter()->addContents(std::wstring(CANNOT_INITIALIZE), true);
			LogWriter::getLogWriter()->finish();
			LogWriter::getLogWriter()->save();
		}
		return FALSE;
	}

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

// CF4DConverterApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CF4DConverterApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CF4DConverterApp �޽��� ó����



