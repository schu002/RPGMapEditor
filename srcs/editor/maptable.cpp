#include "maindef.h"
#include "maptable_moc.h"
#include "qbitmap.h"

#define L_PIXSIZE		32

MapTable::MapTable(QWidget *pParent, MainWindow *pMainWin, int pRowNum, int pColNum)
 :	QTable(pParent), mMainWin(pMainWin), mRowNum(pRowNum), mColNum(pColNum),
	mCurPixmap(NULL), mCurIconIdx(-1)
{
	if (mRowNum <= 0) mRowNum = L_DEFALUT_MAPSIZE_ROW;
	if (mColNum <= 0) mColNum = L_DEFALUT_MAPSIZE_COL;
	setNumRows(mRowNum);
	setNumCols(mColNum);
	setSelectionMode(QTable::Single);

	setLeftMargin(30);
	QHeader *hHeader = horizontalHeader();
	QHeader *vHeader = verticalHeader();
	hHeader->setResizeEnabled(false);
	vHeader->setResizeEnabled(false);
	hHeader->setClickEnabled(false);
	vHeader->setClickEnabled(false);

	for (int r = 0; r < mRowNum; r++) {
		setRowHeight(r, L_PIXSIZE);
		setRowReadOnly(r, true);
	}
	for (int c = 0; c < mColNum; c++) {
		setColumnWidth(c, L_PIXSIZE);
		setColumnReadOnly(c, true);
	}

	connect(this, SIGNAL(clicked(int,int,int,const QPoint&)), this, SLOT(slot_OnClicked(int,int,int,const QPoint&)));
	mData.resize(mRowNum * mColNum);
	fill(mData.begin(), mData.end(), -1);
}

MapTable::~MapTable()
{
}

void MapTable::Init(int pRowNum, int pColNum, vector<int> *pData)
{
	if (pData) {
		ChangeSize(pRowNum, pColNum);
		if (pData->size() < pRowNum*pColNum) {
			pData->resize(pRowNum*pColNum, -1);
		}
		QPixmap pix;
		for (int r = 0; r < mRowNum; r++) {
			for (int c = 0; c < mColNum; c++) {
				int idx = r * mColNum + c;
				// if (mData[idx] == (*pData)[idx]) continue;
				mData[idx] = (*pData)[idx];
				int iconIdx = mData[idx];
				if (iconIdx >= 0 && mMainWin->GetPixmap(pix, iconIdx)) {
					setPixmap(r, c, pix);
				} else {
					clearCell(r, c);
				}
			}
		}
	} else {
		if (mRowNum == pRowNum && mColNum == pColNum) return;

		const vector<int> oldData(mData);
		for (int r = 0; r < pRowNum; r++) {
			for (int c = 0; c < pColNum; c++) {
				int idx1 = r * pColNum + c;
				int idx2 = r * mColNum + c;
				mData[idx1] = (r < mRowNum && c < mColNum)? oldData[idx2] : -1;
			}
		}

		ChangeSize(pRowNum, pColNum);
	}
}

void MapTable::ChangeSize(int pRowNum, int pColNum)
{
	if (mRowNum == pRowNum && mColNum == pColNum) return;

	setNumRows(pRowNum);
	setNumCols(pColNum);
	for (int r = mRowNum; r < pRowNum; r++) setRowHeight(r, L_PIXSIZE);
	for (int c = mColNum; c < pColNum; c++) setColumnWidth(c, L_PIXSIZE);
	mRowNum = pRowNum;
	mColNum = pColNum;
	mData.resize(pRowNum * pColNum);
}

void MapTable::NotifyIconChanged(int idx, QPixmap *pixmap)
{
	mCurIconIdx = idx;
	mCurPixmap = pixmap;
}

int MapTable::GetIconNum(int row, int col) const
{
	int idx = row * mColNum + col;
	return mData[idx]+1;
}

void MapTable::SetPixmap(int pRow, int pCol, int pIconIdx, QPixmap *pPixmap)
{
	if (pIconIdx >= 0) {
		if (pPixmap) {
			setPixmap(pRow, pCol, *pPixmap);
		} else {
			QPixmap pix;
			mMainWin->GetPixmap(pix, pIconIdx);
			setPixmap(pRow, pCol, pix);
		}
	} else {
		setPixmap(pRow, pCol, QPixmap(""));
	}

	int idx = pRow * mColNum + pCol;
	mData[idx] = pIconIdx;
}

void MapTable::slot_OnClicked(int row, int col, int button, const QPoint &mousePos)
{
	if ((button & Qt::LeftButton) == 0) return;

	int idx = row * mColNum + col;
	if (mData[idx] == mCurIconIdx) return;

	SetPixmap(row, col, mCurIconIdx, mCurPixmap);
	mMainWin->NotifyEdited();
}
