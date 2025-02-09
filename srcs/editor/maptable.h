#ifndef bmpfilledit_h
#define bmpfilledit_h 1

#include "zone.h"

#define L_DEFALUT_MAPSIZE_ROW	100
#define L_DEFALUT_MAPSIZE_COL	100


class MainWindow;

class MapTable : public QTable
{
	Q_OBJECT

public:
	MapTable(QWidget *pParent, MainWindow *pMainWin, int pRowNum = 0, int pColNum = 0);
	~MapTable();

	void Init(int pRowNum, int pColNum, vector<int> *pData = NULL);

	// row colの位置のアイコン番号を取得する
	int GetIconNum(int row, int col) const;
	int GetRowNum() const { return mRowNum; }
	int GetColNum() const { return mColNum; }

	void NotifyIconChanged(int idx = -1, QPixmap *pixmap = NULL);

public slots:
	void slot_OnClicked(int row, int col, int button, const QPoint &mousePos);

private:
	void SetPixmap(int pRow, int pCol, int pIconIdx, QPixmap *pPixmap = NULL);
	void ChangeSize(int pRowNum, int pColNum);

private:
	QPixmap *mCurPixmap;
	int mCurIconIdx;
	int mRowNum, mColNum;
	vector<int>	mData;
	MainWindow *mMainWin;
};

#endif
