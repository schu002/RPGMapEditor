#include "common/commondef.h"
#include "common/fileutil.h"

#define	L_CDirSep	'/'
#define L_FILE_SIZE 8192

namespace {

void DosToUnixFile(string &pfilenm, int pPosix = 0)
{
    char outfile[L_FILE_SIZE];
    char *uname = outfile;

#if L_OS_WINDOWS
    wchar_t unibuffer[L_FILE_SIZE];
    int dlen = pfilenm.size();
    mbstowcs(unibuffer, pfilenm.c_str(), L_FILE_SIZE);
    wchar_t *dname = unibuffer;
#endif

    if (dlen >= 2 && dname[1] == ':') {
		int drv = dname[0];
		if (drv >= 'A' && drv <= 'Z') {
		    if (pPosix) {
		        sprintf(uname, "//%c/", drv);
		        uname += 3;
		        dname += 2;
		    } else {
		        sprintf(uname, "%c:/", drv);
		        uname += 2;
		        dname += 2;
		    }
		}
    }

    while (*dname) {
#if L_OS_WINDOWS
		if (*dname == '\\' && (dname == unibuffer || (*(dname-1)) < 0x0081 || (0x00A0 <= (*(dname-1)) && (*(dname-1)) < 0x00E0) || (*(dname-1)) >= 0x00F0)) {
#endif
		    *uname = '/';
		    uname++;
		} else {
#if L_OS_WINDOWS
		    int size = wctomb(uname, *dname);
		    if (size > 0) uname += size;
#endif
		}
		dname++;
    }
    *uname = 0;
    pfilenm = outfile;
}

void ModifyDirSep(string &pdirname, int paddflg)
{
	if (pdirname.empty()) return;  // NULL‚Ì‚Æ‚«‚Í‰½‚à‚µ‚È‚¢‚Å–ß‚é
	int pos = (int)pdirname.size() - 1;
	if (paddflg) {
		if (pdirname[pos] != L_CDirSep)
			pdirname += string("/");
	} else {
		if (pdirname[pos] == L_CDirSep)
			pdirname.resize(pos);
	}
}

};

string FileUtil::GetCurDir(int paddsepflg)
{
	char buffer[L_FILE_SIZE];
	string dirnm;
	if (getcwd(buffer, sizeof(buffer)) == NULL)
		dirnm = "";
	else {
		dirnm = buffer;
#if L_OS_WINDOWS
		DosToUnixFile(dirnm);
#endif
		ModifyDirSep(dirnm, paddsepflg);
	}

	return dirnm;
}
