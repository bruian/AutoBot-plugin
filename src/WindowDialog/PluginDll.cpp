// PluginDLL is a template DLL which demonstrates how to create
// AutoBot component in a DLL which is called from AutoBot application
//
// Bruian 
// (C) 2008 Bruianio co
// www.bruian.com
//**************************************************
#include "PluginDll.h"

#include <windows.h>

#include "wx/dynlib.h"
#include "wx/window.h"

IMPLEMENT_PLUGIN(CWindowDialog)
//**************************************************
// Class: CComponent
// Name: WindowDialog
// Version: 1.0
// Description: Класс компоненты для работы с диалоговыми окнами ОС
// Component GUID: {B79763EA-0EC4-4EB6-BABF-5BF5BDE05564}
class CWindowDialog
{
private:
	wxString	mWS_DialogName;
	wxString	mWS_DialogMsg;
public:
	wxString		typecoGUID;	//GUID типа компоненты
	//EXTERN member
	int				m_ReturnAnswer;		//m5 - переменная для ответа в окне диалога
	int				m_DialogType;		//m1 - тип выполняемой операции над процессами
	long			m_DialogStyle;		//m2 - стиль диалогового окна
	const xchar*	m_DialogName;		//m3 - заголовок окна
	const xchar*	m_DialogMsg;		//m4 - сообщение диалогового окна

	const xchar*	m_GUIDBot;			//m10 - GUID бота
	const xchar*	m_GUIDRule;			//m11 - GUID правила
public:
	void set_DialogName(const xchar* string) { mWS_DialogName = wxString(string); m_DialogName = mWS_DialogName.c_str(); }
	void set_DialogMsg(const xchar* string) { mWS_DialogMsg = wxString(string); m_DialogMsg = mWS_DialogMsg.c_str(); }
public:
	CWindowDialog();
	void Execute();
};
//**************************************************
namespace script
{
	SCRIPT_CLASS_REGISTER_BEGIN( CWindowDialog )
		.declareClass<CWindowDialog>(wxT("CComponent_WindowDialog"))
			.constructor< objOwn >(wxT("create"))
			.method(wxT("Execute"), &CWindowDialog::Execute)
			.setter(wxT("set_DialogType"), &CWindowDialog::m_DialogType)
			.setter(wxT("set_DialogStyle"), &CWindowDialog::m_DialogStyle)
			.method(wxT("set_DialogName"), &CWindowDialog::set_DialogName)
			.method(wxT("set_DialogMsg"), &CWindowDialog::set_DialogMsg)
			.getter(wxT("get_ReturnAnswer"), &CWindowDialog::m_ReturnAnswer)
			.setter(wxT("set_GUIDBot"), &CWindowDialog::m_GUIDBot)
			.setter(wxT("set_GUIDRule"), &CWindowDialog::m_GUIDRule)
	SCRIPT_CLASS_REGISTER_END( CWindowDialog )
}
//**************************************************
CWindowDialog::CWindowDialog()
{
	typecoGUID = wxT("B79763EA-0EC4-4EB6-BABF-5BF5BDE05564");
	m_DialogType	= 0;
	m_DialogStyle	= 0;
	m_DialogName	= wxEmptyString;
	m_DialogMsg		= wxEmptyString;
	m_ReturnAnswer	= 0;
	m_GUIDBot		= wxEmptyString;
	m_GUIDRule		= wxEmptyString;
}
//**************************************************
void CWindowDialog::Execute()
{
	switch(m_DialogType)
	{
	case 0:	//Окно сообщения
		{
			wxString sDialogName(m_DialogName);
			wxString sDialogMessage(m_DialogMsg);
			int iAnswer = wxMessageBox(sDialogMessage, sDialogName, (long)m_DialogStyle);
			m_ReturnAnswer = iAnswer;
		}
		break;
	case 1:	//Окно ввода
		{
		}
		break;
	default:
		break;
	}
}
//**************************************************
extern "C" __declspec(dllexport) void GetPluginInfo(struct PluginStartupInfo *Info)
{
	Info->PluginName = wxT("WindowDialog.plug");
	Info->ComponentName = wxT("CComponent_WindowDialog");
	Info->ComponentGUID = wxT("B79763EA-0EC4-4EB6-BABF-5BF5BDE05564");
	Info->ComponentAutor = wxT("Bruian");
	Info->MajorVers = 0;
	Info->MinorVers = 6;
	Info->ReleaseVers = 11;
	Info->StructSize = sizeof(*Info);
}
//**************************************************
extern "C" __declspec(dllexport) void InitClass(script::VMCore *core)
{
	script::ClassTraits<CWindowDialog>::bind(core);
}