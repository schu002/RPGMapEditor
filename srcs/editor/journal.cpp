#include "maindef.h"
#include "journal.h"

Journal::Journal(int pRowNum, int pColNum, int pOpe, const vector<int> &pData, const Zone &pSelZone, const Point &pCurPos)
 : mRowNum(pRowNum), mColNum(pColNum), mOperation(pOpe), mData(pData), mSelZone(pSelZone), mCurPos(pCurPos)
{
}
