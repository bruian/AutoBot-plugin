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

#include "wx/timer.h"
#include "wx/dir.h"
#include "wx/filename.h"

#include "wx/dynlib.h"
#include "wx/window.h"
#include "wx/msgdlg.h"
#include "wx/dialog.h"
#include "wx/button.h"

//#include "../plugin.h"
//**************************************************
//Information of plugin setting
//static struct PluginStartupInfo Infos;
IMPLEMENT_PLUGIN(MyPlugin)
//**************************************************
// Class: CComponent_FileAction
// Name: FileAction
// Version: 0.5.10
// Description: Component implementation. Class for work
// with processes and application OS.
// Component type GUID: {64A880E6-3651-4083-85C5-BD2ECB94B862}
class CFileAction
{
private:
	//INNER member - not using in script, but doubled and contain information from EXTERN member/
	//Very important, this member needed for (const xchar*) member.
	wxString	mWS_FileFrom;
	wxString	mWS_FileTo;
	wxString	mWS_ReturnResult;
public:
	wxString typecoGUID;	//GUID component type

	//EXTERN member - using in script
	int				m_FileOperations;	//m3 - Operation with files
	bool			m_FileRewrite;		//m4 - Overwrite the file if you copy. True - Yes.
	const xchar*	m_FileFrom;			//m5 - From - copy, move, delete...
	const xchar*	m_FileTo;			//m6 - To, Destination
	const xchar*	m_ReturnResult;		//m7 - Operation result
	
	const xchar*	m_GUIDBot;		//m10 - GUID bot
	const xchar*	m_GUIDRule;		//m11 - GUID rule (rule - object, component - class)
public:
	//Setter string values
	void set_FileFrom(const xchar* string) { mWS_FileFrom = wxString(string); m_FileFrom = mWS_FileFrom.c_str(); }
	void set_FileTo(const xchar* string) { mWS_FileTo = wxString(string); m_FileTo = mWS_FileTo.c_str(); }
public:
	//Constructor
	CFileAction();
	//This function execute functional components
	void Execute();
};
//**************************************************
//Auxiliary class for the realization of class components
class wxDirTraverserSimple : public wxDirTraverser
{
public:
	wxDirTraverserSimple(wxArrayString& files, wxArrayString& dirs) : m_files(files), m_dirs(dirs) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename)
	{
		m_files.Add(filename);
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& dirname)
	{
		m_dirs.Add(dirname);
		return wxDIR_CONTINUE;
	}
private:
	wxArrayString& m_files;
	wxArrayString& m_dirs;
};
//**************************************************
class wxDirTraverserFind : public wxDirTraverser
{
public:
	wxDirTraverserFind(wxArrayString& files) : m_files(files) { }
	virtual wxDirTraverseResult OnFile(const wxString& filename)
	{
		m_files.Add(filename);
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& dirname)
	{
		m_files.Add(dirname);
		return wxDIR_CONTINUE;
	}
private:
	wxArrayString& m_files;
};
//**************************************************
//This bind component class with AutoBot Squirrel core
//always use this namespace for such declarations. It is good practice.
namespace script
{
	SCRIPT_CLASS_REGISTER_BEGIN( CFileAction )
		.declareClass<CFileAction>(wxT("CComponent_FileAction"))
			.constructor< objOwn >(wxT("create"))
			.method(wxT("Execute"), &CFileAction::Execute)
			.setter(wxT("set_FileOperations"), &CFileAction::m_FileOperations)
			.setter(wxT("set_FileRewrite"), &CFileAction::m_FileRewrite)
			.method(wxT("set_FileFrom"), &CFileAction::set_FileFrom)
			.method(wxT("set_FileTo"), &CFileAction::set_FileTo)
			.getter(wxT("get_ReturnResult"), &CFileAction::m_ReturnResult)
			.setter(wxT("set_GUIDBot"), &CFileAction::m_GUIDBot)
			.setter(wxT("set_GUIDRule"), &CFileAction::m_GUIDRule)
	SCRIPT_CLASS_REGISTER_END( CFileAction )
}
//**************************************************
//Auxiliary function for the realization of class components
static int CompareStringLen(const wxString& first, const wxString& second)
{
	return first.length() - second.length();
}
//**************************************************
bool CopyDir(wxString From, wxString To, bool overwrite, int fo)
{
	wxArrayString files;
	wxArrayString dirs;
	wxDirTraverserSimple traverser(files, dirs);

	wxDir dir(From);
	dir.Traverse(traverser);
	bool results = true;
	//dirs.Sort(CompareStringLen);

	wxString s, dirTo, fileTo;
	size_t count = dirs.GetCount();
	for(size_t n = 0; n < count; n++)
	{
		//s.Printf(dirs[n].c_str());
		s = dirs[n];
		s.Remove(0,From.Len());
		dirTo = To + s;
		if(!wxDirExists(dirTo))
		{
			if(!wxMkdir(dirTo))
				results = false;
		}
	}

	count = files.GetCount();
	for(size_t n = 0; n < count; n++)
	{
		s = files[n];
		s.Remove(0, From.Len());
		fileTo = To + s;
		if(!wxCopyFile(files[n], fileTo, overwrite))
			results = false;
		if(fo == 1)
		{
			if(!wxRemoveFile(files[n]))
				results = false;
		}
	}

	if(fo == 1)
	{
		count = dirs.GetCount();
		for (size_t n = count; n > 0; n--)
		{
			if(!wxRmdir(dirs[n-1]))
				results = false;
		}
	}

	return results;
}
//**************************************************
//Realiz function of Component
CFileAction::CFileAction()
{
	m_FileOperations = 0;
	m_FileRewrite	 = false;
	m_FileFrom		 = wxEmptyString;
	m_FileTo		 = wxEmptyString;
	m_ReturnResult	 = wxEmptyString;
}
//**************************************************
void CFileAction::Execute()
{
	m_ReturnResult = wxT("FAIL");
	wxString sFrom(m_FileFrom);
	wxString sTo(m_FileTo);
	if(sFrom == wxEmptyString)
		return;

	wxString sFromDir; wxString sFromFile; wxString sFromExt;
	wxFileName::SplitPath(sFrom, &sFromDir, &sFromFile, &sFromExt);
	if(sFromExt == wxEmptyString)		//Проверим если нет расширения файла, то будем считать что это каталог
		if(sFrom.Right(1) != wxT("\\"))	//Если нет на конце косой черты, то добавим
			sFrom += wxT("\\"); 

	switch(m_FileOperations)
	{
	case 0:
	case 1: //Операция копирования(0) или перемещения(1) файлов
		{
			if(sTo == wxEmptyString)	//Если копировать некуда, то выходим
				return;

			wxString sToDir; wxString sToFile; wxString sToExt;
			wxFileName::SplitPath(sTo, &sToDir, &sToFile, &sToExt);
			if(sTo.Right(1) != wxT("\\"))	//Если нет на конце косой черты, то добавим
				sTo += wxT("\\"); 

			if(sFromExt == wxEmptyString)
			{
				if(!CopyDir(sFrom, sTo, m_FileRewrite, m_FileOperations))
					return;
				if(m_FileOperations == 1)
				{
					if(!wxRmdir(sFrom.RemoveLast(1)))
						return;
				}
			}
			else
			{
				if(sToExt == wxEmptyString)
				{
					sTo += sFromFile + wxT(".") + sFromExt;
				}

				if(!wxCopyFile(sFrom, sTo, m_FileRewrite))
					return;

				if(m_FileOperations == 1)
				{
					if(!wxRemoveFile(sFrom))
						return;
				}
			}

			m_ReturnResult = wxT("OK");
		}
		break;
	case 2:	//Операция удаления(2)
		{
			if(sFromExt == wxEmptyString)
			{
				if(!wxRmdir(sFrom.RemoveLast(1)))
					return;
			}
			else
			{
				if(!wxRemoveFile(sFrom))
					return;
			}

			m_ReturnResult = wxT("OK");
		}
		break;
	case 3: //Операция создания(3)
		{
			if(sFromExt == wxEmptyString)
			{
				if(!wxMkdir(sFrom.RemoveLast(1)))
					return;
			}
			
			m_ReturnResult = wxT("OK");
		}
		break;
	case 4: //Операция проверки на существование(4)
		{
			if(sFromExt == wxEmptyString)
			{
				if(!wxDirExists(sFrom.RemoveLast(1)))
					m_ReturnResult = wxT("OK");
			}
			else
			{
				if(wxFileExists(sFrom))
					m_ReturnResult = wxT("OK");
			}
		}
		break;
	case 5:	//Операция поиска
	//	//{
	//	//	m_PosFind = 0;
	//	//	if(m_FileFind.GetCount() > 0)
	//	//		m_FileFind.Clear();

	//	//	wxString sDir;
	//	//	wxFileName::SplitPath(sFrom, &sDir, &m_FileMask, &m_ExtMask);

	//	//	wxDirTraverserFind traverser(m_FileFind);
	//	//	wxDir dir(wxPathOnly(sFrom));
	//	//	if(dir.Traverse(traverser) == 0)
	//	//		return false;
	//	//	if(m_PosFind < m_FileFind.GetCount())
	//	//	{
	//	//		if((m_FileMask == wxT("*")) && (m_ExtMask == wxT("*")))
	//	//		{
	//	//			bot->SetValueFromMember(GetTo(), m_FileFind[m_PosFind]);
	//	//			bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//	//			m_PosFind++;
	//	//		}
	//	//		else if(m_ExtMask == wxT("*"))
	//	//		{
	//	//			wxString sFile;
	//	//			wxString sExt;
	//	//			while(true)
	//	//			{
	//	//				if(m_PosFind >= m_FileFind.GetCount())
	//	//					break;
	//	//				wxFileName::SplitPath(m_FileFind[m_PosFind], &sDir, &sFile, &sExt);
	//	//				if(sFile == m_FileMask)
	//	//				{
	//	//					bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//	//					bot->SetValueFromMember(GetTo(), m_FileFind[m_PosFind]);
	//	//					m_PosFind++;
	//	//					break;
	//	//				}
	//	//				m_PosFind++;
	//	//			}
	//	//		}
	//	//		else if(m_FileMask == wxT("*"))
	//	//		{
	//	//			wxString sFile;
	//	//			wxString sExt;
	//	//			while(true)
	//	//			{
	//	//				if(m_PosFind >= m_FileFind.GetCount())
	//	//					break;
	//	//				wxFileName::SplitPath(m_FileFind[m_PosFind], &sDir, &sFile, &sExt);
	//	//				if(sExt == m_ExtMask)
	//	//				{
	//	//					bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//	//					bot->SetValueFromMember(GetTo(), m_FileFind[m_PosFind]);
	//	//					m_PosFind++;
	//	//					break;
	//	//				}
	//	//				m_PosFind++;
	//	//			}
	//	//		}
	//	//	}
	//	//}
		break;
	case 6: //Операция найти следующий
		break;
	case 7: //Операция переименования
		{
			wxString sToDir;
			if(wxPathOnly(sTo) == wxEmptyString)
			{
				sToDir += wxT("\\")+sTo;
				sTo = sToDir;
			}
			if(!wxRenameFile(sFrom, sTo, m_FileRewrite))
				return;

			m_ReturnResult = wxT("OK");
		}
		break;
	}
}
//**************************************************
void MyPlugin::PerformTasks()
{
	//wxMessageBox("I would if I could...");
}
//**************************************************
wxChar* MyPlugin::GetName()
{
	return wxT("The other one");
}
//**************************************************
void MyPlugin::GetPluginInfo(struct PluginStartupInfo *Info)
{
	Info->PluginName = wxT("Plugin.plug");
	Info->ComponentName = wxT("CComponent_FileAction");
	Info->ComponentGUID = wxT("64A880E6-3651-4083-85C5-BD2ECB94B862");
	Info->ComponentAutor = wxT("Bruian");
	Info->MajorVers = 0;
	Info->MinorVers = 5;
	Info->ReleaseVers = 10;
	Info->StructSize = sizeof(*Info);
}
//**************************************************
void MyPlugin::InitClass(script::VMCore *core)
{
	script::ClassTraits<CFileAction>::bind(core);
}