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
					const Zone *pSelZone, const Point *pCurPos, bool pIsSelect)
{
	if (pData) {
		ChangeSize(pRowNum, pColNum);
		if (pData->size() < pRowNum*pColNum) {
			pData->resize(pRowNum*pColNum, -1);
		}
		bool selFlg1 = false, selFlg2 = false;
		QPixmap pix;
		for (int r = 0; r < mRowNum; r++) {
			for (int c = 0; c < mColNum; c++) {
				if (pIsSelect) {
					selFlg1 = (pSelZone && pSelZone->contains(r, c))? true : false;
					selFlg2 = mSelZone.contains(r, c);
				}
				int idx = r * mColNum + c;
				// if (mData[idx] == (*pData)[idx]) continue;
				mData[idx] = (*pData)[idx];
				int iconIdx = mData[idx];
				if (iconIdx >= 0 && mMainWin->GetPixmap(pix, iconIdx)) {
					if (selFlg1 || (!pSelZone && selFlg2)) {
						pix.setMask( pix.createHeuristicMask() );
					}
					setPixmap(r, c, pix);
				} else {
					clearCell(r, c);
				}
			}
		}
		if (pSelZone && mSelZone != *pSelZone) {
			mSelZone = *pSelZone;
			clearSelection();
			if (!mSelZone.empty()) selectCells(mSelZone[0].r, mSelZone[0].c, mSelZone[1].r, mSelZone[1].c);
		}
		if (pCurPos && !pCurPos->empty()) setCurrentCell(pCurPos->r, pCurPos->c);
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

void MapTable::SetSelectMode(bool onoff)
{
	if (onoff) {
		mAttr |= L_Attr_SelectMode;
	} else {
		UnSelect();
		mAttr &= ~L_Attr_SelectMode;
	}
}

void MapTable::NotifyIconChanged(int idx, QPixmap *pixmap)
{
	mCurIconIdx = idx;
	mCurPixmap = pixmap;
}

int MapTable::GetIconIdx(int row, int col) const
{
	int idx = row * mColNum + col;
	return mData[idx];
}

bool MapTable::SetPixmap(int pRow, int pCol, int pIconIdx, bool pIsSelect)
{
	if (pIconIdx >= 0) {
		QPixmap pix;
		mMainWin->GetPixmap(pix, pIconIdx);
		if (pIsSelect) pix.setMask( pix.createHeuristicMask() );
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

void MapTable::ResetSelZonePixmap(const Zone *pZone)
{
	for (int r = mSelZone[0].r; r <= mSelZone[1].r && r >= 0; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			if (pZone && pZone->contains(r, c)) continue;
			int idx = r * mColNum + c;
			SetPixmap(r, c, mData[idx]);
		}
	}
}

bool MapTable::Select(const Zone *pSelZone)
{
	const Zone &selZone = (pSelZone)? *pSelZone : mSelZone;
	if (selZone.empty()) return false;
	if (pSelZone && *pSelZone == mSelZone) return false;

	for (int r = selZone[0].r; r <= selZone[1].r; r++) {
		for (int c = selZone[0].c; c <= selZone[1].c; c++) {
			// if (mSelZone.contains(r, c)) continue;
			int idx = r * mColNum + c;
			QPixmap pix;
			mMainWin->GetPixmap(pix, mData[idx]);
			pix.setMask( pix.createHeuristicMask() );
			setPixmap(r, c, pix);
		}
	}
	return true;
}

void MapTable::UnSelect()
{
	if (mSelZone.empty()) return;

	for (int r = mSelZone[0].r; r <= mSelZone[1].r; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			int idx = r * mColNum + c;
			QPixmap pix;
			mMainWin->GetPixmap(pix, mData[idx]);
			setPixmap(r, c, pix);
		}
	}
	mPressPnt.init();
	mSelZone.init();
	mMainWin->NotifySelectChanged();
}
	
void MapTable::SelectAll()
{
	if ((mAttr & L_Attr_SelectMode) == 0) return;

	mSelZone.init(0, 0, mRowNum-1, mColNum-1);
	Select();
	selectCells(0, 0, mRowNum-1, mColNum-1);
	mMainWin->NotifySelectChanged();
}

void MapTable::Clear()
{
	if (mSelZone.empty()) return;

	AddUndo(L_OPE_CLEAR);

	for (int r = mSelZone[0].r; r <= mSelZone[1].r; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			int idx = r * mColNum + c;
			SetPixmap(r, c, -1);
		}
	}
	clearSelection();
	mSelZone.init();
	mMainWin->NotifyEdited();
}

void MapTable::Move(int key)
{
	if (mSelZone.empty()) return;

	int moveRow = 0, moveCol = 0;
	if		(key == Qt::Key_Left)  moveCol = -1;
	else if (key == Qt::Key_Up)	   moveRow = -1;
	else if (key == Qt::Key_Right) moveCol = 1;
	else if (key == Qt::Key_Down)  moveRow = 1;
	else return;

	AddUndo(L_OPE_MOVE);

	int incRow = (moveRow > 0)? -1 : 1;
	int incCol = (moveCol > 0)? -1 : 1;
	int sRow = (incRow > 0)? mSelZone[0].r : mSelZone[1].r;
	int eRow = (incRow > 0)? mSelZone[1].r : mSelZone[0].r;
	int sCol = (incCol > 0)? mSelZone[0].c : mSelZone[1].c;
	int eCol = (incCol > 0)? mSelZone[1].c : mSelZone[0].c;
	for (int r = sRow; r != eRow+incRow; r += incRow) {
		for (int c = sCol; c != eCol+incCol; c += incCol) {
			int iconIdx = GetIconIdx(r, c);
			SetPixmap(r+moveRow, c+moveCol, iconIdx, true);
			SetPixmap(r, c, -1);
		}
	}
	mSelZone.move(moveRow, moveCol);
	clearSelection();
	selectCells(mSelZone[0].r, mSelZone[0].c, mSelZone[1].r, mSelZone[1].c);

	mMainWin->NotifyEdited();
	Sleep(100);
}

int MapTable::Undo()
{
	if (mUndoStack.empty()) return 0;

	Point curPos(currentRow(), currentColumn());
	Journal preJnl = mUndoStack.back();
	mUndoStack.pop_back();
	if (!mUndoStack.empty() && !preJnl.IsSelect()) {
		preJnl.mCurPos = mUndoStack.back().mCurPos;
		preJnl.mSelZone = mUndoStack.back().mSelZone;
	}
	Journal redoJnl(mRowNum, mColNum, preJnl.mOperation, mData, mSelZone, curPos);
//	printf("undo %zd: (%d %d)\n", mUndoStack.size(), preJnl.mCurPos.r, preJnl.mCurPos.c);
	mRedoStack.push_back(redoJnl);
//	printf("add redo %zd: ", mRedoStack.size()); redoJnl.Dump();
	Init(preJnl.mRowNum, preJnl.mColNum, &preJnl.mData, &preJnl.mSelZone, &preJnl.mCurPos, preJnl.IsSelect());
	return (mUndoStack.empty())? 0 : 1;
}

int MapTable::Redo()
{
	if (mRedoStack.empty()) return 0;

	Journal &newJnl = mRedoStack.back();
	AddUndo(newJnl.mOperation, &newJnl.mCurPos);
	Init(newJnl.mRowNum, newJnl.mColNum, &newJnl.mData, &newJnl.mSelZone, &newJnl.mCurPos, newJnl.IsSelect());
	mRedoStack.pop_back();
//	printf("redo %zd: (%d %d)\n", mRedoStack.size(), newJnl.mCurPos.r, newJnl.mCurPos.c);
	return (mRedoStack.empty())? 0 : 1;
}

void MapTable::AddUndo(int ope, const Point *pCurPos)
{
	Point curPos(currentRow(), currentColumn());
	if (pCurPos) curPos = *pCurPos;
	Journal jnl(mRowNum, mColNum, ope, mData, mSelZone, curPos);
	mUndoStack.push_back(jnl);
//	printf("add undo %zd: ", mUndoStack.size()); jnl.Dump();
	if (!pCurPos) mRedoStack.clear();
}

void MapTable::slot_OnPressed(int row, int col, int button, const QPoint &mousePos)
{
	if ((button & Qt::LeftButton) == 0) return;

	mAttr |= L_Attr_MousePress;
	mPressPnt.init(row, col);
	Zone newZone(mPressPnt);
	ResetSelZonePixmap(&newZone);

	mSelZone = newZone;

	if (mAttr & L_Attr_SelectMode) {
		Select();
	} else {
		setPixmap(row, col, (mCurPixmap)? *mCurPixmap : QPixmap(""));
	}
}

void MapTable::slot_OnCurrentChanged(int row, int col)
{
	if ((mAttr & L_Attr_MousePress) == 0) return;
	if (mPressPnt.empty()) return;

	// 選択されなくなったセルは元のアイコンに戻す
	Zone newZone(mPressPnt, Point(row, col));
	ResetSelZonePixmap(&newZone);

	mSelZone = newZone;
	if (mAttr & L_Attr_SelectMode) {
		Select();
	} else {
		SetPixmap(mSelZone, (mCurPixmap)? *mCurPixmap : QPixmap(""));
	}
}

// 入力を確定する
void MapTable::FinalizeInput()
{
	if (mSelZone.empty()) return;

	AddUndo(L_OPE_INPUT);

	for (int r = mSelZone[0].r; r <= mSelZone[1].r; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			int idx = r * mColNum + c;
			mData[idx] = mCurIconIdx;
		}
	}
	mSelZone.init();
}

bool MapTable::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == QEvent::MouseButtonRelease) {
		if (((QMouseEvent *)e)->button() == QMouseEvent::LeftButton) {
			if ((mAttr & L_Attr_MousePress) && !mSelZone.empty()) {
				mAttr &= ~L_Attr_MousePress;
				mPressPnt.init();
				if (mAttr & L_Attr_SelectMode) {
					mMainWin->NotifySelectChanged();
				} else {
					FinalizeInput();
					mMainWin->NotifyEdited();
				}
			}
		}
	} else if (e->type() == QEvent::KeyPress) {
		const int key = ((QKeyEvent *)e)->key();
		const int state = ((QKeyEvent *)e)->state();
		if (key == Qt::Key_Delete) {
			Clear();
		} else if (key == Qt::Key_Escape) {
			UnSelect();
		} else if (key == Qt::Key_Left || key == Qt::Key_Up || key == Qt::Key_Right || key == Qt::Key_Down) {
			Move(key);
		} else if (key == Qt::Key_A && state == Qt::ControlButton) {
			SelectAll();
		}
	}
	return QTable::eventFilter(obj, e);
}
