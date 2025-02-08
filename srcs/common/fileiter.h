#ifndef fileiter_h
#define fileiter_h 1

#if L_OS_WINDOWS
#include	"windows.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <string>
using namespace std;

#include "common/commonpack.h"


class FileIter
{
public:
	FileIter(const char *pDir, const char *pFilter);
	~FileIter();

	int Next();
	const char * GetFileName() const;

private:
	int NextFile();
	int Close();

private:
	string mDir;
	string mFilter;
#if	L_OS_WINDOWS
	HANDLE			mhFIND;
	WIN32_FIND_DATA	mFIND_DATA;
#endif
};

#endif
