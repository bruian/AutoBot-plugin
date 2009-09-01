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

#include "Mail.h"
//**************************************************
//Information of plugin setting
//static struct PluginStartupInfo Infos;
IMPLEMENT_PLUGIN(abPlugin)
//**************************************************
enum eMailOperations
{
	eMailOperations_FindLetter = 0,
	eMailOperations_GetLetter,
	eMailOperations_SendLetter,
	eMailOperations_DeleteLetter,
	eMailOperations_Max
};
//**************************************************
// Class: CComponent
// Name: MailAction
// Version: 1.0
// Description: Класс компоненты для работы с процессами
//	и приложениями ОС.
// Component GUID: {1CDFCB1A-4A95-45fe-8FA4-ACF04D3A6024}
class CMailAction
{
public:
	wxString typecoGUID;	//GUID типа компоненты

	//EXTERN member
	int					m_ProcessType;		//m1 - тип выполняемой операции над процессами
	wxString			m_Description;		//m10 - Описание действия компоненты
	wxString			m_GUID;				//m11 - GUID компоненты
public:
	CMailAction()
	{
		typecoGUID = wxT("1CDFCB1A-4A95-45fe-8FA4-ACF04D3A6024");
	};

	void Execute();
};
//**************************************************
//class CMailAction
//{
//private:
//	eMailOperations m_MailOperations;
//	wxString m_OperationResult;
//	wxString m_MailServer;
//	wxString m_MailPort;
//	wxString m_MailLogin;
//	wxString m_MailPassword;
//	wxString m_EMailTo;
//	wxString m_EMailFrom;
//	wxString m_MailTheme;
//	wxString m_MailText;
//	wxString m_MailAttach;
//	wxString m_MailId;
//	wxString m_REMailFrom;
//	wxString m_RMailTheme;
//	//wxString m_DateLetter;
//public:
//	CMailAction();
//
//	eMailOperations GetMailOperations() { return m_MailOperations; }
//	void SetMailOperations(eMailOperations value) { m_MailOperations = value; }
//	wxString GetOperationResult() { return m_OperationResult; }
//	void SetOperationResult(wxString value) { m_OperationResult = value; }
//	wxString GetMailServer() { return m_MailServer; }
//	void SetMailServer(wxString value) { m_MailServer = value; }
//	wxString GetMailPort() { return m_MailPort; }
//	void SetMailPort(wxString value) { m_MailPort = value; }
//	wxString GetMailLogin() { return m_MailLogin; }
//	void SetMailLogin(wxString value) { m_MailLogin = value; }
//	wxString GetMailPassword() { return m_MailPassword; }
//	void SetMailPassword(wxString value) { m_MailPassword = value; }
//	wxString GetEMailTo() { return m_EMailTo; }
//	void SetEMailTo(wxString value) { m_EMailTo = value; }
//	wxString GetEMailFrom() { return m_EMailFrom; }
//	void SetEMailFrom(wxString value) { m_EMailFrom = value; }
//	wxString GetMailTheme() { return m_MailTheme; }
//	void SetMailTheme(wxString value) { m_MailTheme = value; }
//	wxString GetMailText() { return m_MailText; }
//	void SetMailText(wxString value) { m_MailText = value; }
//	wxString GetMailAttach() { return m_MailAttach; }
//	void SetMailAttach(wxString value) { m_MailAttach = value; }
//	wxString GetMailId() { return m_MailId; }
//	void SetMailId(wxString value) { m_MailId = value; }
//	wxString GetREMailFrom() { return m_REMailFrom; }
//	void SetREMailFrom(wxString value) { m_REMailFrom = value; }
//	wxString GetRMailTheme() { return m_RMailTheme; }
//	void SetRMailTheme(wxString value) { m_RMailTheme = value; }
//
//	wxString PackValuesToString();
//	void UnPackValuesFromString(wxString value);
//
//	bool Execute(CBot *bot);
//};
//**************************************************
//CMailAction::CMailAction()
//{
//	m_MailOperations	= eMailOperations_FindLetter;
//	m_OperationResult	= wxEmptyString;
//	m_MailServer		= wxEmptyString;
//	m_MailPort			= wxEmptyString;
//	m_MailLogin			= wxEmptyString;
//	m_MailPassword		= wxEmptyString;
//	m_EMailTo			= wxEmptyString;
//	m_EMailFrom			= wxEmptyString;
//	m_MailTheme			= wxEmptyString;
//	m_MailText			= wxEmptyString;
//	m_MailAttach		= wxEmptyString;
//	m_MailId			= wxEmptyString;
//	m_REMailFrom		= wxEmptyString;
//	m_RMailTheme		= wxEmptyString;
//}
////**************************************************
//wxString CMailAction::PackValuesToString()
//{
//	wxString retString = wxEmptyString;
//	retString += wxString::Format(wxT("m1:%d"), GetMailOperations());
//	if(GetOperationResult() == wxEmptyString) retString += wxT("m2:NULL");
//		else retString += wxT("m2:") + GetOperationResult();
//	if(GetMailServer() == wxEmptyString) retString += wxT("m3:NULL");
//		else retString += wxT("m3:") + GetMailServer();
//	if(GetMailPort() == wxEmptyString) retString += wxT("m4:NULL");
//		else retString += wxT("m4:") + GetMailPort();
//	if(GetMailLogin() == wxEmptyString) retString += wxT("m5:NULL");
//		else retString += wxT("m5:") + GetMailLogin();
//	if(GetMailPassword() == wxEmptyString) retString += wxT("m6:NULL");
//		else retString += wxT("m6:") + GetMailPassword();
//	if(GetEMailTo() == wxEmptyString) retString += wxT("m7:NULL");
//		else retString += wxT("m7:") + GetEMailTo();
//	if(GetEMailFrom() == wxEmptyString) retString += wxT("m8:NULL");
//		else retString += wxT("m8:") + GetEMailFrom();
//	if(GetMailTheme() == wxEmptyString) retString += wxT("m9:NULL");
//		else retString += wxT("m9:") + GetMailTheme();
//	if(GetMailText() == wxEmptyString) retString += wxT("m10:NULL");
//		else retString += wxT("m10:") + GetMailText();
//	if(GetMailAttach() == wxEmptyString) retString += wxT("m11:NULL");
//		else retString += wxT("m11:") + GetMailAttach();
//	if(GetMailId() == wxEmptyString) retString += wxT("m12:NULL");
//		else retString += wxT("m12:") + GetMailId();
//	if(GetREMailFrom() == wxEmptyString) retString += wxT("m13:NULL");
//		else retString += wxT("m13:") + GetREMailFrom();
//	if(GetRMailTheme() == wxEmptyString) retString += wxT("m14:NULL");
//		else retString += wxT("m14:") + GetRMailTheme();
//
//	return retString;
//}
////**************************************************
//void CMailAction::UnPackValuesFromString(wxString value)
//{
//	size_t startpos = 0;
//	size_t endpos = 0;
//	wxString valStr;
//	for(int i = 1; i < 15; i++)
//	{
//		size_t len = wxString::Format(wxT("m%d:"),i).Length();
//		startpos = value.find(wxString::Format(wxT("m%d:"),i), startpos);
//		if(startpos > value.Len())
//			break;
//		endpos	 = value.find(wxString::Format(wxT("m%d:"),i+1), startpos);
//		valStr	 = value.Mid(startpos+len, endpos-startpos-len);
//		if(valStr == wxT("NULL"))
//			valStr = wxEmptyString;
//
//		switch(i)
//		{
//		case 1:
//			{
//				ULONG val = 0; valStr.ToULong(&val);
//				SetMailOperations(eMailOperations((int)val));
//			}
//			break;
//		case 2:
//			SetOperationResult(valStr);
//			break;
//		case 3:
//			SetMailServer(valStr);
//			break;
//		case 4:
//			SetMailPort(valStr);
//			break;
//		case 5:
//			SetMailLogin(valStr);
//			break;
//		case 6:
//			SetMailPassword(valStr);
//			break;
//		case 7:
//			SetEMailTo(valStr);
//			break;
//		case 8:
//			SetEMailFrom(valStr);
//			break;
//		case 9:
//			SetMailTheme(valStr);
//			break;
//		case 10:
//			SetMailText(valStr);
//			break;
//		case 11:
//			SetMailAttach(valStr);
//			break;
//		case 12:
//			SetMailId(valStr);
//			break;
//		case 13:
//			SetREMailFrom(valStr);
//			break;
//		case 14:
//			SetRMailTheme(valStr);
//			break;
//		}
//	}
//}
//**************************************************
void CMailAction::Execute()
{
	////Заранее установим результат действия false
	//bot->SetValueFromMember(GetOperationResult(), wxT("false"));
	////Получаем параметры сеанса почты, обязательно: адрес сервер, порт сервера, имя пользователя и пароль
	//wxString sAddress	= bot->GetValueFromMember(GetMailServer());
	//wxString sMailPort	= bot->GetValueFromMember(GetMailPort());
	//wxString sLogin		= bot->GetValueFromMember(GetMailLogin());
	//wxString sPassword	= bot->GetValueFromMember(GetMailPassword());

	////Проверим все ли данные доступны
	//if((sAddress == wxEmptyString) || (sLogin == wxEmptyString))
	//	return false;

	//double port = 110;	//По умолчанию зададим порт 110
	//if(sMailPort != wxEmptyString)
	//	sMailPort.ToDouble(&port);	//Если значение не пустое, то переопределим порт.

	//switch(GetMailOperations())//Выбор исполняемой операции
	//{
	//	//Поиск письма по критериям
	//case eMailOperations_FindLetter:
	//	{
	//		//Критерий адреса отправки
	//		wxString sFrom = bot->GetValueFromMember(GetEMailFrom());

	//		wxPop *pop = new wxPop();
	//		if(pop->popConnect(sAddress, sMailPort))
	//		{
	//			if(pop->popLogin(sLogin, sPassword))
	//			{
	//				int iLetterCount = pop->popLetterCount();
	//				for(int i = 1; i < iLetterCount+1; i++)
	//				{
	//					if(!pop->popGetLetterFromServer(i, true))
	//						break;
	//					wxMimeLetter *popLetter = pop->popGetLetter();
	//					//int iCountParts = pop->popGetLetter()->CountParts();
	//					//wxString str = pop->popGetLetter()->GetPart(0)->GetEncodingData();
	//					//wxString str2 = pop->popGetLetter()->GetPart(1)->GetEncodingData();
	//					//wxString str3 = pop->popGetLetter()->GetPart(1)->GetEncodingString();
	//					//wxString path = pop->popGetLetter()->GetPart(1)->SaveFileTo(wxT("D:\\Temp\\urbd\\"));
	//					
	//					//Если критерий адреса отправки задан то пытаемся найти письма с этим адресом
	//					if(sFrom != wxEmptyString)
	//					{
	//						//Получаем из письма отправителя и сверяем с критерием
	//						wxString str(popLetter->GetSender());
	//						if(str.Contains(sFrom))
	//						{
	//							//Отправитель совпал, проверим второй критерий "Тема письма"
	//							wxString sMailTheme = bot->GetValueFromMember(GetMailTheme());
	//							if(sMailTheme != wxEmptyString)
	//							{
	//								//Получаем из письма тему и сверяем с критерием
	//								wxString subject(popLetter->GetSubject());
	//								if(subject.Contains(sMailTheme))
	//								{
	//									//Тема и отправитель совпали. Смело заносим результат в переменные ответа.
	//									bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//									bot->SetValueFromMember(GetMailId(), wxString::Format(wxT("%d"), i));
	//									bot->SetValueFromMember(GetREMailFrom(), wxString(popLetter->GetSender()));
	//									bot->SetValueFromMember(GetRMailTheme(), wxString(popLetter->GetSubject()));
	//									break;
	//								}
	//							}
	//							else
	//							{
	//								//Отправитель совпал. По теме не искали. Заносим результат в переменные ответа.
	//								bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//								bot->SetValueFromMember(GetMailId(), wxString::Format(wxT("%d"), i));
	//								bot->SetValueFromMember(GetREMailFrom(), wxString(popLetter->GetSender()));
	//								bot->SetValueFromMember(GetRMailTheme(), wxString(popLetter->GetSubject()));
	//								break;
	//							}
	//						}
	//					}
	//					else
	//					{
	//						//Отправителя не задавали. Попробуем поиск по теме письма.
	//						wxString sMailTheme = bot->GetValueFromMember(GetMailTheme());
	//						if(sMailTheme != wxEmptyString)
	//						{
	//							wxString subject(popLetter->GetSubject());
	//							if(subject.Contains(sMailTheme))
	//							{
	//								//Тема совпала. По отправителю не искали. Заносим результат в переменные ответа.
	//								bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//								bot->SetValueFromMember(GetMailId(), wxString::Format(wxT("%d"), i));
	//								bot->SetValueFromMember(GetREMailFrom(), wxString(popLetter->GetSender()));
	//								bot->SetValueFromMember(GetRMailTheme(), wxString(popLetter->GetSubject()));
	//								break;
	//							}
	//						}
	//						else
	//						{
	//							//По теме не искали. По отправителю не искали. Заносим результат в переменные ответа.
	//							bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//							bot->SetValueFromMember(GetMailId(), wxString::Format(wxT("%d"), i));
	//							bot->SetValueFromMember(GetREMailFrom(), wxString(popLetter->GetSender()));
	//							bot->SetValueFromMember(GetRMailTheme(), wxString(popLetter->GetSubject()));
	//							break;
	//						}
	//					}
	//				}
	//			}
	//		}
	//		delete pop;
	//	}
	//	break;
	//	//Получение письма по ID
	//case eMailOperations_GetLetter:
	//	{
	//		//Получим и проверим ID письма
	//		bot->SetValueFromMember(GetOperationResult(), wxT("false"));
	//		wxString sLetterID = bot->GetValueFromMember(GetMailId());
	//		if(sLetterID == wxEmptyString)
	//			return false;
	//		double id = 0;
	//		sLetterID.ToDouble(&id);
	//		if(id == 0)
	//			return false;

	//		//Попытаемся залогиниться
	//		wxPop *pop = new wxPop();
	//		if(pop->popConnect(sAddress, sMailPort))
	//		{
	//			if(pop->popLogin(sLogin, sPassword))
	//			{
	//				//Получим каталог сохранения письма
	//				wxString path, name, ext;
	//				wxSplitPath(bot->GetValueFromMember(GetMailAttach()), &path, &name, &ext);
	//				if(ext.IsEmpty())
	//					if(path.Right(1) == wxT("\\"))
	//						path += name;
	//					else
	//						path += (wxT("\\") + name);
	//				//Попробуем получить письмо из ящика
	//				if(!pop->popGetLetterFromServer(id, false))
	//					break;
	//				wxMimeLetter *popLetter = pop->popGetLetter();
	//				if(popLetter != NULL)
	//				{
	//					int iCountParts = pop->popGetLetter()->CountParts();
	//					for(int i = 0; i < iCountParts; i++)
	//					{
	//						wxString pathOut = popLetter->GetPart(i)->SaveFileTo(path);
	//					}
	//					bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//				}
	//			}
	//		}
	//		delete pop;
	//	}
	//	break;
	//	//Операция отправки письма
	//case eMailOperations_SendLetter:
	//	{
	//		//Получим и проверим мыло получателя
	//		wxString sEMailTo    = bot->GetValueFromMember(GetEMailTo());
	//		wxString sEMailFrom	 = bot->GetValueFromMember(GetEMailFrom());
	//		if((sEMailTo == wxEmptyString) || (sEMailFrom == wxEmptyString))
	//			return false;
	//		wxString sAttachFile = bot->GetValueFromMember(GetMailAttach());
	//		wxString sTheme		 = bot->GetValueFromMember(GetMailTheme());

	//		wxSMTP m_smtp;
	//		if(m_smtp.Connect(sAddress, sLogin, sMailPort))
	//		{
	//			m_smtp.Mail(sEMailFrom);
	//			m_smtp.SetTo(sEMailTo);

	//			wxMimeLetter *mimeLetter = new wxMimeLetter;
	//			wxMimePart *mimePart = new wxMimePart();
	//			mimePart->AppendFile(sAttachFile);
	//			mimeLetter->GetListParts()->Append(mimePart);
	//			mimeLetter->SetSubject(sTheme);
	//			if(!m_smtp.Data(mimeLetter))
	//				bot->SetValueFromMember(GetOperationResult(), wxT("false"));
	//			else
	//				bot->SetValueFromMember(GetOperationResult(), wxT("true"));
	//			
	//			delete mimeLetter;
	//		}
	//		m_smtp.Disconnect();
	//	}
	//	break;
	//	//Операция удаления письма
	//case eMailOperations_DeleteLetter:
	//	{
	//		//Получим и проверим ID письма
	//		wxString sLetterID = bot->GetValueFromMember(GetMailId());
	//		if(sLetterID == wxEmptyString)
	//			return false;
	//		double id = 0;
	//		sLetterID.ToDouble(&id);
	//		if(id == 0)
	//			return false;
	//		//Попытаемся залогиниться
	//		wxPop *pop = new wxPop();
	//		if(pop->popConnect(sAddress, sMailPort))
	//		{
	//			if(pop->popLogin(sLogin, sPassword))
	//			{
	//				pop->popDeleteLetterFromServer(id);
	//			}
	//		}
	//		delete pop;
	//	}
	//	break;
	//}
	//return true;
}
//**************************************************
void abPlugin::GetPluginInfo(struct PluginStartupInfo *Info)
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
void abPlugin::InitClass(script::VMCore *core)
{
	script::ClassTraits<CWindowAction>::bind(core);
}