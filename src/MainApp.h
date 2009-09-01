#ifndef MAINAPP_H
#define MAINAPP_H

#include "wx/app.h"

class MainFrame;

class MainApp: public wxApp
{
public:
	virtual bool OnInit();
	
protected:
	MainFrame* m_pMainWin;
};

#endif