#ifndef zone_h
#define zone_h 1

// à íu
class Point {
public:
	Point() : r(-1), c(-1) {}
	Point(int row, int col) : r(row), c(col) {}
	void init() { r = c = -1; }
	void init(int row, int col);
	bool empty() const { return (r == -1 && c == -1); }
	void move(int row, int col);

	bool operator == (const Point &pnt) const { return (r == pnt.r && c == pnt.c); }
	bool operator != (const Point &pnt) const { return (r != pnt.r || c != pnt.c); }

public:
	int r, c;
};

// É]Å[Éì
class Zone {
public:
	Zone() {}
	Zone(int row, int col);
	Zone(int row1, int col1, int row2, int col2);
	Zone(const Point &pnt);
	Zone(const Point &pnt1, const Point &pnt2);
	void init();
	void init(const Point &pnt);
	void init(int row, int col);
	void init(int row1, int col1, int row2, int col2);
	void init(const Point &pnt1, const Point &pnt2);
	bool empty() const { return (mPnts[0].r == -1 && mPnts[1].r == -1 && mPnts[0].c == -1 && mPnts[1].c == -1); }
	bool contains(int row, int col) const;
	bool contains(const Point &pnt) const;
	void merge(int row, int col);
	void merge(const Zone &zone);
	void move(int row, int col);
	int rows() const { return (mPnts[0].r < 0)? 0 : mPnts[1].r-mPnts[0].r+1; }
	int cols() const { return (mPnts[0].c < 0)? 0 : mPnts[1].c-mPnts[0].c+1; }

	Point & operator [](int i) { return mPnts[i]; }
	const Point & operator [](int i) const { return mPnts[i]; }

	bool operator == (const Zone &zone) const { return (mPnts[0] == zone[0] && mPnts[1] == zone[1]); }
	bool operator != (const Zone &zone) const { return (mPnts[0] != zone[0] || mPnts[1] != zone[1]); }

	void Dump(bool pRet = false) const;

private:
	Point mPnts[2];
};

#endif
