#include "common/commondef.h"

CTokenizer::CTokenizer(const string &s)
 : mString(s)
{
	mPlace = NULL;
}

string CTokenizer::operator()(const char* ws)
{
	char const* eos = mString.c_str() + mString.size();

	if (mPlace==NULL)		// the first time through?
		mPlace = mString.c_str();	// Initialize 'mPlace'

	size_t extent = 0;
	while (1) {
		if (mPlace >= eos)
			return "";
		mPlace += ::strspn(mPlace, ws);
		extent = ::strcspn(mPlace, ws);
		if (extent)
			break;
		++mPlace;	 // skip null
	}
	size_t start = mPlace - mString.c_str();
	mPlace += extent;		// Advance the mPlaceholder

	return mString.substr(start, extent);
}
