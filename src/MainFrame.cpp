#ifdef __WXMSW__
  #pragma warning (disable: 4786)
#endif
#include "MainFrame.h"

#include "wx/app.h"
#include "wx/icon.h"
#include "wx/msgdlg.h"
#include "wx/thread.h"
#include "wx/dynload.h"
#include "wx/dynlib.h"
#include "wx/dir.h"

#include <script/VMCore.h>
#include <script/Declarator.h>

#include "plugin.h"

#define CMD_REFRESH	1000

wxString GetExecutablePath();
bool LoadPlugins ();

const xchar code_1[] = 
xSTRING("\
        myBasic <- CComponent_FileAction.create();  \
        myBasic.Execute();    \
");

MainFrame::MainFrame(const wxString &strTitle, const wxPoint &ptPos, const wxSize &xySize)
: wxFrame( NULL, -1, strTitle, ptPos, xySize, wxWANTS_CHARS | wxDEFAULT_FRAME_STYLE)
{
	m_pPluginObj = NULL;
	m_pPluginWnd = NULL;

	//build menus
	m_pMenuBar = new wxMenuBar;

	wxMenu *pMenuFile = new wxMenu;
	m_pMenuBar->Append(pMenuFile, wxT("&File"));
	pMenuFile->Append(CMD_REFRESH, wxT("Test"));

	SetMenuBar(m_pMenuBar);

	LoadPlugins();
}

MainFrame::~MainFrame()
{
	wxDELETE(m_pPluginObj);
}

bool LoadPlugins()
{
	wxString strDir = wxPathOnly(GetExecutablePath());
	wxDir dir(strDir);
	if (!dir.IsOpened())
		return FALSE;

	//scan for plugins
	script::VMCore *core = new script::VMCore(64);
	wxString filename;
	wxString ext = wxT("*.plug");
	bool cont = dir.GetFirst(&filename, ext, wxDIR_FILES );
	wxPluginLibrary *lib;
	while ( cont )
	{
		lib = wxPluginManager::LoadLibrary (filename);
		if (lib)
		{
			wxMessageBox(wxString::Format(wxT("Loaded [ %s ]\n"), filename));
			PluginStartupInfo plsi;
			wxDYNLIB_FUNCTION(CreatePlugin_function, CreatePlugin, *(lib));
			if(pfnCreatePlugin)
			{
				//Important: Use Detach(), otherwise the DLL will be unloaded once the wxDynamibLibrary object
				//goes out of scope
				lib->Detach();
				Plugin* plugin = pfnCreatePlugin();
				//call some method in it
				//plugin->PerformTasks();
				plugin->GetPluginInfo(&plsi);

				sq_compilebuffer(core->getVM(), code_1, wcslen(code_1), xSTRING("code_1"), false);
				sq_pushroottable(core->getVM());
				sq_call(core->getVM(), 1, false, true);

				wxDELETE(core);
				wxDELETE(plugin);
			}
		}
		wxDELETE(lib);
		cont = dir.GetNext(&filename);
	}
	
	return true;
}