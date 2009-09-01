#include "wx/wx.h"
#include "MainApp.h"
#include "MainFrame.h"

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
	m_pMainWin = new MainFrame(wxT("Plugin test (use 'Test' menu to create object exported from plugin)"), wxDefaultPosition, wxDefaultSize);
	m_pMainWin->Show(true);
	m_pMainWin->Maximize(true);
	SetTopWindow(m_pMainWin);
	return true;
}

