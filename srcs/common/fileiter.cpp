#include "common/commondef.h"
#include "common/fileiter.h"

namespace {

bool MatchWild(const string &pPattern, const string &pStr)
{
	if (pPattern.size() > 2 && pPattern[0] == '*' && pPattern[1] == '.') {
		int idx = pStr.find_last_of('.');
		if (idx <= 0 || idx == string::npos) return false;
		string str1 = pPattern.substr(2, pPattern.size()-2);
		string str2 = pStr.substr(idx+1, pStr.size()-idx-1);
		return (str1 == str2);
	} else {
		return (pPattern == pStr);
	}
}

};

FileIter::FileIter(const char *pDir, const char *pFilter)
	: mDir(pDir), mFilter(pFilter), mhFIND(INVALID_HANDLE_VALUE)
{
}

FileIter::~FileIter()
{
	Close();
}

int FileIter::Next()
{
	string filter;
	if (mFilter.length()) {
		filter = mFilter.data();
		transform(filter.begin(), filter.end(), filter.begin(), ::tolower);
	}

	while (NextFile()) {
		if (!filter.empty()) {
			string fname(GetFileName());
			transform(fname.begin(), fname.end(), fname.begin(), ::tolower);
			if (!MatchWild(filter, fname)) continue;
		}
		return 1;
	}
	return 0;
}

int FileIter::NextFile()
{
	if (mhFIND == INVALID_HANDLE_VALUE) {
		string cstr = mDir;
		int clen = cstr.size();
		if (clen == 0) {
			cstr = "*";					//たぶんカレント
		} else if (cstr.substr(0,clen-1) == "\\" || cstr.substr(0,clen-1) == ":") {
			cstr += "*";				//そのまま*を付ける
		} else {
			cstr += "\\*";				//\*を付ける
		}

		mhFIND = ::FindFirstFile((const char *)cstr.c_str(), &mFIND_DATA);
		if (mhFIND == INVALID_HANDLE_VALUE) return 0;
	} else {
		if (::FindNextFile(mhFIND, &mFIND_DATA) == 0) {
			Close();
			return 0;
		}
	}

	return 1;
}

int FileIter::Close()
{
	if (mhFIND!= INVALID_HANDLE_VALUE) {
		FindClose(mhFIND);
		mhFIND= INVALID_HANDLE_VALUE;
	}
	return R_NORMAL;
}

const char * FileIter::GetFileName() const
{
	return (mhFIND == INVALID_HANDLE_VALUE)? "" : mFIND_DATA.cFileName;
}
