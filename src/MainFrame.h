#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wx/event.h"
#include "wx/frame.h"
#include "wx/menu.h"

class Plugin;

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString &strTitle, const wxPoint &szPos, const wxSize &xySize);
	virtual ~MainFrame();

protected:
	wxMenuBar *m_pMenuBar;

	wxWindow  *m_pPluginWnd;
	Plugin	  *m_pPluginObj;
};

#endif

