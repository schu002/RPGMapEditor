#include "maindef.h"
#include "zone.h"

void Point::init(int row, int col)
{
	r = row, c = col;
}

void Point::move(int row, int col)
{
	r += row;
	c += col;
}


Zone::Zone(int row, int col)
{
	init(row, col);
}

Zone::Zone(int row1, int col1, int row2, int col2)
{
	init(row1, col1, row2, col2);
}

Zone::Zone(const Point &pnt)
{
	init(pnt);
}

Zone::Zone(const Point &pnt1, const Point &pnt2)
{
	init(pnt1.r, pnt1.c, pnt2.r, pnt2.c);
}

void Zone::init()
{
	mPnts[0].r = mPnts[0].c = -1;
	mPnts[1].r = mPnts[1].c = -1;
}

void Zone::init(int row, int col)
{
	mPnts[0].r = mPnts[1].r = row;
	mPnts[0].c = mPnts[1].c = col;
}

void Zone::init(const Point &pnt)
{
	mPnts[0] = mPnts[1] = pnt;
}

void Zone::init(int row1, int col1, int row2, int col2)
{
	mPnts[0].r = M_MIN(row1, row2);
	mPnts[1].r = M_MAX(row1, row2);
	mPnts[0].c = M_MIN(col1, col2);
	mPnts[1].c = M_MAX(col1, col2);
}

void Zone::init(const Point &pnt1, const Point &pnt2)
{
	mPnts[0].r = M_MIN(pnt1.r, pnt2.r);
	mPnts[1].r = M_MAX(pnt1.r, pnt2.r);
	mPnts[0].c = M_MIN(pnt1.c, pnt2.c);
	mPnts[1].c = M_MAX(pnt1.c, pnt2.c);
}

bool Zone::contains(int row, int col) const
{
	return (mPnts[0].r <= row && row <= mPnts[1].r &&
			mPnts[0].c <= col && col <= mPnts[1].c)? true : false;
}

bool Zone::contains(const Point &pnt) const
{
	return (mPnts[0].r <= pnt.r && pnt.r <= mPnts[1].r &&
			mPnts[0].c <= pnt.c && pnt.c <= mPnts[1].c)? true : false;
}

void Zone::merge(int row, int col)
{
	if (row < 0 || col < 0) return;

	if (empty()) {
		init(row, col);
	} else {
		mPnts[0].r = M_MIN(mPnts[0].r, row);
		mPnts[1].r = M_MAX(mPnts[1].r, row);
		mPnts[0].c = M_MIN(mPnts[0].c, col);
		mPnts[1].c = M_MAX(mPnts[1].c, col);
	}
}

void Zone::merge(const Zone &zone)
{
	if (zone.empty()) return;

	if (empty()) {
		*this = zone;
	} else {
		mPnts[0].r = M_MIN(mPnts[0].r, zone[0].r);
		mPnts[1].r = M_MAX(mPnts[1].r, zone[1].r);
		mPnts[0].c = M_MIN(mPnts[0].c, zone[0].c);
		mPnts[1].c = M_MAX(mPnts[1].c, zone[1].c);
	}
}

void Zone::move(int row, int col)
{
	mPnts[0].move(row, col);
	mPnts[1].move(row, col);
}
