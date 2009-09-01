//Реализация классов Mail приложения
#include "Mail.h"
//**************************************************
wxString Decode(wxString value, wxString mailCodepage = wxT("q"))
{
	wxString Result = wxEmptyString;
	
	if(mailCodepage.Lower() == wxT("b"))
	{
		// The 7-bit alphabet used to encode binary information
		wxString m_sBase64Alphabet = wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
		long nSize = (long)value.Length();
		int c;
		int nDigit;
		int nDecode[ 256 ];
		int m_lBitStorage = 0;
		int m_nBitsRemaining = 0;

		// Build Decode Table
		for(int i = 0; i < 256; i++) 
			nDecode[i] = -2; // Illegal digit
		for(int i=0; i < 64; i++)
		{
			nDecode[ m_sBase64Alphabet.GetChar(i) ] = i;
			nDecode[ m_sBase64Alphabet.GetChar(i) | 0x80 ] = i; // Ignore 8th bit
			nDecode[ '=' ] = -1; 
			nDecode[ '=' | 0x80 ] = -1; // Ignore MIME padding char
			nDecode[ '\n'] = -1; // Skip the CR/LFs
			nDecode[ '\r'] = -1;
		}

		// Decode the Input
		long lp = 0;
		for(int i = 0; lp < nSize; lp++)
		{
			c = value.GetChar(lp);
			nDigit = nDecode[ c & 0x7F ];
			if(nDigit < -1)
			{
				return wxT("");
			} 
			else if(nDigit >= 0)
			// i (index into output) is incremented by write_bits()
			{
				UINT nBits = nDigit & 0x3F;
				int nNumBits = 6;

				UINT nScratch;
				m_lBitStorage = (m_lBitStorage << nNumBits) | nBits;
				m_nBitsRemaining += nNumBits;
				while( m_nBitsRemaining > 7 ) 
				{
					nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
					Result += nScratch & 0xFF;
					i++;
					m_nBitsRemaining -= 8;
				}
			}
		}
	}
	else
	{
		int nSize = (int)value.Length();
		wxChar *characterBuffer = new wxChar[nSize];
		memcpy((void*)characterBuffer, value.c_str(), nSize);
		int outpos = 0;

		for(int i = 0; i < nSize; )
		{
			if(characterBuffer[i] == 61) // Equal Sign
			{
				int octet = 0;
				if (swscanf((const wxChar *)characterBuffer+i+1, wxT("%2x"), &octet)>0)
				{
					Result += octet;
					i += 3;
					continue;
				}
				else 
				{
					// is this a CR/LF?
					if (characterBuffer[i+1] == 13)
					{
						i+=2; // simply skip the CR...
						if (characterBuffer[i] == 10) i++; //...and, probably, LF
						continue;
					}
				}
			}
			// decoding error occured! try to recover by outputting the 
			// untransformed octet and starting over from the very beginning;
			outpos++;
			Result += characterBuffer[i++];
		}
		delete[] characterBuffer;
	}

	return Result;
}
//**************************************************
UINT read_bits(int nNumBits, int *pBitsRead, int &lp, int *nBitsRemaining, wxString value, ULONG *lBitStorage)
{
	int m_nMask[] = {0, 1, 3, 7, 15, 31, 63, 127, 255};

	ULONG lScratch;
	while((*nBitsRemaining < nNumBits) && (lp < (int)value.Length()))
	{
		int c = value.GetChar(lp++);
		*lBitStorage <<= 8;
		*lBitStorage |= (c & 0xff);
		*nBitsRemaining += 8;
	}
	if(*nBitsRemaining < nNumBits)
	{
		lScratch = *lBitStorage << (nNumBits - *nBitsRemaining);
		*pBitsRead = *nBitsRemaining;
		*nBitsRemaining = 0;
	}
	else
	{
		lScratch = *lBitStorage >> (*nBitsRemaining - nNumBits);
		*pBitsRead = nNumBits;
		*nBitsRemaining -= nNumBits;
	}
	return (UINT)lScratch & m_nMask[nNumBits];
}
//**************************************************
wxString Encode(wxString value, wxString mailCodepage = wxT("q"))
{
	if(value.IsEmpty())
		return wxEmptyString;

	wxString Result = wxEmptyString;
	if(mailCodepage.Lower() == wxT("b"))
	{
		wxString m_sBase64Alphabet = wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
		int nNumBits = 6;
		UINT nDigit;
		int lp = 0;

		ULONG m_lBitStorage = 0;
		int m_nBitsRemaining = 0;
		nDigit = read_bits(nNumBits, &nNumBits, lp, &m_nBitsRemaining, value, &m_lBitStorage);
		while(nNumBits > 0)
		{
			Result += m_sBase64Alphabet[(int)nDigit];
			nDigit = read_bits(nNumBits, &nNumBits, lp, &m_nBitsRemaining, value, &m_lBitStorage);
		}
		// Pad with '=' as per RFC 1521
		while(Result.Length() % 4 != 0)
		{
			Result += '=';
		}
	}
	else
	{
		wxString code = wxT("0123456789ABCDEF");
		wxString extended_code = wxT("0123456789abcdef");
		int nSize = value.Length();
		int m_linelength = 76;

		wxChar *buffer = new wxChar[3*(nSize+3*nSize/(m_linelength))];
		int chunklen;
		int bufpos=0, linepos=0;
		for (int i = 0; i<nSize; i++)
		{
			// test if the szEncoding[i] allows literal representation
			chunklen = ((value.GetChar(i)>32 && value.GetChar(i)<=60) || (value.GetChar(i)>61 && value.GetChar(i)<=126)) ? 1 : 3;
			// test if line break needed
			if (linepos+chunklen > m_linelength-1)
			{ // insert linebreak
				buffer[bufpos++]= 61; // Equal sign
				buffer[bufpos++]= 13; // Line 
				buffer[bufpos++]= 10; //  break
				linepos=0;
			}
			if (chunklen == 1)
			{
				buffer[bufpos++] = value.GetChar(i); // literal
				linepos++;
			}
			else
			{
				buffer[bufpos++]= 61; // Equal sign
				buffer[bufpos++]= code.GetChar((value.GetChar(i) >> 4) & 0xF); //high nibble
				buffer[bufpos++]= code.GetChar(value.GetChar(i) & 0xF); //  low nibble
				linepos+=3;
			}
		}
		Result.assign(buffer, bufpos);
		delete buffer;
	}

	return Result;
}
//**************************************************
bool wxMimePart::AppendFile(wxString filename)
{
	wxString FileName;
	if(filename != wxEmptyString)
	{
		wxString path, name, ext;
		wxSplitPath(filename, &path, &name, &ext);
		if(ext.IsEmpty())
			return false;

		name += wxT(".") + ext;
		m_Name = Encode(name, wxT("q"));
		if(name != m_Name)
			m_Name = wxT("=?") + wxLocale::GetSystemEncodingName() + wxT("?q?") + m_Name + wxT("?=");
		m_ContentTransferEncoding = wxT("base64");
		m_ContentDisposition	  = wxT("attachment; filename=\"") + m_Name + wxT("\"");
		if(ext == wxT("txt"))
			m_ContentType = wxT("text/plain; name=\"") + m_Name + wxT("\"");
		else if(ext == wxT("mdb"))
			m_ContentType = wxT("application/msaccess; name=\"") + m_Name + wxT("\"");
		else if(ext == wxT("xls"))
			m_ContentType = wxT("application/msexcel; name=\"") + m_Name + wxT("\"");
		else if(ext == wxT("doc"))
			m_ContentType = wxT("application/msword; name=\"") + m_Name + wxT("\"");
		else if(ext == wxT("pdf"))
			m_ContentType = wxT("application/pdf; name=\"") + m_Name + wxT("\"");
		else if(ext == wxT("exe"))
			m_ContentType = wxT("application/x-msdownload; name=\"") + m_Name + wxT("\"");
		else if(ext == wxT("zip"))
			m_ContentType = wxT("application/x-zip-compressed; name=\"") + m_Name + wxT("\"");
		else
			m_ContentType = wxT("application/octet-stream; name=\"") + m_Name + wxT("\"");

		wxString buf;
		wxFile file(filename);
		wxChar *b2 = new wxChar[file.Length()];
		file.Read(b2, file.Length());
		m_Data->assign(b2, file.Length());
		delete b2;
	}
	else
		return false;

	return true;
}
//**************************************************
wxString wxMimePart::GetEncodingData(wxString value)
{
	wxString Result = wxEmptyString;

	wxFileOutputStream *file_output = NULL;
	wxString FileName;
	if(value != wxEmptyString)
	{
		if(m_Name.IsEmpty())
			return wxEmptyString;
		wxString path, name, ext;
		wxSplitPath(value, &path, &name, &ext);
		if(ext.IsEmpty())
			if(path.Right(1) == wxT("\\"))
				path += name;
			else
				path += (wxT("\\") + name);
		
		wxString filedecode = Decode(m_Name);
		wxRegEx reFile;
		if(reFile.Compile(wxT("[\?].[\?](.+)[\?]=")))
			if ( reFile.Matches(filedecode) )
				filedecode = reFile.GetMatch(filedecode, 1);
		
		if(path.Right(1) == wxT("\\"))
			FileName = path + filedecode;
		else
			FileName = path + wxT("\\") + filedecode;
		
		file_output = new wxFileOutputStream(FileName);
	}

	if(m_ContentTransferEncoding.Contains(wxT("quoted-printable")))
	{
		size_t nSize = m_Data->Length();
		wxChar *characterBuffer = new wxChar[nSize];
		memcpy((void*)characterBuffer, m_Data->c_str(), nSize);
		int i = 0, outpos = 0;

		while(characterBuffer[i])
		{
			if(characterBuffer[i] == 61) // Equal Sign
			{
				int octet = 0;
				if (swscanf((const wxChar *)characterBuffer+i+1, _T("%2x"), &octet)>0)
				{
					if(value != wxEmptyString)
					{
						//if(file_output->Ok())
							file_output->PutC((wxChar)octet);
					}
					else
					{
						Result += octet;
					}
					i += 3;
					continue;
				}
				else 
				{
					// is this a CR/LF?
					if (characterBuffer[i+1] == 13)
					{
						i+=2; // simply skip the CR...
						if (characterBuffer[i] == 10) i++; //...and, probably, LF
						continue;
					}
				}
			}
			// decoding error occured! try to recover by outputting the 
			// untransformed octet and starting over from the very beginning;
			outpos++;
			if(value != wxEmptyString)
			{
				file_output->PutC(characterBuffer[i++]);
			}
			else
			{
				Result += characterBuffer[i++];
			}
		}
		delete[] characterBuffer;
	}
	else if(m_ContentTransferEncoding.Contains(wxT("base64")))
	{
		// The 7-bit alphabet used to encode binary information
		wxString m_sBase64Alphabet = wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
		//int CBase64::m_nMask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };
		long nSize = (long)m_Data->Length();
		int c;
		int nDigit;
		int nDecode[ 256 ];

		//LPTSTR szOutput = Result.GetBuffer(nSize); // Output is never longer than input

		int m_lBitStorage = 0;
		int m_nBitsRemaining = 0;

		// Build Decode Table
		for(int i = 0; i < 256; i++) 
			nDecode[i] = -2; // Illegal digit
		for(int i=0; i < 64; i++)
		{
			nDecode[ m_sBase64Alphabet.GetChar(i) ] = i;
			nDecode[ m_sBase64Alphabet.GetChar(i) | 0x80 ] = i; // Ignore 8th bit
			nDecode[ '=' ] = -1; 
			nDecode[ '=' | 0x80 ] = -1; // Ignore MIME padding char
			nDecode[ '\n'] = -1; // Skip the CR/LFs
			nDecode[ '\r'] = -1;
		}

		// Decode the Input
		long lp = 0;
		for(int i = 0; lp < nSize; lp++)
		{
			c = m_Data->GetChar(lp);
			nDigit = nDecode[ c & 0x7F ];
			if(nDigit < -1)
			{
				return _T("");
			} 
			else if(nDigit >= 0)
				// i (index into output) is incremented by write_bits()
				{
					UINT nBits = nDigit & 0x3F;
					int nNumBits = 6;

					UINT nScratch;
					m_lBitStorage = (m_lBitStorage << nNumBits) | nBits;
					m_nBitsRemaining += nNumBits;
					while( m_nBitsRemaining > 7 ) 
					{
						nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
					    if(value != wxEmptyString)
						{
							file_output->PutC(nScratch & 0xFF);
						}
						else
						{
							Result += nScratch & 0xFF;
						}
						i++;
						m_nBitsRemaining -= 8;
					}
				}
		}
	}

    if(value != wxEmptyString)
	{
		file_output->Close();
		file_output->Reset();
		delete file_output;
		return FileName;
	}
	else
		return Result;
}
//**************************************************
wxMimeLetter::wxMimeLetter() 
{
	m_Parts = new wxList();
	m_StreamUpload = false;
	m_TempPart = NULL;
	
	m_MIMEVersion = wxT("1.0");
	m_ContentType = wxT("multipart/mixed");
	m_Boundary	  = wxT("----9v9vDqN7-o4j5MTPXMap3AxbJ:1221817557");
}
//**************************************************
wxMimeLetter::~wxMimeLetter() 
{
	m_Parts->DeleteContents(true);
	if(m_Parts != NULL)
		delete m_Parts;
	if(m_TempPart != NULL)
		delete m_TempPart;
}
//**************************************************
wxString wxMimeLetter::DataPack()
{
	wxString packString;
	packString.Append(wxT("MIME-Version:") + m_MIMEVersion + wxT("\r\n"));
	packString.Append(wxT("Content-Type:") + m_ContentType + wxT(";\r\n"));
	packString.Append(wxT("\tboundary=\"") + m_Boundary + wxT("\"\r\n"));
	packString.Append(wxT("\n\n"));
	
	if(m_Parts->IsEmpty())
		return wxEmptyString;

	wxList::compatibility_iterator node = m_Parts->GetFirst();
	while (node)
	{
		wxMimePart *mimePart = (wxMimePart *)node->GetData();
		packString.Append(wxT("--") + m_Boundary + wxT("\r\n"));
		packString.Append(wxT("Content-Type: ") + mimePart->GetContentType() + wxT("\r\n"));
		packString.Append(wxT("Content-Disposition: ") + mimePart->GetContentDisposition() + wxT("\r\n"));
		packString.Append(wxT("Content-Transfer-Encoding: ") + mimePart->GetContentTransferEncoding() + wxT("\r\n\n"));
		packString.Append(Encode(*mimePart->GetData(), wxT("b")));
		packString.Append(wxT("\r\n"));
		node = node->GetNext();
	}
	//packString.Append(m_Boundary + wxT("--\r\n"));
	return packString;
}
//**************************************************
wxString wxMimeLetter::GetEncodedSubject()
{
	wxString returnString(Encode(m_Subject, wxT("q")));
	if(m_Subject != returnString)
		returnString = wxT("=?") + wxLocale::GetSystemEncodingName() + wxT("?q?") + returnString + wxT("?=");

	return returnString;
}
//**************************************************
bool wxMimeLetter::DataUnPack(wxString value)
{
	wxString lowValue = value.Lower();
	//wxLogDebug(value);
	if(!m_Boundary.IsEmpty())
	{
		if(value.Contains(m_Boundary + wxT("--")))
		{
			m_Parts->Append(m_TempPart);
			m_TempPart = NULL;
			m_StreamUpload = false;
			return false;
		}
	}

	if(m_TempPart != NULL)
	{
		if(value.Contains(m_Boundary))
		{
			m_Parts->Append(m_TempPart);
			m_TempPart = new wxMimePart();
			m_StreamUpload = false;
		}
		else if(m_StreamUpload)
		{
			if((value.Right(1) == wxT("\r")) && (m_TempPart->IsFileStream()))
				m_TempPart->GetData()->Append(value, value.Length()-1);
			else
				m_TempPart->GetData()->Append(value);
		}
		else if(lowValue.Contains(wxT("content-type:")))
		{
			wxString sTag(wxT("content-type:"));
			m_TempPart->SetContentType(lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1)));
			size_t first = lowValue.find(wxT("name"), 0);
			if((first == 0) || (first > lowValue.Len()))
				m_TempPart->SetName(wxEmptyString);
			else
				m_TempPart->SetName(value.Mid(first + 6, value.Length() - (first + 8)));
		}
		else if(lowValue.Contains(wxT("content-transfer-encoding:")))
		{
			wxString sTag(wxT("content-transfer-encoding:"));
			m_TempPart->SetContentTransferEncoding(lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1)));
		}
		else if(lowValue.Contains(wxT("content-disposition:")))
		{
			wxString sTag(wxT("Content-Disposition:"));
			m_TempPart->SetContentDisposition(lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1)));
		}
		else if(lowValue.Contains(wxT("name")))
		{
			size_t first = lowValue.find(wxT("name"), 0);
			if((first > 0) || (first < lowValue.Len()))
				m_TempPart->SetName(value.Mid(first + 6, value.Length() - (first + 8)));
		}
		else if(value == wxT("\r"))
			m_StreamUpload = true;

		return true;
	}

	if(lowValue.Contains(wxT("date:")))
	{
		if(lowValue.find(wxT("date:"))<1)
		{
			wxString sTag(wxT("date:"));
			lowValue.Trim();
			m_Date = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("from:")))
	{
		if(lowValue.find(wxT("from:"))<1)
		{
			wxString sTag(wxT("from:"));
			lowValue.Trim();
			m_From = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("x-mailer:")))
	{
		if(lowValue.find(wxT("x-mailer:"))<1)
		{
			wxString sTag(wxT("x-mailer:"));
			lowValue.Trim();
			m_XMailer = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("organization:")))
	{
		if(lowValue.find(wxT("organization:"))<1)
		{
			wxString sTag(wxT("organization:"));
			lowValue.Trim();
			m_Organization = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("x-priority:")))
	{
		if(lowValue.find(wxT("x-priority:"))<1)
		{
			wxString sTag(wxT("x-priority:"));
			lowValue.Trim();
			m_XPriority = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("message-id:")))
	{
		if(lowValue.find(wxT("message-id:"))<1)
		{
			wxString sTag(wxT("message-id:"));
			lowValue.Trim();
			m_MessageID = lowValue.Right(value.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("to:")))
	{
		if(lowValue.find(wxT("to:"))<1)
		{
			wxString sTag(wxT("to:"));
			lowValue.Trim();
			m_To = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("subject:")))
	{
		if(lowValue.find(wxT("subject:"))<1)
		{
			wxString sTag(wxT("subject:"));
			lowValue.Trim();
			m_Subject = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("mime-version:")))
	{
		if(lowValue.find(wxT("mime-version:"))<1)
		{
			wxString sTag(wxT("mime-version:"));
			lowValue.Trim();
			m_MIMEVersion = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if((lowValue.Contains(wxT("content-type:"))) && (m_TempPart == NULL))
	{
		if(lowValue.find(wxT("content-type:"))<1)
		{
			wxString sTag(wxT("content-type:"));
			lowValue.Trim();
			m_ContentType = lowValue.Right(lowValue.Length() - (lowValue.find(sTag, 0) + sTag.Length() + 1));
		}
	}
	else if(lowValue.Contains(wxT("boundary")))
	{
		size_t first = value.find(wxT("boundary"), 0);
		SetBoundary(value.Mid(first + 10, value.Length() - (first + 12)));
	}
	else if(!m_Boundary.IsEmpty())
	{
		if(value.Contains(m_Boundary))
			m_TempPart = new wxMimePart();
	}

	return true;
}
//**************************************************
wxString wxMimeLetter::GetSender()	
{ 
	wxString texts = m_From;
	wxRegEx reEmail;
	if(reEmail.Compile(wxT("([[:alpha:][:alnum:]_\.\-]+@[[:alpha:][:alnum:]\.\-]+)")))
		if(reEmail.Matches(texts))
		{
			return Decode(reEmail.GetMatch(texts));
		}
	return Decode(texts);
}
//**************************************************
wxString wxMimeLetter::GetSubject()
{ 
	wxString texts = m_Subject;
	wxRegEx reEmail;
	if(reEmail.Compile(wxT("[\?](.)[\?](.+)[\?]=")))
		if(reEmail.Matches(texts))
		{
			return Decode(reEmail.GetMatch(texts, 2), reEmail.GetMatch(texts, 1));
		}
	return Decode(texts);
}
//**************************************************
void wxMimeLetter::SetBoundary(wxString value)
{
	wxString texts = value;
	wxRegEx reEmail;
	if(reEmail.Compile(wxT("(\-+.+)\:[[:alnum:]]")))
		if(reEmail.Matches(value))
		{
			m_Boundary = reEmail.GetMatch(value, 1);
			return;
		}
	m_Boundary = value;
}
//**************************************************
wxMimePart *wxMimeLetter::GetPart(int number)
{
	if(number > (int)m_Parts->GetCount())
		return NULL;
	if(m_Parts->IsEmpty())
		return NULL;

	wxObjectListNode *obj = m_Parts->Item((size_t)number);
	if(obj != NULL)
	{
		wxMimePart *mimePart = (wxMimePart*)obj->GetData();
		if(mimePart != NULL)
			return mimePart;
	}
	return NULL;
}
//**************************************************
BEGIN_EVENT_TABLE(wxPopEvtHandler, wxEvtHandler)
	EVT_SOCKET(popSOCKET_ID, wxPopEvtHandler::popOnSocketEvent)
END_EVENT_TABLE()
//**************************************************
wxPop::wxPop()
{
	m_Logged		= false;
	// Create the socket
	m_popLetter		= NULL;
	m_popSocket		= new wxSocketClient();
	//m_popEvtSocket	= new wxPopEvtHandler();

	// Setup the event handler and subscribe to most events
	//m_popSocket->SetEventHandler(*m_popEvtSocket, popSOCKET_ID);
	//m_popSocket->SetNotify(	wxSOCKET_CONNECTION_FLAG |
	//						wxSOCKET_INPUT_FLAG |
	//						wxSOCKET_OUTPUT_FLAG |
	//						wxSOCKET_LOST_FLAG);
	//m_popSocket->Notify(true);
}
//**************************************************
wxPop::~wxPop()
{
	wxString popReply = socSendCommand(wxT("QUIT "));

	if (m_popSocket->IsConnected())
		m_popSocket->Close();

	//delete m_popEvtSocket;
	delete m_popSocket;
	delete m_popLetter;
}
//**************************************************
bool wxPop::popConnect(wxString popServerAddress, wxString popServerPort) 
{
	wxIPV4address popAddr;

	popAddr.Hostname(popServerAddress);
	popAddr.Service(popServerPort);
	
	if (m_popSocket->IsConnected())
		m_popSocket->Close();

	//m_popSocket->SetFlags(wxSOCKET_NONE);
	//m_popSocket->SetTimeout(5);

	m_popSocket->Connect(popAddr, false);
	m_popSocket->WaitOnConnect(3);
    if (m_popSocket->IsConnected())
    {
		wxChar *buf = new wxChar[128];
		m_popSocket->Read(buf, 128);
		wxUint32 uiCount	= m_popSocket->LastCount();
		wxString readString(buf, uiCount);
		delete buf;
		
		if(readString.Contains(wxT("+OK")))
			return true;
		else
			popSetError(readString);
    }

	return false;
}
//**************************************************
wxString wxPop::socSendCommand(wxString popCommand)
{
	if(!m_popSocket->IsConnected())
	{
		popSetError(wxT("ERROR: In wxSocket::Connected"));
		return wxEmptyString;
	}
	wxUint32 uiCount;
	wxString stringBuffer;

	m_popSocket->Write(popCommand.c_str(), popCommand.length());
	m_popSocket->Write("\r\n", 2);
	
	//m_popSocket->SetFlags(wxSOCKET_NONE);
	//m_popSocket->SetTimeout(5);

	wxChar *readBuffer = new wxChar[250];
	for(;;)
	{
		if(m_popSocket->Read(readBuffer, 250).Error())
		{
			if(!m_popSocket->IsConnected())
				popSetError(wxT("ERROR: In wxSocket::Connected"));
			break;
		}
		uiCount = m_popSocket->LastCount();
		if(uiCount < 1)
			break;
		stringBuffer.append(readBuffer, uiCount);
		if(uiCount < 250)
			break;		
	}
	delete readBuffer;

	return stringBuffer;
}
//**************************************************
bool wxPop::popLogin(wxString popLogin, wxString popPassword)
{
	wxString popCommand(wxT("USER ") + popLogin);
	wxString popReply = socSendCommand(popCommand);
	if(!popReply.Contains(wxT("+OK")))
	{
		popSetError(popReply);
		return false;
	}
	popCommand.clear(); 
	popCommand.Printf(wxT("PASS ") + popPassword);
	popReply = socSendCommand(popCommand);
	if(!popReply.Contains(wxT("+OK")))
	{
		popSetError(popReply);
		return false;
	}

	return true;
}
//**************************************************
int wxPop::popLetterCount()
{
	wxString popReply = socSendCommand(wxT("STAT"));
	if(!popReply.Contains(wxT("+OK")))
	{
		popSetError(popReply);
		return 0;
	}
	
	wxString sLetterCount = wxEmptyString;
	for(size_t i = 4; i < popReply.length(); i++)
	{
		if(popReply.GetChar(i) == ' ')
			break;
		sLetterCount += popReply.GetChar(i);
	}
	
	double dLetterCount;
	if(sLetterCount.ToDouble(&dLetterCount))
		return (int)dLetterCount;
	else
		return 0;
}
//**************************************************
bool wxPop::popDeleteLetterFromServer(int numberLetter)
{
	wxString popCommand;
	popCommand.Printf(wxT("DELE %d"), numberLetter);
	wxString popReply = socSendCommand(popCommand);
	if(!popReply.Contains(wxT("+OK")))
	{
		popSetError(popReply);
		return false;
	}
	popReply = socSendCommand(wxT("QUIT "));
	if(!popReply.Contains(wxT("+OK")))
	{
		popSetError(popReply);
		return false;
	}

	return true;
}
//**************************************************
//Получить письмо с сервера.
//numberLetter - ID номер письма
//onlyhat	   - (true)загружает только шапку письма, тело не не загружает. 
//				 Позволяет сократить объем трафика, при просмотре заголовков писем
//возвращает true в случае успешного получения письма
bool wxPop::popGetLetterFromServer(int numberLetter, bool onlyhat)
{
	wxString popCommand;
	wxUint32 readPortion;
	wxUint32 uiCount;
	wxString stringBuffer;
	if(onlyhat)
	{
		popCommand.Printf(wxT("TOP %d"), numberLetter);
		readPortion = 600;
	}
	else
	{
		popCommand.Printf(wxT("RETR %d"), numberLetter);
		readPortion = 128;
	}

	if(!m_popSocket->IsConnected())
	{
		popSetError(wxT("ERROR: In wxSocket::Connected"));
		return false;
	}

	m_popSocket->Write(popCommand.c_str(), popCommand.length());
	m_popSocket->Write("\r\n", 2);
	
	m_popLetter = new wxMimeLetter();
	m_popLetter->SetOnlyHat(onlyhat);
	
	m_popSocket->SetFlags(wxSOCKET_NONE);
	m_popSocket->SetTimeout(2);
	bool can = false;

	wxChar *readBuffer = new wxChar[readPortion];
	for(;;)
	{
		if(m_popSocket->Read(readBuffer, readPortion).Error())
		{
			uiCount = m_popSocket->LastCount();
			if(uiCount > 0)
			{
				stringBuffer.append(readBuffer, uiCount);
				memset(readBuffer, 0, readPortion);
			}
			
			wxSocketError err = m_popSocket->LastError();
			if(err == wxSOCKET_WOULDBLOCK)
			{
				if(uiCount > 0)
					continue;
				if(readBuffer[0] == 0)
					if(can) break;
			}
			else
				break;
			if(!m_popSocket->IsConnected())
				popSetError(wxT("ERROR: In wxSocket::Connected"));
		}
		
		uiCount = m_popSocket->LastCount();
		if(uiCount > 0)
		{
			stringBuffer.append(readBuffer, uiCount);
			memset(readBuffer, 0, readPortion);
			can = true;
		}
	}
	delete readBuffer;

	wxStringTokenizer tokenizer(stringBuffer, wxT("\n"));
	while(tokenizer.HasMoreTokens())
	{
		if(!m_popLetter->DataUnPack(tokenizer.GetNextToken()))
			break;
	}

	return true;
}
//**************************************************
wxSMTP::wxSMTP()
{
	m_NoOfTo = 0;
}
//**************************************************
wxSMTP::~wxSMTP()
{
	m_To.Clear();
	if (m_SMTPServer.IsConnected())
		m_SMTPServer.Close();
}
//**************************************************
// Коннектимся к SMTP Серверу
bool wxSMTP::Connect()
{
	return Connect(m_Host, m_From);
}
//**************************************************
//Коннектимся к SMTP Серверу
bool wxSMTP::Connect(wxString Host, wxString From, wxString port)
{
	//Заполним структуру адреса, куда передадим значения хоста и порта
	wxIPV4address smtpAddr;
	smtpAddr.Hostname(Host);
	smtpAddr.Service(port);
	
	//Установими соединение с смтп сервером, при этом попробуем обождать 1 секунду.
	m_SMTPServer.Connect(smtpAddr, false);
	m_SMTPServer.WaitOnConnect(1);
    if(m_SMTPServer.IsConnected())					//Убедимся что соединение установлено
    {
		m_SMTPServer.SetFlags(wxSOCKET_NONE);
		m_SMTPServer.SetTimeout(10);
	
		wxChar *buf = new wxChar[1024];
		m_SMTPServer.Read(buf, 1024);
		wxUint32 uiCount = m_SMTPServer.LastCount();
		wxString responseString(buf, uiCount);
		delete buf;

		if(!CheckResponse(CONNECTION_CHECK, responseString))
			return false;
				
		wxString smtpCommand(wxT("HELO ") + From);
		responseString = socSendCommand(smtpCommand);

		if (!CheckResponse(HELLO_CHECK, responseString))
			return false;
		else 
			return true; 
	}

	return false;
}
//**************************************************
wxString wxSMTP::socSendCommand(wxString smtpCommand)
{
	if(!m_SMTPServer.IsConnected())
	{
		return wxT("ERROR: In wxSocket::Connected");
	}

	m_SMTPServer.Write(smtpCommand.c_str(), smtpCommand.length());
	m_SMTPServer.Write("\r\n", 2);
	
	m_SMTPServer.SetFlags(wxSOCKET_WAITALL);

	wxString readString = wxEmptyString;
	wxChar ch = wxT('\0');
	for(;;)
	{
		if(m_SMTPServer.Read(&ch, sizeof(ch)).Error())
		{
			if(!m_SMTPServer.IsConnected())
			{
				break;
			}
		}
		if ( ch == '\r' )
			continue;
		if ( ch == '\n' )
			break;
		readString += ch;
	}
	
	return readString;
}
//**************************************************
// Устанавливает адрес хоста
void wxSMTP::SetHost(wxString Host)
{
	m_Host = Host;
}
//**************************************************
// Возвращаем адрес хоста
wxString wxSMTP::GetHost()
{
	return m_Host;
}
//**************************************************
// Посылаем SMTP серверу команду "QUIT"
bool wxSMTP::Disconnect()
{
	wxString readString = socSendCommand(wxT("QUIT"));

	if(!CheckResponse(QUIT_CHECK, readString))
		return false;
	else 
		return true;
}
//**************************************************
// Посылаем SMTP серверу команду "MAIL"
bool wxSMTP::Mail(wxString from)
{
	wxString smtpCommand;
	smtpCommand.Printf(wxT("MAIL From:<%s>"), from);
	wxString readString = socSendCommand(smtpCommand);

	if(!CheckResponse(MAIL_CHECK, readString))
		return false;
	else 
		return true; 
}
//**************************************************
// Устанавливаем строку From
void wxSMTP::SetFrom(wxString from)
{
	m_From = from;
}
//**************************************************
// Возвращаем строку From
wxString wxSMTP::GetFrom()
{
	return m_From;
}
//**************************************************
// Устанавливаем строку TO
bool wxSMTP::SetTo(wxString to)
{
	m_To.Add(to); // Сохраняем содержимое "to"

	wxString smtpCommand;
	smtpCommand.Printf(wxT("RCPT TO:<%s>"), to);
	wxString readString = socSendCommand(smtpCommand);

	if(!CheckResponse(RCPT_CHECK, readString))
		return false;
	else
		return true;
}
//**************************************************
// Возвращаем строку TO
wxString wxSMTP::GetTo()
{ 
	if((int)m_To.GetCount()>= m_NoOfTo)
	{
		m_NoOfTo++;
		return m_To.Item(m_NoOfTo-1);
	}
	else
		return wxT("No more To available");
}
//**************************************************
// Посылаем SMTP серверу команду "DATA"
bool wxSMTP::Data(wxMimeLetter *mimeLetter)
{
	if(mimeLetter == NULL)
		return false;
	wxString smtpCommand(wxT("DATA"));
	
	wxString readString = socSendCommand(smtpCommand);
	if(!CheckResponse(DATA_CHECK, readString))
		return false;
	else
	{
		smtpCommand.clear();
		smtpCommand.Printf(wxT("SUBJECT:%s"), mimeLetter->GetEncodedSubject());
		m_SMTPServer.Write(smtpCommand.c_str(), smtpCommand.length());
		m_SMTPServer.Write("\r\n", 2);
		wxString Body(mimeLetter->DataPack());
		m_SMTPServer.Write(Body.c_str(), Body.length());
		m_SMTPServer.Write("\r\n", 2);
		wxString readString = socSendCommand(wxT("."));
		if(!CheckResponse(DATA_END_CHECK, readString))
			return false;
		else
			return true;
	}
}
//**************************************************
// Этот метод проверяет ответы от сервера
bool wxSMTP::CheckResponse(int Type, wxString response)
{
	double dResponse = 0;
	response.Left(3).ToDouble(&dResponse);

	switch (Type)
	{
	case CONNECTION_CHECK:
		if ((int)dResponse != 220)
		{
			m_ErrorMessage = GetError(response);
			return false;
		} 
		break;
	case HELLO_CHECK:
		if ((int)dResponse != 250)
		{
			m_ErrorMessage = GetError(response);
			return false;
		} 
		break;
	case MAIL_CHECK:
		if ((int)dResponse != 250)
		{
			m_ErrorMessage = GetError(response);
			return false;
		} 
		break;
	case RCPT_CHECK:
		if ((int)dResponse != 250)
		{
			m_ErrorMessage = GetError(response);
			return false;
		} 
		break;
	case DATA_START_CHECK:
		if ((int)dResponse != 354)
		{
			m_ErrorMessage = GetError(response);
			return false;
		} 
		break;
	case DATA_END_CHECK:
		if ((int)dResponse != 250)
		{
			m_ErrorMessage = GetError(response);
			return false;
		} 
		break;
	case QUIT_CHECK:
		if ((int)dResponse != 221)
		{
			m_ErrorMessage = GetError(response);
			return false;
		} 
		break;
	case DATA_CHECK:
		if ((int)dResponse != 354)
		{
			m_ErrorMessage = GetError(response);
			return false;
		} 
		break;
	}
	return true;
}
//**************************************************
// Получаем сообщение об ошибке
wxString wxSMTP::GetErrorMessage()
{
	return m_ErrorMessage;
}
//**************************************************
// Сопоставляем сообщению об ошибке - номер ошибки
wxString wxSMTP::GetError(wxString Response)
{
	if(Response.Contains(wxT("211")))
		return wxT("System status or system help reply");
	if(Response.Contains(wxT("214")))
		return wxT("Help Message");
	if(Response.Contains(wxT("220")))
		return wxT("Service is ready");
	if(Response.Contains(wxT("221")))
		return wxT("Service closing transmission channel");
	if(Response.Contains(wxT("250")))
		return wxT("Requested mail action okay, completed");
	if(Response.Contains(wxT("251")))
		return wxT("user not local: will forward to forward path");
	if(Response.Contains(wxT("354")))
		return wxT("Start mail input; end with <CRLF>.<CRLF>");
	
	return wxT("No Error Number is matched with ")+Response;
}
//**************************************************
// Метод Mail без параметров
bool wxSMTP::Mail()
{
	return Mail(m_From);
}