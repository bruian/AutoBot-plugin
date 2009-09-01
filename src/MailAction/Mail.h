#ifndef __MAIL_H__
#define __MAIL_H__

#include "AutoBot.h"
//**************************************************
#define CONNECTION_CHECK 0
#define HELLO_CHECK 1
#define MAIL_CHECK 2
#define RCPT_CHECK 3
#define DATA_START_CHECK 4
#define DATA_END_CHECK 5
#define QUIT_CHECK 6
#define DATA_CHECK 7
//**************************************************
//wxString Decode(wxString value, wxString mailCodepage = wxT("q"));
//**************************************************
enum
{
	popSOCKET_ID = 777
};
//**************************************************
class wxMimePart : public wxObject
{
private:
	wxString m_ContentType;
	wxString m_ContentTransferEncoding;
	wxString m_ContentDisposition;
	wxString m_Name;
	wxString *m_Data;
	bool	 m_FileStream;
private:
	wxString GetEncodingData(wxString value);
public:
	wxMimePart() 
	{
		m_Data = new wxString();
		m_FileStream = false;
	}
	~wxMimePart() 
	{
		m_Data->clear();
		delete m_Data;
	}
	wxString GetContentType() { return m_ContentType; }
	wxString GetContentTransferEncoding() { return m_ContentTransferEncoding; }
	wxString GetContentDisposition() { return m_ContentDisposition; }
	void SetContentType(wxString value) { m_ContentType = value; }
	void SetContentTransferEncoding(wxString value) { m_ContentTransferEncoding = value; }
	void SetContentDisposition(wxString value)
	{ 
		m_ContentDisposition = value; 
		m_FileStream = value.Contains(wxT("attachment"));
	}
	void SetName(wxString value) { m_Name = value; }
	wxString *GetData() { return m_Data; }
	wxString GetEncodingString() { return GetEncodingData(wxEmptyString); }
	wxString SaveFileTo(wxString value) { return GetEncodingData(value); }
	bool AppendFile(wxString filename);
	bool IsFileStream() { return m_FileStream; }
};
//**************************************************
class wxMimeLetter
{
private:
	wxString m_Date;
	wxString m_From;
	wxString m_XMailer;
	wxString m_Organization;
	wxString m_XPriority;
	wxString m_MessageID;
	wxString m_To;
	wxString m_Subject;
	wxString m_MIMEVersion;
	wxString m_ContentType;
	wxString m_Boundary;

	wxMimePart	*m_TempPart;
	wxList		*m_Parts;
	bool		m_OnlyHat;
	bool		m_StreamUpload;
private:
	bool isStreamUpload() { return m_StreamUpload; }
	wxString GetBoundary() { return m_Boundary; }
public:
	bool DataUnPack(wxString value);
	wxString DataPack();
	wxString GetEncodedSubject();
	void SetOnlyHat(bool value) { m_OnlyHat = value; }
	bool isOnlyHat() { return m_OnlyHat; }
	void SetBoundary(wxString value);

	//ForUsers
	wxMimeLetter();
	~wxMimeLetter();
	
	//Возвращает список частей письма
	wxList *GetListParts() { return m_Parts; }
	//Количество частей в письме
	int CountParts() { return (int)m_Parts->GetCount(); }
	//Возвращает дату письма
	wxString GetDate() { return m_Date; }
	//Возвращает декодированное значение отправителя
	wxString GetSender();
	//Возвращает декодированное значение темы письма
	wxString GetSubject();
	//Установить тему письма
	void SetSubject(wxString value) { m_Subject = value; }
	//Получить часть письма по номеру
	wxMimePart *GetPart(int number);
};
//**************************************************
class wxPopEvtHandler : public wxEvtHandler
{
	DECLARE_EVENT_TABLE()
public:
	void popOnSocketEvent(wxSocketEvent& event) 
	{
		wxString s = wxT("OnSocketEvent: ");
		//wxString s(event.GetClientData());
		switch(event.GetSocketEvent())
		{
			case wxSOCKET_INPUT      : s.Append(wxT("wxSOCKET_INPUT\n"));		break;
			case wxSOCKET_OUTPUT	 : s.Append(wxT("wxSOCKET_OUTPUT\n"));		break;
			case wxSOCKET_LOST       : s.Append(wxT("wxSOCKET_LOST\n"));		break;
			case wxSOCKET_CONNECTION : s.Append(wxT("wxSOCKET_CONNECTION\n"));	break;
			default                  : s.Append(wxT("Unexpected event !\n"));	break;
		}
		//wxLogDebug(s);
	}
};
//**************************************************
class wxPop
{
private:
	wxSocketClient  *m_popSocket;
	wxPopEvtHandler *m_popEvtSocket;
	wxString		m_popError;
	bool			m_Logged;
	wxMimeLetter		*m_popLetter;
private:
	wxString socSendCommand(wxString popCommand);
	void popSetError(wxString value) { m_popError = value; }
public:
	wxPop();
	~wxPop();

	wxSocketError popGetLastSocketError() { return m_popSocket->LastError(); }
	wxString	  popGetLastError()		  { return m_popError; }
	bool popConnect(wxString popServerAddress, wxString popServerPort = wxT("110"));
	bool popLogin(wxString popLogin, wxString popPassword);
	int  popLetterCount();
	bool popGetLetterFromServer(int numberLetter, bool onlyhat = false);
	bool popDeleteLetterFromServer(int numberLetter);
	wxMimeLetter *popGetLetter() { return m_popLetter; }
};
//**************************************************
class wxSMTP 
{
private:
	wxSocketClient m_SMTPServer;
	wxString m_ErrorMessage;
	wxString m_From;
	wxString m_Host;
	wxArrayString m_To;
	int m_NoOfTo;
	
	wxString GetError(wxString Response);
	bool CheckResponse(int Type, wxString response);
	wxString socSendCommand(wxString smtpCommand);
public:
	wxSMTP();
	~wxSMTP();

	bool Connect(wxString Host, wxString Me, wxString port = wxT("25"));
	bool Connect();
	bool Mail();
	bool Mail(wxString from);
	bool Disconnect();
	wxString GetErrorMessage();
	bool Data(wxMimeLetter *mimeLetter);

	wxString GetTo();
	bool SetTo(wxString to);
	wxString GetFrom();
	void SetFrom(wxString from);
	wxString GetHost();
	void SetHost(wxString Host);
};
#endif