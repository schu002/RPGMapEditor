#include "maindef.h"
#include "maptable_moc.h"

#define L_PIXSIZE		32
#define L_KEY_MAPSIZE	"MapSize"
#define L_KEY_MAPDATA	"MapData"
#define L_CR			"<cr>"

MapTable::MapTable(QWidget *pParent, MainWindow *pMainWin, int pRowNum, int pColNum)
 :	QTableWidget(pParent), mMainWin(pMainWin), mRowNum(pRowNum), mColNum(pColNum), mAttr(0),
	mCurPixmap(NULL), mCurIconIdx(-1)
{
	if (mRowNum <= 0) mRowNum = L_DEFALUT_MAPSIZE_ROW;
	if (mColNum <= 0) mColNum = L_DEFALUT_MAPSIZE_COL;
	setRowCount(mRowNum);
	setColumnCount(mColNum);
	setEditTriggers(QAbstractItemView::NoEditTriggers); // 編集不可にする
	setSelectionMode(QAbstractItemView::NoSelection);
	setStyleSheet("QTableView::item:selected { background: #B4D4FF; }"); // 選択時の色（デフォルトの青）
	setIconSize(QSize(L_PIXSIZE, L_PIXSIZE));

	setContentsMargins(0, 0, 0, 0);
	QHeaderView *hHeader = horizontalHeader();
	QHeaderView *vHeader = verticalHeader();
	// hHeader->setSectionResizeMode(QHeaderView::Fixed);
	// vHeader->setSectionResizeMode(QHeaderView::Fixed);
	hHeader->setDefaultSectionSize(L_PIXSIZE);
	vHeader->setDefaultSectionSize(L_PIXSIZE);
	hHeader->setMinimumSectionSize(32);
	vHeader->setMinimumSectionSize(32);
	hHeader->setSectionsClickable(false);
	vHeader->setSectionsClickable(false);
	// ヘッダーをフラットなデザインにする
//	hHeader->setStyleSheet("QHeaderView::section { border: none; background: whitesmoke; padding: 0px; }");
//	vHeader->setStyleSheet("QHeaderView::section { border: none; background: whitesmoke; padding: 0px; }");

	for (int r = 0; r < mRowNum; r++) setRowHeight(r, L_PIXSIZE);
	for (int c = 0; c < mColNum; c++) setColumnWidth(c, L_PIXSIZE);

	connect(this, SIGNAL(cellPressed(int,int)), this, SLOT(slot_OnPressed(int,int)));
	connect(this, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(slot_OnCurrentChanged(int,int,int,int)));
	mData.resize(mRowNum * mColNum);
	fill(mData.begin(), mData.end(), -1);
	viewport()->installEventFilter(this);
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
				if (pIsSelect && (mAttr & L_Attr_SelectMode)) {
					selFlg1 = (pSelZone && pSelZone->contains(r, c))? true : false;
					selFlg2 = mSelZone.contains(r, c);
				}
				int idx = r * mColNum + c;
				if (mData[idx] == (*pData)[idx] && !selFlg1 && !selFlg2) continue;
				mData[idx] = (*pData)[idx];

				bool isSelect = (selFlg1 || (!pSelZone && selFlg2))? true : false;
				SetPixmap(r, c, mData[idx], isSelect, false);
			}
		}
		if (pSelZone && mSelZone != *pSelZone && (mAttr & L_Attr_SelectMode)) {
			mSelZone = *pSelZone;
			clearSelection();
			if (!mSelZone.empty()) {
				QTableWidgetSelectionRange range(mSelZone[0].r, mSelZone[0].c, mSelZone[1].r, mSelZone[1].c);
				setRangeSelected(range, true);
			}
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

void MapTable::OutputFile(FILE *fp)
{
	FinalizeMove();

	fprintf(fp, "%s = %d %d\n", L_KEY_MAPSIZE, mRowNum, mColNum);
	fprintf(fp, "%s =", L_KEY_MAPDATA);
	for (int r = 0; r < mRowNum; r++) {
		if (r > 0) fprintf(fp, "\t");
		for (int c = 0; c < mColNum; c++) {
			fprintf(fp, "%3d,", GetIconIdx(r, c)+1);
		}
		if (r < mRowNum-1) fprintf(fp, "%s", L_CR);
		fprintf(fp, "\n");
	}
}

void MapTable::ChangeSize(int pRowNum, int pColNum)
{
	if (mRowNum == pRowNum && mColNum == pColNum) return;

	FinalizeMove();
	setRowCount(pRowNum);
	setColumnCount(pColNum);
	for (int r = mRowNum; r < pRowNum; r++) setRowHeight(r, L_PIXSIZE);
	for (int c = mColNum; c < pColNum; c++) setColumnWidth(c, L_PIXSIZE);
	mRowNum = pRowNum;
	mColNum = pColNum;
	mData.resize(pRowNum * pColNum);
}

void MapTable::SetDrawGrid(bool onoff)
{
	setShowGrid(onoff);
	const int pixSize = (onoff)? L_PIXSIZE-1 : L_PIXSIZE;
	QPixmap pix;
	for (int r = 0; r < mRowNum; r++) {
		for (int c = 0; c < mColNum; c++) {
			int idx = r * mColNum + c;
			int iconIdx = mData[idx];
			if (iconIdx < 0 || !mMainWin->GetPixmap(pix, iconIdx)) continue;
			QLabel *label = qobject_cast<QLabel *>(cellWidget(r, c));
			if (!label) continue;
			label->setPixmap(pix);
			label->setFixedSize(pixSize, pixSize);
		}
	}
}

void MapTable::SetSelectMode(bool onoff)
{
	if (onoff) {
		mAttr |= L_Attr_SelectMode;
		setSelectionMode(QAbstractItemView::ContiguousSelection);
	} else {
		UnSelect();
		mAttr &= ~L_Attr_SelectMode;
		setSelectionMode(QAbstractItemView::NoSelection);
	}
}

void MapTable::SetCopyMode(bool onoff)
{
	if (onoff) {
		mAttr |= L_Attr_CopyMode;
	} else {
		FinalizeMove();
		mAttr &= ~L_Attr_CopyMode;
	}
}

void MapTable::NotifyIconChanged(int idx, const QPixmap *pixmap)
{
	mCurIconIdx = idx;
	mCurPixmap = pixmap;
}

int MapTable::GetIconIdx(int row, int col) const
{
	if (row < 0 || col < 0 || row >= mRowNum || col >= mColNum)
		return -1;

	int idx = row * mColNum + col;
	return mData[idx];
}

bool MapTable::SetPixmap(int pRow, int pCol, int pIconIdx, bool pIsSelect, bool pIsUpdate)
{
	if (pRow < 0 || pCol < 0 || pRow >= mRowNum || pCol >= mColNum)
		return false;
	int idx = pRow * mColNum + pCol;
	if (pIconIdx < 0) {
		if (!pIsUpdate || mData[idx] == pIconIdx) {
			QLabel *label = qobject_cast<QLabel *>(cellWidget(pRow, pCol));
			if (label) label->setPixmap(QPixmap(""));
			return false;
		}
	}

	QPixmap &pix = mTempPixmap;
	if (mMainWin->GetPixmap(pix, pIconIdx)) {
		if (pIsSelect) pix.setMask( pix.createHeuristicMask() );
	}
	int showFlg = (pIconIdx < 0)? 0 : 1;
	SetPixmap(pRow, pCol, pix, showFlg);

	if (pIsUpdate) {
		if (mData[idx] == pIconIdx) return false;
		mData[idx] = pIconIdx;
	}
	return true;
}

void MapTable::SetPixmap(int row, int col, const QPixmap &pixmap, int pShowFlg)
{
	QLabel *label = qobject_cast<QLabel *>(cellWidget(row, col));
	if (!label) {
		label = new QLabel(this);
		label->setPixmap(pixmap);
		label->setAlignment(Qt::AlignCenter);
		setCellWidget(row, col, label);
	} else {
		label->setPixmap(pixmap);
	}

	if (pShowFlg >= 0) {
		if (pShowFlg) {
			label->show();
		} else {
			label->setPixmap(QPixmap(""));
			label->hide();
		}
	}
}

void MapTable::SetPixmap(const Zone &pZone, const QPixmap &pPixmap, int pShowFlg)
{
	if (pZone.empty()) return;

	for (int r = pZone[0].r; r <= pZone[1].r; r++) {
		for (int c = pZone[0].c; c <= pZone[1].c; c++) {
			SetPixmap(r, c, pPixmap, pShowFlg);
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
			int idx = r * mColNum + c;
			SetPixmap(r, c, mData[idx], true, false);
		}
	}
	return true;
}

void MapTable::UnSelect()
{
	if (mSelZone.empty()) return;

	FinalizeMove();
	for (int r = mSelZone[0].r; r <= mSelZone[1].r; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			int idx = r * mColNum + c;
			SetPixmap(r, c, mData[idx], false, false);
		}
	}

	QTableWidgetSelectionRange range(0, 0, mRowNum-1, mColNum-1);
    setRangeSelected(range, false);
	mPressPnt.init();
	mSelZone.init();
	mMainWin->NotifySelectChanged();
}

void MapTable::SelectAll()
{
	if ((mAttr & L_Attr_SelectMode) == 0) return;

	FinalizeMove();
	mSelZone.init(0, 0, mRowNum-1, mColNum-1);
	Select();
	QTableWidgetSelectionRange range(mSelZone[0].r, mSelZone[0].c, mSelZone[1].r, mSelZone[1].c);
    setRangeSelected(range, true);
	mMainWin->NotifySelectChanged();
}

void MapTable::Clear()
{
	if (mSelZone.empty()) return;

	FinalizeMove();
	AddUndo(L_OPE_CLEAR);

	for (int r = mSelZone[0].r; r <= mSelZone[1].r; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			int idx = r * mColNum + c;
			SetPixmap(r, c, -1);
		}
	}
	QTableWidgetSelectionRange range(0, 0, mRowNum-1, mColNum-1);
    setRangeSelected(range, false);
	mSelZone.init();
	mMainWin->NotifyEdited();
}

void MapTable::Move(int pOfsRow, int pOfsCol)
{
	if		(mSelZone[0].r + pOfsRow < 0) pOfsRow = -mSelZone[0].r;
	else if (mSelZone[1].r + pOfsRow > mRowNum-1) pOfsRow = mRowNum-mSelZone[1].r-1;
	if		(mSelZone[0].c + pOfsCol < 0) pOfsCol = -mSelZone[0].c;
	else if (mSelZone[1].c + pOfsCol > mColNum-1) pOfsCol = mColNum-mSelZone[1].c-1;
	if (pOfsRow == 0 && pOfsCol == 0) return;

	bool isCopy = IsCopyMode();
	int incRow = (pOfsRow > 0)? -1 : 1;
	int incCol = (pOfsCol > 0)? -1 : 1;
	int sRow = (incRow > 0)? mSelZone[0].r : mSelZone[1].r;
	int eRow = (incRow > 0)? mSelZone[1].r : mSelZone[0].r;
	int sCol = (incCol > 0)? mSelZone[0].c : mSelZone[1].c;
	int eCol = (incCol > 0)? mSelZone[1].c : mSelZone[0].c;
	int ofsRow = mMovePnt.r - mSelZone[0].r;
	int ofsCol = mMovePnt.c - mSelZone[0].c;
	Zone iniZone(mSelZone);
	iniZone.move(ofsRow, ofsCol);
	for (int r = sRow; r != eRow+incRow; r += incRow) {
		for (int c = sCol; c != eCol+incCol; c += incCol) {
			int iconIdx1 = GetIconIdx(r+ofsRow, c+ofsCol);
			int iconIdx2 = (!isCopy && iniZone.contains(r, c))? -1 : GetIconIdx(r, c);
			// printf("iconIdx1 = %d (%d %d)\n", iconIdx1, r+ofsRow, c+ofsCol);
			// printf("iconIdx2 = %d (%d %d)\n", iconIdx2, r, c);
			SetPixmap(r+pOfsRow, c+pOfsCol, iconIdx1, true, false);
			SetPixmap(r, c, iconIdx2, false, false);
		}
	}
	mSelZone.move(pOfsRow, pOfsCol);
	QTableWidgetSelectionRange range(0, 0, mRowNum-1, mColNum-1);
    setRangeSelected(range, false);
	QTableWidgetSelectionRange range2(mSelZone[0].r, mSelZone[0].c, mSelZone[1].r, mSelZone[1].c);
    setRangeSelected(range2, true);
	mAttr |= L_Attr_Moved;
	// Sleep(100);
}

int MapTable::Undo()
{
	FinalizeMove();
	if (mUndoStack.empty()) return 0;

	Point curPos(currentRow(), currentColumn());
	Journal preJnl = mUndoStack.back();
	mUndoStack.pop_back();
	if (!mUndoStack.empty() && !preJnl.IsSelect()) {
		preJnl.mCurPos = mUndoStack.back().mCurPos;
		preJnl.mSelZone = mUndoStack.back().mSelZone;
	}
	if (preJnl.mOperation == L_OPE_COPY) preJnl.mSelZone.init();

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

bool MapTable::IsSelectAll() const
{
	if ((mAttr & L_Attr_SelectMode) == 0) return false;
	return (mSelZone[0].r == 0 && mSelZone[0].c == 0 &&
			mSelZone[1].r == mRowNum-1 && mSelZone[1].c == mColNum-1)?
			true : false;
}

void MapTable::slot_OnPressed(int row, int col)
{
	FinalizeMove();

	mAttr |= L_Attr_MousePress;
	mPressPnt.init(row, col);

	if (mAttr & L_Attr_SelectMode) {
		if (mSelZone.contains(row, col) && !IsSelectAll()) {
			// ドラッグ移動開始
			mAttr |= L_Attr_DragMove;
			mMovePnt = mSelZone[0];
			setSelectionMode(QAbstractItemView::NoSelection);
			return;
		}

		Zone newZone(mPressPnt);
		ResetSelZonePixmap(&newZone);

		mSelZone = newZone;

		QTableWidgetSelectionRange range(mSelZone[0].r, mSelZone[0].c, mSelZone[1].r, mSelZone[1].c);
		setRangeSelected(range, true);
		Select();
	} else {
		mSelZone.init(mPressPnt);
		int showFlg = (mCurIconIdx < 0)? 0 : 1;
		SetPixmap(row, col, (mCurPixmap)? *mCurPixmap : QPixmap(""), showFlg);
	}
}

void MapTable::slot_OnCurrentChanged(int row, int col, int preRow, int preCol)
{
	if ((mAttr & L_Attr_MousePress) == 0) return;
	if (mPressPnt.empty()) return;

	if (mAttr & L_Attr_DragMove) {
		if (mSelZone.empty()) return;
		int ofsRow = row-mSelZone[0].r-mPressPnt.r+mMovePnt.r, ofsCol = col-mSelZone[0].c-mPressPnt.c+mMovePnt.c;
		Move(ofsRow, ofsCol);
		return;
	}

	// 選択されなくなったセルは元のアイコンに戻す
	Zone newZone(mPressPnt, Point(row, col));
	ResetSelZonePixmap(&newZone);

	mSelZone = newZone;
	if (mAttr & L_Attr_SelectMode) {
		Select();
	} else {
		int showFlg = (mCurIconIdx < 0)? 0 : 1;
		SetPixmap(mSelZone, (mCurPixmap)? *mCurPixmap : QPixmap(""), showFlg);
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

// 移動を確定する
void MapTable::FinalizeMove()
{
	if ((mAttr & L_Attr_Moved) == 0) return;
	if (mMovePnt.empty() || mSelZone.empty()) return;

	int ofsRow = mMovePnt.r - mSelZone[0].r;
	int ofsCol = mMovePnt.c - mSelZone[0].c;
	if (ofsRow == 0 && ofsCol == 0) return;

	int ope = (IsCopyMode())? L_OPE_COPY : L_OPE_MOVE;
	AddUndo(ope);
	mAttr &= ~(L_Attr_Moved|L_Attr_DragMove);
	setSelectionMode(QAbstractItemView::ContiguousSelection);

	int incRow = (ofsRow < 0)? -1 : 1;
	int incCol = (ofsCol < 0)? -1 : 1;
	int sRow = (incRow > 0)? mSelZone[0].r : mSelZone[1].r;
	int eRow = (incRow > 0)? mSelZone[1].r : mSelZone[0].r;
	int sCol = (incCol > 0)? mSelZone[0].c : mSelZone[1].c;
	int eCol = (incCol > 0)? mSelZone[1].c : mSelZone[0].c;
	for (int r = sRow; r != eRow+incRow; r += incRow) {
		for (int c = sCol; c != eCol+incCol; c += incCol) {
			int preRow = r+ofsRow, preCol = c+ofsCol;
			int iconIdx = GetIconIdx(preRow, preCol);
			int idx = r * mColNum + c;
			mData[idx] = iconIdx;
		}
	}

	// 元の選択領域を初期化する
	if (ope == L_OPE_MOVE) {
		for (int r = mSelZone[0].r+ofsRow; r <= mSelZone[1].r+ofsRow; r++) {
			for (int c = mSelZone[0].c+ofsCol; c <= mSelZone[1].c+ofsCol; c++) {
				if (mSelZone.contains(r, c)) continue;
				int idx = r * mColNum + c;
				mData[idx] = -1;
			}
		}
	}
	mMovePnt.init();
	mMainWin->NotifyEdited();
}

bool MapTable::eventFilter(QObject *obj, QEvent *e)
{
	if (obj != viewport()) return false;

	if (e->type() == QEvent::MouseButtonRelease) {
		if (((QMouseEvent *)e)->button() != Qt::LeftButton) return false;
		if ((mAttr & L_Attr_MousePress) && !mSelZone.empty()) {
			mAttr &= ~L_Attr_MousePress;
			mPressPnt.init();
			if (mAttr & L_Attr_DragMove) {
				FinalizeMove();
			} else if (mAttr & L_Attr_SelectMode) {
				mMovePnt = mSelZone[0];
				mMainWin->NotifySelectChanged();
			} else {
				FinalizeInput();
				mMainWin->NotifyEdited();
			}
		}
	}
	return false;
}

bool MapTable::event(QEvent *e)
{
	if (e->type() == QEvent::KeyPress && (mAttr & L_Attr_SelectMode)) {
		const int key = ((QKeyEvent *)e)->key();
		const int mod = ((QKeyEvent *)e)->modifiers();
		if (key == Qt::Key_Delete) {
			Clear();
		} else if (key == Qt::Key_Escape) {
			UnSelect();
		} else if (key == Qt::Key_Left || key == Qt::Key_Up || key == Qt::Key_Right || key == Qt::Key_Down) {
			if (!mSelZone.empty()) {
				int ofsRow = 0, ofsCol = 0;
				if		(key == Qt::Key_Left)  ofsCol = -1;
				else if (key == Qt::Key_Up)	   ofsRow = -1;
				else if (key == Qt::Key_Right) ofsCol = 1;
				else if (key == Qt::Key_Down)  ofsRow = 1;
				Move(ofsRow, ofsCol);
			}
		} else if (key == Qt::Key_A && (mod & Qt::ControlModifier)) {
			SelectAll();
		}
	}
	return QTableWidget::event(e);
}
