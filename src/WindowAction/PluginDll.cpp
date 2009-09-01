// PluginDLL is a template DLL which demonstrates how to create
// AutoBot component in a DLL which is called from AutoBot application
//
// Bruian 
// (C) 2008 Bruianio co
// www.bruian.com
//**************************************************
#include "PluginDll.h"

#include <windows.h>
#include <process.h>

#include <windowsx.h>
#include <shellapi.h>
#include <Shlobj.h>
#include <atlconv.h>

#include "wx/dynlib.h"
#include "wx/window.h"
#include "wx/process.h"
//**************************************************
typedef struct _f_param
{
	wxString sWindowTitle;
	wxString sWindowText;
	wxString sWindowClass;
	long	 lHandlePID;
	bool	 bFindMethod;
	long	 lHandleWindow;
} FIND_PARAM;
//**************************************************
//Information of plugin setting
IMPLEMENT_PLUGIN(CWindowAction)
//**************************************************
// Class: CComponent
// Name: WindowAction
// Version: 1.0
// Description: Класс компоненты для поиска окон и диалогов по различным параметрам
// Component GUID: {407501AA-17F9-43ED-964E-868F9C1A5C6C}
class CWindowAction
{
private:
	wxString	mWS_WindowName;
	wxString	mWS_WindowText;
	wxString	mWS_CriterionPID;
	wxString	mWS_WindowHandle;
	wxString	mWS_ResultFind;
	wxString	mWS_WindowClass;
	wxString	mWS_ResultWindowName;
	wxString	mWS_ResultWindowText;
	wxString	mWS_ResultPID;
	wxString	mWS_ResultHandle;
	wxString	mWS_ResultHandleParent;
	wxString	mWS_ResultClassWindow;
public:
	wxString typecoGUID;				//GUID типа компоненты

	//EXTERN member
	int				m_WinActionType;	//m3 - тип выполняемой операции над окнами
	const xchar*	m_WindowName;		//m4 - критерий, заголовок окна
	bool			m_FullCorrespond;	//m5 - полное соответствие в заголовке окна
	const xchar*	m_WindowText;		//m6 - критерий, текст в окне
	const xchar*	m_CriterionPID;		//m7 - критерий, Process ID
	const xchar*	m_WindowHandle;		//m8 - критерий, хэндл окна
	const xchar*	m_WindowClass;		//m9 - критерий, класс окна
	const xchar*	m_ResultFind;		//m10 - результат поиска окна
	const xchar*	m_ResultWindowName;	//m11 - результат, заголовок окна
	const xchar*	m_ResultWindowText;	//m12 - результат, текст в окне
	const xchar*	m_ResultPID;		//m13 - результат, PID процесса окна
	const xchar*	m_ResultHandle;		//m14 - результат, хэндл окна
	const xchar*	m_ResultHandleParent;//m15 - результат, хэндл родительского окна
	const xchar*	m_ResultClassWindow;//m16 - результат, класс окна
	int				m_WindowMessage;	//m17 - сообщение посылаемое окну

	const xchar*	m_GUIDBot;			//GUID бота
	const xchar*	m_GUIDRule;			//GUID правила
public:
	void set_WindowName(const xchar* string) { mWS_WindowName = wxString(string); m_WindowName = mWS_WindowName.c_str(); }
	void set_WindowText(const xchar* string) { mWS_WindowText = wxString(string); m_WindowText = mWS_WindowText.c_str(); }
	void set_CriterionPID(const xchar* string) { mWS_CriterionPID = wxString(string); m_CriterionPID = mWS_CriterionPID.c_str(); }
	void set_WindowHandle(const xchar* string) { mWS_WindowHandle = wxString(string); m_WindowHandle = mWS_WindowHandle.c_str(); }
	void set_WindowClass(const xchar* string) { mWS_WindowClass = wxString(string); m_WindowClass = mWS_WindowClass.c_str(); }
public:
	CWindowAction();
	void Execute();
};
//**************************************************
namespace script
{
	SCRIPT_CLASS_REGISTER_BEGIN( CWindowAction )
		.declareClass<CWindowAction>(wxT("CComponent_WindowAction"))
			.constructor< objOwn >(wxT("create"))
			.method(wxT("Execute"), &CWindowAction::Execute)
			.setter(wxT("set_WinActionType"), &CWindowAction::m_WinActionType)
			.method(wxT("set_WindowName"), &CWindowAction::set_WindowName)
			.setter(wxT("set_FullCorrespond"), &CWindowAction::m_FullCorrespond)
			.method(wxT("set_WindowText"), &CWindowAction::set_WindowText)
			.method(wxT("set_CriterionPID"), &CWindowAction::set_CriterionPID)
			.method(wxT("set_WindowHandle"), &CWindowAction::set_WindowHandle)
			.method(wxT("set_WindowClass"), &CWindowAction::set_WindowClass)
			.setter(wxT("set_WindowMessage"), &CWindowAction::m_WindowMessage)
			.getter(wxT("get_ResultFind"), &CWindowAction::m_ResultFind)
			.getter(wxT("get_ResultWindowName"), &CWindowAction::m_ResultWindowName)
			.getter(wxT("get_ResultWindowText"), &CWindowAction::m_ResultWindowText)
			.getter(wxT("get_ResultPID"), &CWindowAction::m_ResultPID)
			.getter(wxT("get_ResultHandle"), &CWindowAction::m_ResultHandle)
			.getter(wxT("get_ResultHandleParent"), &CWindowAction::m_ResultHandleParent)
			.getter(wxT("get_ResultClassWindow"), &CWindowAction::m_ResultClassWindow)
			.setter(wxT("set_GUIDBot"), &CWindowAction::m_GUIDBot)
			.setter(wxT("set_GUIDRule"), &CWindowAction::m_GUIDRule)
	SCRIPT_CLASS_REGISTER_END( CWindowAction )
}
//**************************************************
CWindowAction::CWindowAction()
{
	typecoGUID			= wxT("407501AA-17F9-43ED-964E-868F9C1A5C6C");
	m_WinActionType		= 0;
	m_WindowName		= wxEmptyString;
	m_FullCorrespond	= false;
	m_WindowText		= wxEmptyString;
	m_CriterionPID		= wxEmptyString;
	m_WindowHandle		= wxEmptyString;
	m_WindowClass		= wxEmptyString;
	m_ResultFind		= wxEmptyString;
	m_ResultWindowName	= wxEmptyString;
	m_ResultWindowText	= wxEmptyString;
	m_ResultPID			= wxEmptyString;
	m_ResultHandle		= wxEmptyString;
	m_ResultHandleParent = wxEmptyString;
	m_ResultClassWindow = wxEmptyString;
	m_WindowMessage		= 0;
	m_GUIDBot			= wxEmptyString;
	m_GUIDRule			= wxEmptyString;
}
//**************************************************
BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam) 
{
	FIND_PARAM *param = (FIND_PARAM *)lParam;

	if(param->lHandlePID != -1)
	{
		DWORD dwPid; GetWindowThreadProcessId(hwndChild, &dwPid);
		if(dwPid == (DWORD)param->lHandlePID)
		{
			if(param->sWindowClass != wxEmptyString)
			{
				PWINDOWINFO pwi = new WINDOWINFO(); 
				GetWindowInfo(hwndChild, pwi);

				double atom;
				param->sWindowClass.ToDouble(&atom);
				if(pwi->atomWindowType == (WORD)atom)
				{
					if(param->sWindowTitle != wxEmptyString)
					{
						int cTxtLen = GetWindowTextLength(hwndChild);
						LPWSTR pszMem = (LPWSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE);
						GetWindowText(hwndChild, pszMem, cTxtLen + 1);
						wxString str(pszMem);
						bool Contain = str.Contains(param->sWindowTitle);
						VirtualFree(pszMem, 0, MEM_RELEASE);
						if(str == param->sWindowTitle)
						{
							param->lHandleWindow = (long)hwndChild;
							return false;
						}
						else if((Contain) && (param->bFindMethod == false))
						{
							param->lHandleWindow = (long)hwndChild;
							return false;
						}
					}
					else
					{
						param->lHandleWindow = (long)hwndChild;
						return false;
					}
				}
				delete pwi;
			}
			else
			{
				if(param->sWindowTitle != wxEmptyString)
				{
					int cTxtLen = GetWindowTextLength(hwndChild);
					LPWSTR pszMem = (LPWSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE);
					GetWindowText(hwndChild, pszMem, cTxtLen + 1);
					wxString str(pszMem);
					bool Contain = str.Contains(param->sWindowTitle);
					VirtualFree(pszMem, 0, MEM_RELEASE);
					if(str == param->sWindowTitle)
					{
						param->lHandleWindow = (long)hwndChild;
						return false;
					}
					else if((Contain) && (param->bFindMethod == false))
					{
						param->lHandleWindow = (long)hwndChild;
						return false;
					}
				}
				else
				{
					param->lHandleWindow = (long)hwndChild;
					return false;
				}
			}
		}
	}
	else
	{
		if(param->sWindowClass != wxEmptyString)
		{
			PWINDOWINFO pwi = new WINDOWINFO(); 
			GetWindowInfo(hwndChild, pwi);
			double atom;
			param->sWindowClass.ToDouble(&atom);

			if(pwi->atomWindowType == (WORD)atom)
			{
				if(param->sWindowTitle != wxEmptyString)
				{
					int cTxtLen = GetWindowTextLength(hwndChild);
					LPWSTR pszMem = (LPWSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE);
					GetWindowText(hwndChild, pszMem, cTxtLen + 1);
					wxString str(pszMem);
					bool Contain = str.Contains(param->sWindowTitle);
					VirtualFree(pszMem, 0, MEM_RELEASE);
					if(str == param->sWindowTitle)
					{
						param->lHandleWindow = (long)hwndChild;
						return false;
					}
					else if((Contain) && (param->bFindMethod == false))
					{
						param->lHandleWindow = (long)hwndChild;
						return false;
					}
				}
				else
				{
					param->lHandleWindow = (long)hwndChild;
					return false;
				}
			}
			delete pwi;
		}
		else
		{
			if(param->sWindowTitle != wxEmptyString)
			{
				int cTxtLen = GetWindowTextLength(hwndChild);
				LPWSTR pszMem = (LPWSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE);
				GetWindowText(hwndChild, pszMem, cTxtLen + 1);
				wxString str(pszMem);
				bool Contain = str.Contains(param->sWindowTitle);
				VirtualFree(pszMem, 0, MEM_RELEASE);
				if(str == param->sWindowTitle)
				{
					param->lHandleWindow = (long)hwndChild;
					return false;
				}
				else if((Contain) && (param->bFindMethod == false))
				{
					param->lHandleWindow = (long)hwndChild;
					return false;
				}
			}
		}
	}

	param->lHandleWindow = -1;
	return true;
}
//**************************************************
void CWindowAction::Execute()
{
	HWND hMain;

	switch(m_WinActionType)
	{
	case 0:
		{
			//Получим заголовок окна
			wxString sWindowTitle(m_WindowName);
			wxString sWindowText(m_WindowText);
			wxString sWindowClass(m_WindowClass);
			wxString sHandlePID(m_CriterionPID);
			wxString sHandleWindow(m_WindowHandle);
			
			FIND_PARAM *f_param = NULL;
			f_param = new FIND_PARAM;
			f_param->sWindowTitle = sWindowTitle;
			f_param->sWindowText  = sWindowText;
			f_param->sWindowClass = sWindowClass;
			f_param->bFindMethod  = m_FullCorrespond;
			if(sHandlePID != wxEmptyString)
			{
				long pid; sHandlePID.ToLong(&pid);
				if(wxProcess::Exists(pid))
				{
					f_param->lHandlePID = pid;
				}
				else
				{
					f_param->lHandlePID = -1;
				}
			}
			else
			{
				f_param->lHandlePID = -1;
			}

			long lHandle; sHandleWindow.ToLong(&lHandle);
			HWND handleWin = (HWND)lHandle;
			if(handleWin == NULL)
			{
				EnumChildWindows(NULL, EnumChildProc, (LPARAM)f_param);
			}
			else
			{
				EnumChildWindows(handleWin, EnumChildProc, (LPARAM)f_param);
			}
			
			if(f_param->lHandleWindow != -1)
			{
				hMain = (HWND)f_param->lHandleWindow;
				long lMain = (long)hMain;
				int cTxtLen = GetWindowTextLength(hMain);
				LPWSTR pszMem = (LPWSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE); 
				GetWindowText(hMain, pszMem, cTxtLen + 1);
				DWORD dwPid; GetWindowThreadProcessId(hMain, &dwPid);
				HWND hParent = GetParent(hMain);
				long lParent = (long)hParent;
				PWINDOWINFO pwi = new WINDOWINFO(); 
				
				if(GetWindowInfo(hMain, pwi))
				{
					mWS_ResultClassWindow = wxString::Format(wxT("%d"), pwi->atomWindowType);
					m_ResultClassWindow = mWS_ResultClassWindow.c_str();
				}
				mWS_ResultWindowName = wxString(pszMem).c_str();
				m_ResultWindowName = mWS_ResultWindowName.c_str();
				m_ResultWindowText = mWS_ResultWindowName.c_str();

				mWS_ResultPID = wxString::Format(wxT("%d"), dwPid);
				m_ResultPID = mWS_ResultPID.c_str();

				mWS_ResultHandle = wxString::Format(wxT("%d"), lMain);
				m_ResultHandle = mWS_ResultHandle.c_str();

				mWS_ResultHandleParent = wxString::Format(wxT("%d"), lParent);
				m_ResultHandleParent = mWS_ResultHandleParent.c_str();

				m_ResultFind = wxT("Ok");
				VirtualFree(pszMem, 0, MEM_RELEASE); 
				delete f_param;
				delete pwi;
				return;
			}

			delete f_param;
			m_ResultWindowName = wxEmptyString;
			m_ResultPID = wxEmptyString;
			m_ResultHandle = wxEmptyString;
			m_ResultHandleParent = wxEmptyString;
			m_ResultFind = wxT("Fail");
		}
		break;
	case 1:
		{
			wxString sHandleWindow(m_WindowHandle);
			long lHandle; sHandleWindow.ToLong(&lHandle);
			HWND handleWin = (HWND)lHandle;
			if(handleWin != NULL)
			{
				BringWindowToTop(handleWin);
				switch(m_WindowMessage)
				{
				case 0:
					SendMessage(handleWin, WM_LBUTTONDOWN, NULL, NULL);
					SendMessage(handleWin, WM_LBUTTONUP, NULL, NULL);
					break;
				case 1:
					SendMessage(handleWin, WM_LBUTTONDOWN, NULL, NULL);
					break;
				case 2:
					SendMessage(handleWin, WM_LBUTTONUP, NULL, NULL);
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}
//**************************************************
extern "C" __declspec(dllexport) void GetPluginInfo(struct PluginStartupInfo *Info)
{
	Info->PluginName = wxT("WindowAction.plug");
	Info->ComponentName = wxT("CComponent_WindowAction");
	Info->ComponentGUID = wxT("407501AA-17F9-43ED-964E-868F9C1A5C6C");
	Info->ComponentAutor = wxT("Bruian");
	Info->MajorVers = 0;
	Info->MinorVers = 6;
	Info->ReleaseVers = 11;
	Info->StructSize = sizeof(*Info);
}
//**************************************************
extern "C" __declspec(dllexport) void InitClass(script::VMCore *core)
{
	script::ClassTraits<CWindowAction>::bind(core);
}