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
#include <atlconv.h>
#include "wx/dynlib.h"
#include "wx/window.h"
#include "wx/process.h"
//**************************************************
//Information of plugin setting
IMPLEMENT_PLUGIN(CProcessAction)
//**************************************************
// Class: CComponent
// Name: ProcessAction
// Version: 1.0
// Description: Класс компоненты для работы с процессами
//	и приложениями ОС.
// Component GUID: {3A7C5246-78D2-4399-BE7F-644CEB4473AE}
class CProcessAction
{
private:
	wxString	mWS_ProgramName;
	wxString	mWS_CriterionPID;
	wxString	mWS_ProcessName;
	wxString	mWS_ReturnPID;
	wxString	mWS_ResultOperation;
	wxString	mWS_HandleTopWindow;
public:
	wxString typecoGUID;	//GUID типа компоненты

	//EXTERN member
	int				m_ProcessType;		//m1 - тип выполняемой операции над процессами
	const xchar*	m_ProgramName;		//m2 - критерий, Путь к исполняемому файлу процесса
	bool			m_WaitEndProcess;	//m3 - критерий, Ожидание завершения работы процесса
	const xchar*	m_ReturnPID;		//m4 - результат, Process ID
	const xchar*	m_CriterionPID;		//m5 - критерий, Process ID
	int				m_MethodEndProcess;	//m6 - критерий, метод завершения работы процесса
	const xchar*	m_ResultOperation;	//m7 - результат, результат выполнения операции над процессом
	const xchar*	m_ProcessName;		//m8 - критерий, наименование процесса
	const xchar*	m_HandleTopWindow;	//m9 - Результат, хэндл главного окна приложения
	const xchar*	m_GUIDBot;			//m10 - GUID бота
	const xchar*	m_GUIDRule;			//m11 - GUID правила
public:
	void set_ProgramName(const xchar* string) { mWS_ProgramName = wxString(string); m_ProgramName = mWS_ProgramName.c_str(); }
	void set_CriterionPID(const xchar* string) { mWS_CriterionPID = wxString(string); m_CriterionPID = mWS_CriterionPID.c_str(); }
	void set_ProcessName(const xchar* string) { mWS_ProcessName = wxString(string); m_ProcessName = mWS_ProcessName.c_str(); }
public:
	CProcessAction();
	void Execute();
};
//**************************************************
//This bind component class with AutoBot Squirrel core
//always use this namespace for such declarations. It is good practice.
namespace script
{
	SCRIPT_CLASS_REGISTER_BEGIN( CProcessAction )
		.declareClass<CProcessAction>(wxT("CComponent_ProcessAction"))
			.constructor< objOwn >(wxT("create"))
			.method(wxT("Execute"), &CProcessAction::Execute)
			.setter(wxT("set_ProcessType"), &CProcessAction::m_ProcessType)
			.method(wxT("set_ProgramName"), &CProcessAction::set_ProgramName)
			.setter(wxT("set_WaitEndProcess"), &CProcessAction::m_WaitEndProcess)
			.setter(wxT("set_ReturnPID"), &CProcessAction::m_ReturnPID)
			.method(wxT("set_CriterionPID"), &CProcessAction::set_CriterionPID)
			.setter(wxT("set_MethodEndProcess"), &CProcessAction::m_MethodEndProcess)
			.setter(wxT("set_ResultOperation"), &CProcessAction::m_ResultOperation)
			.method(wxT("set_ProcessName"), &CProcessAction::set_ProcessName)
			.setter(wxT("set_HandleTopWindow"), &CProcessAction::m_HandleTopWindow)
			.getter(wxT("get_ReturnPID"), &CProcessAction::m_ReturnPID)
			.getter(wxT("get_ResultOperation"), &CProcessAction::m_ResultOperation)
			.setter(wxT("set_GUIDBot"), &CProcessAction::m_GUIDBot)
			.setter(wxT("set_GUIDRule"), &CProcessAction::m_GUIDRule)
	SCRIPT_CLASS_REGISTER_END( CProcessAction )
}
//**************************************************
CProcessAction::CProcessAction()
{
	typecoGUID = wxT("3A7C5246-78D2-4399-BE7F-644CEB4473AE");
	m_ProcessType		= 0;
	m_ProgramName		= wxEmptyString;
	m_WaitEndProcess	= false;
	m_ReturnPID			= wxEmptyString;
	m_CriterionPID		= wxEmptyString;
	m_MethodEndProcess	= 0;
	m_ResultOperation	= wxEmptyString;
	m_ProcessName		= wxEmptyString;
	m_HandleTopWindow	= wxEmptyString;
	m_GUIDBot			= wxEmptyString;
	m_GUIDRule			= wxEmptyString;
}
//**************************************************
void CProcessAction::Execute()
{
	//CError *er = wxGetApp().GetError();
	//CAppProperties *ap = wxGetApp().GetAppProperties();
	switch(m_ProcessType)
	{
	case 0:	//Execute process
		{
			wxString sProgramName(m_ProgramName);
			if(sProgramName == wxEmptyString)
			{
				m_ResultOperation = wxT("Fail");
				//er->AddMessage(ap->GetLngRes(wxT("lr_ProcessAction_EmptyPrName")), CError::eMsgType_RuleErr, wxString((const wchar_t*)m_GUIDBot), wxString((const wchar_t*)m_GUIDRule));
				return;
			}

			STARTUPINFOW si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			USES_CONVERSION;
			//LPWSTR szCmdline = T2W(sProgramName);
			LPWSTR szCmdline = (LPWSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (sProgramName.Length()), MEM_COMMIT, PAGE_READWRITE); 
			memcpy(szCmdline, sProgramName.wc_str(), sProgramName.Length() * sizeof(wxChar));
			//LPWSTR szCmdline = sProgramName.c_str();
			if(!CreateProcessW(NULL,   // No module name (use command line)
				szCmdline,      // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi)           // Pointer to PROCESS_INFORMATION structure
			)
			{
				VirtualFree(szCmdline, 0, MEM_RELEASE); 
				m_ResultOperation = wxT("Fail");
				return;
			}
			VirtualFree(szCmdline, 0, MEM_RELEASE); 
			
			if(m_WaitEndProcess)
			{
				WaitForSingleObjectEx(pi.hProcess, INFINITE, TRUE);
			}
			
			mWS_ReturnPID = wxString::Format(wxT("%d"), pi.dwProcessId);
			m_ReturnPID = mWS_ReturnPID.c_str();
			
			m_ResultOperation = wxT("Ok");
			
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		break;
	case 1:	//Kill process
		{
			wxString sCPID(m_CriterionPID);
			if(sCPID.Length() == 0)
			{
				//er->AddMessage(ap->GetLngRes(wxT("lr_ProcessAction_EmptyPid")), CError::eMsgType_RuleErr, wxString((const wchar_t*)m_GUIDBot), wxString((const wchar_t*)m_GUIDRule));
				m_ResultOperation = wxT("Fail");
				return;
			}
			
			long pid; sCPID.ToLong(&pid);
			if(wxProcess::Exists(pid))
			{
				switch(m_MethodEndProcess)
				{
				case 0:
					wxKill(pid, wxSIGQUIT);
					break;
				case 1:
					wxKill(pid, wxSIGKILL);
					break;
				}
				m_ResultOperation = wxT("Ok");
			}
			else
			{
				m_ResultOperation = wxT("Fail");
				//er->AddMessage(ap->GetLngRes(wxT("lr_ProcessAction_EmptyPid")), CError::eMsgType_RuleErr, wxString((const wchar_t*)m_GUIDBot), wxString((const wchar_t*)m_GUIDRule));
			}
		}
		break;
	case 2:	//Find handle on process
		{
			wxString sCPID(m_CriterionPID);
			if(sCPID.Length() == 0)
			{
				m_ResultOperation = wxT("Fail");
				//er->AddMessage(ap->GetLngRes(wxT("lr_ProcessAction_EmptyPid")), CError::eMsgType_RuleErr, wxString((const wchar_t*)m_GUIDBot), wxString((const wchar_t*)m_GUIDRule));
				return;
			}
			
			long pid; sCPID.ToLong(&pid);
			if(wxProcess::Exists(pid))
			{
				HWND hMain = GetTopWindow(NULL);
				HWND eNumWin = GetWindow(hMain, GW_HWNDNEXT);
				while(eNumWin != NULL)
				{
					DWORD dwPid; GetWindowThreadProcessId(eNumWin, &dwPid);
					if(dwPid == (DWORD)pid)
					{
						long lMain = (long)eNumWin;
						mWS_HandleTopWindow = wxString::Format(wxT("%d"), lMain);
						m_HandleTopWindow = mWS_HandleTopWindow.c_str();
						m_ResultOperation = wxT("Ok");
						return;
					}
					eNumWin = GetWindow(eNumWin, GW_HWNDNEXT);
				}
				m_ResultOperation = wxT("Fail");
			}
			else
			{
				m_ResultOperation = wxT("Fail");
				//er->AddMessage(ap->GetLngRes(wxT("lr_ProcessAction_EmptyPid")), CError::eMsgType_RuleErr, wxString((const wchar_t*)m_GUIDBot), wxString((const wchar_t*)m_GUIDRule));
			}
		}
		break;
	default: //Nothing
		break;
	}
}
extern "C" __declspec(dllexport) void GetPluginInfo(struct PluginStartupInfo *Info)
{
	Info->PluginName = wxT("ProcessAction.plug");
	Info->ComponentName = wxT("CComponent_ProcessAction");
	Info->ComponentGUID = wxT("3A7C5246-78D2-4399-BE7F-644CEB4473AE");
	Info->ComponentAutor = wxT("Bruian");
	Info->MajorVers = 0;
	Info->MinorVers = 6;
	Info->ReleaseVers = 11;
	Info->StructSize = sizeof(*Info);
}
extern "C" __declspec(dllexport) void InitClass(script::VMCore *core)
{
	script::ClassTraits<CProcessAction>::bind(core);
}