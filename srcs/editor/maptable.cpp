#include "maindef.h"
#include "maptable_moc.h"
#include "qbitmap.h"

#define L_PIXSIZE		32

MapTable::MapTable(QWidget *pParent, MainWindow *pMainWin, int pRowNum, int pColNum)
 :	QTable(pParent), mMainWin(pMainWin), mRowNum(pRowNum), mColNum(pColNum), mAttr(0),
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

	connect(this, SIGNAL(pressed(int,int,int,const QPoint&)), this, SLOT(slot_OnPressed(int,int,int,const QPoint&)));
	connect(this, SIGNAL(currentChanged(int,int)), this, SLOT(slot_OnCurrentChanged(int,int)));
	mData.resize(mRowNum * mColNum);
	fill(mData.begin(), mData.end(), -1);
}

MapTable::~MapTable()
{
}

void MapTable::Init(int pRowNum, int pColNum, vector<int> *pData,
					const Zone *pSelZone, const Point *pCurPos)
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
		if (pSelZone) mSelZone = *pSelZone;
		if (pCurPos && !pCurPos->empty()) setCurrentCell(pCurPos->r, pCurPos->c);
		if ((mAttr & L_Attr_JournalNow) == 0) AddUndo(L_OPE_OPEN);
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

bool MapTable::SetPixmap(int pRow, int pCol, int pIconIdx)
{
	if (pIconIdx >= 0) {
		QPixmap pix;
		mMainWin->GetPixmap(pix, pIconIdx);
		setPixmap(pRow, pCol, pix);
	} else {
		setPixmap(pRow, pCol, QPixmap(""));
	}

	int idx = pRow * mColNum + pCol;
	if (mData[idx] == pIconIdx) return false;
	mData[idx] = pIconIdx;
	return true;
}

bool MapTable::SetPixmap(const Zone &pZone, int pIconIdx)
{
	if (pZone.empty()) return false;

	bool updFlg = false;
	for (int r = pZone[0].r; r <= pZone[1].r; r++) {
		for (int c = pZone[0].c; c <= pZone[1].c; c++) {
			if (SetPixmap(r, c, pIconIdx)) updFlg = true;
		}
	}
	return updFlg;
}

void MapTable::SetPixmap(const Zone &pZone, const QPixmap &pPixmap)
{
	if (pZone.empty()) return;

	for (int r = pZone[0].r; r <= pZone[1].r; r++) {
		for (int c = pZone[0].c; c <= pZone[1].c; c++) {
			setPixmap(r, c, pPixmap);
		}
	}
}


int MapTable::Undo()
{
	if (mUndoStack.size() <= 1) return 0;

	mAttr |= L_Attr_JournalNow;
	Journal &redoJnl = mUndoStack.back();
	mRedoStack.push_back(redoJnl);

	mUndoStack.pop_back();
	Journal &preJnl = mUndoStack.back();
	const Zone *selZone = (mUndoStack.size() > 1)? &preJnl.mSelZone : NULL;
	const Point *curPos = (mUndoStack.size() > 1)? &preJnl.mCurPos : NULL;
	Init(preJnl.mRowNum, preJnl.mColNum, &preJnl.mData, selZone, curPos);
	mAttr &= ~L_Attr_JournalNow;
	return (mUndoStack.size() <= 1)? 0 : 1;
}

int MapTable::Redo()
{
	if (mRedoStack.empty()) return 0;

	mAttr |= L_Attr_JournalNow;
	Journal &newJnl = mRedoStack.back();
	Init(newJnl.mRowNum, newJnl.mColNum, &newJnl.mData, &newJnl.mSelZone, &newJnl.mCurPos);
	AddUndo(newJnl.mOperation, false);
	mRedoStack.pop_back();
	mAttr &= ~L_Attr_JournalNow;
	return (mRedoStack.empty())? 0 : 1;
}

void MapTable::AddUndo(int ope, bool clearRedo)
{
	Point curPos(currentRow(), currentColumn());
	Journal jnl(mRowNum, mColNum, ope, mData, mSelZone, curPos);
	mUndoStack.push_back(jnl);
	if (clearRedo) mRedoStack.clear();
}

void MapTable::slot_OnPressed(int row, int col, int button, const QPoint &mousePos)
{
	if ((button & Qt::LeftButton) == 0) return;
//	if (!mCurPixmap) return;

	mPressPnt.init(row, col);
	mSelZone.init(mPressPnt);
	mAttr |= L_Attr_MousePress;

	setPixmap(row, col, (mCurPixmap)? *mCurPixmap : QPixmap(""));
}

void MapTable::slot_OnCurrentChanged(int row, int col)
{
	if ((mAttr & L_Attr_MousePress) == 0) return;
	if (mPressPnt.empty()) return;
//	if (!mCurPixmap) return;

	// 選択されなくなったセルは元のアイコンに戻す
	Zone newZone(mPressPnt, Point(row, col));
	for (int r = mSelZone[0].r; r <= mSelZone[1].r && r >= 0; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			if (newZone.contains(r, c)) continue;
			int idx = r * mColNum + c;
			SetPixmap(r, c, mData[idx]);
		}
	}

	mSelZone = newZone;
	SetPixmap(mSelZone, (mCurPixmap)? *mCurPixmap : QPixmap(""));
}

bool MapTable::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::MouseButtonRelease) {
		if (((QMouseEvent *)e)->button() == QMouseEvent::LeftButton) {
			if ((mAttr & L_Attr_MousePress) && !mSelZone.empty()) {
				SetPixmap(mSelZone, mCurIconIdx);
				AddUndo(L_OPE_INPUT);
				mPressPnt.init();
				mSelZone.init();
				mAttr &= ~L_Attr_MousePress;
				mMainWin->NotifyEdited();
			}
		}
	}
	return QTable::eventFilter(obj, e);
}
