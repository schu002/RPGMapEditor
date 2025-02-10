#include "maindef.h"
#include "journal.h"

Journal::Journal(int pRowNum, int pColNum, int pOpe, const vector<int> &pData,
				const Zone &pSelZone, const Point &pCurPos)
 :	mRowNum(pRowNum), mColNum(pColNum), mOperation(pOpe), mData(pData),
	mSelZone(pSelZone), mCurPos(pCurPos)
{
}

bool Journal::IsSelect() const
{
	return (mOperation == L_OPE_INPUT)? false : true;
}

void Journal::Dump() const
{
	printf("Journal ");
	if		(mOperation == L_OPE_INPUT) printf("(Input)");
	else if (mOperation == L_OPE_CLEAR) printf("(Clear)");
	printf(" pos(%d %d)", mCurPos.r, mCurPos.c);
	if (!mSelZone.empty() && IsSelect()) {
		printf(" sel(%d %d)", mSelZone[0].r, mSelZone[0].c);
	}

	for (int r = 0; r < mRowNum; r++) {
		for (int c = 0; c < mColNum; c++) {
			int idx = r * mColNum + c;
			if (mData[idx] >= 0) printf(" (%d %d)[%d]", r, c, mData[idx]);
		}
	}
	printf("\n");
}
