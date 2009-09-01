
#include "wx/string.h"

#ifdef __WXMAC__
# ifdef __DARWIN__
#  include 
# else
#  include 
# endif
# include "wx/filename.h"
#elif defined __WXMSW__
# include <windows.h>
#endif

wxString GetExecutablePath()
{
    static bool found = false;
    static wxString path;

    if (found)
        return path;
    else
    {
#ifdef __WXMSW__

        wchar_t buf[512];
        *buf = '\0';
        GetModuleFileName(NULL, buf, 511);
        path = buf;

#elif defined(__WXMAC__)

        ProcessInfoRec processinfo;
        ProcessSerialNumber procno ;
        FSSpec fsSpec;

        procno.highLongOfPSN = NULL ;
        procno.lowLongOfPSN = kCurrentProcess ;
        processinfo.processInfoLength = sizeof(ProcessInfoRec);
        processinfo.processName = NULL;
        processinfo.processAppSpec = &fsSpec;

        GetProcessInformation( &procno , &processinfo ) ;
        path = wxMacFSSpec2MacFilename(&fsSpec);
#else
        wxString argv0 = wxTheApp->argv[0];

        if (wxIsAbsolutePath(argv0))
            path = argv0;
        else
        {
            wxPathList pathlist;
            pathlist.AddEnvList(wxT("PATH"));
            path = pathlist.FindAbsoluteValidPath(argv0);
        }

        wxFileName filename(path);
        filename.Normalize();
        path = filename.GetFullPath();
#endif
        found = true;
        return path;
    }
}

