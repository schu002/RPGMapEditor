#include "maindef.h"
#include "maptable_moc.h"

#define L_PIXSIZE		32
#define L_KEY_MAPSIZE	"MapSize"
#define L_KEY_MAPDATA	"MapData"
#define L_CR			"<cr>"

class PixmapDelegate : public QStyledItemDelegate {
public:
    explicit PixmapDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        // 【デバッグ】背景を黒で塗る（隙間が見えやすくなる）
        painter->fillRect(option.rect, Qt::white);

        QVariant value = index.data(Qt::DecorationRole);
        if (value.isValid() && value.canConvert<QPixmap>()) {
	        QPixmap pixmap = qvariant_cast<QPixmap>(value);
	        // Pixmap をセル全体にピッタリ描画
	        QRect targetRect = option.rect.adjusted(0, 0, 0, 0);
	        painter->drawPixmap(targetRect, pixmap);
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override {
        return QSize(32, 32);
    }

private:
    QPixmap pixmap;
};


MapTable::MapTable(QWidget *pParent, MainWindow *pMainWin, IconTable *pIconTable, int pRowNum, int pColNum)
 :	QTableWidget(pParent), mMainWin(pMainWin), mIconTable(pIconTable),
	mAttr(0), mPixSize(L_PIXSIZE, L_PIXSIZE)
{
	if (pRowNum <= 0) pRowNum = L_DEFALUT_MAPSIZE_ROW;
	if (pColNum <= 0) pColNum = L_DEFALUT_MAPSIZE_COL;
	setRowCount(pRowNum);
	setColumnCount(pColNum);
	setEditTriggers(QAbstractItemView::NoEditTriggers); // 編集不可にする
	setSelectionMode(QAbstractItemView::NoSelection);
	setStyleSheet("QTableView::item:selected { background: #B4D4FF; }"); // 選択時の色（デフォルトの青）
	setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");
	setIconSize(QSize(L_PIXSIZE, L_PIXSIZE));

	setContentsMargins(0, 0, 0, 0);
	QHeaderView *hHeader = horizontalHeader();
	QHeaderView *vHeader = verticalHeader();
	hHeader->setDefaultSectionSize(L_PIXSIZE);
	vHeader->setDefaultSectionSize(L_PIXSIZE);
	hHeader->setMinimumSectionSize(32);
	vHeader->setMinimumSectionSize(32);
	hHeader->setSectionsClickable(false);
	vHeader->setSectionsClickable(false);

	for (int r = 0; r < pRowNum; r++) setRowHeight(r, L_PIXSIZE);
	for (int c = 0; c < pColNum; c++) setColumnWidth(c, L_PIXSIZE);

	setItemDelegate(new PixmapDelegate(this));

	connect(this, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(slot_OnCurrentChanged(int,int,int,int)));
	mData.resize(pRowNum * pColNum);
	fill(mData.begin(), mData.end(), -1);
	viewport()->installEventFilter(this);
}

void MapTable::Init(int pRowNum, int pColNum, vector<int> *pData,
					const Zone *pSelZone, const Point *pCurPos, bool pIsSelect)
{
	ChangeSize(pRowNum, pColNum);
	if (!pData) return;

	bool selFlg1 = false, selFlg2 = false;
	QPixmap pix;
	for (int r = 0; r < pRowNum; r++) {
		for (int c = 0; c < pColNum; c++) {
			QTableWidgetItem *witem = item(r, c);
			if (!witem) {
				witem = new QTableWidgetItem();
				witem->setTextAlignment(Qt::AlignCenter);
				setItem(r, c, witem);
			}
			if (pIsSelect && (mAttr & L_Attr_SelectMode)) {
				selFlg1 = (pSelZone && pSelZone->contains(r, c))? true : false;
				selFlg2 = mSelZone.contains(r, c);
			}
			int idx = r * pColNum + c;
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
}

void MapTable::Close()
{
	int rowNum = rowCount(), colNum = columnCount();
	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < colNum; c++) {
			QTableWidgetItem *witem = item(r, c);
			if (witem) witem->setData(Qt::DecorationRole, QPixmap(""));
		}
	}
	mData.clear();
	mUndoStack.clear();
	mRedoStack.clear();
}

void MapTable::OutputFile(FILE *fp)
{
	FinalizeMove();

	int rowNum = rowCount(), colNum = columnCount();
	fprintf(fp, "%s = %d %d\n", L_KEY_MAPSIZE, rowNum, colNum);
	fprintf(fp, "%s =", L_KEY_MAPDATA);
	for (int r = 0; r < rowNum; r++) {
		if (r > 0) fprintf(fp, "\t");
		for (int c = 0; c < colNum; c++) {
			fprintf(fp, "%3d,", GetIconIdx(r, c)+1);
		}
		if (r < rowNum-1) fprintf(fp, "%s", L_CR);
		fprintf(fp, "\n");
	}
}

bool MapTable::ExportFile(const QString &pFileName)
{
	if (pFileName.isEmpty()) return false;

	int rowNum = rowCount(), colNum = columnCount();
	QImage finalImage(L_PIXSIZE*colNum, L_PIXSIZE*rowNum, QImage::Format_ARGB32);
    finalImage.fill(Qt::white);
    QPainter painter(&finalImage);

	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < colNum; c++) {
			QTableWidgetItem *witem = item(r, c);
			QVariant value = witem->data(Qt::DecorationRole);
			QPixmap pixmap;
	        if (value.isValid() && value.canConvert<QPixmap>()) {
	        	pixmap = qvariant_cast<QPixmap>(value);
	        }
			// 画像を適切な位置に配置
            int x = c * L_PIXSIZE, y = r * L_PIXSIZE;
            painter.drawPixmap(x, y, pixmap);
		}
	}
	painter.end();

	finalImage.save(pFileName, "PNG");
	return true;
}

void MapTable::ChangeSize(int pRowNum, int pColNum)
{
	if (pRowNum <= 0 || pColNum <= 0) return;

	int rowNum = rowCount(), colNum = columnCount();
	if (rowNum != pRowNum || colNum != pColNum) {
		setRowCount(pRowNum);
		setColumnCount(pColNum);
		for (int r = rowNum; r < pRowNum; r++) setRowHeight(r, L_PIXSIZE);
		for (int c = colNum; c < pColNum; c++) setColumnWidth(c, L_PIXSIZE);
	}

	const vector<int> oldData(mData);
//	FinalizeMove();
	if (pRowNum * pColNum > mData.size())
		mData.resize(pRowNum * pColNum, -1);

	for (int r = 0; r < pRowNum; r++) {
		for (int c = 0; c < pColNum; c++) {
			QTableWidgetItem *witem = item(r, c);
			if (!witem) {
				witem = new QTableWidgetItem();
				witem->setTextAlignment(Qt::AlignCenter);
				setItem(r, c, witem);
			}
			int idx1 = r * pColNum + c;
			int idx2 = r * colNum + c;
			mData[idx1] = (idx2 < oldData.size() && r < rowNum && c < colNum)? oldData[idx2] : -1;
		}
	}
	mData.resize(pRowNum * pColNum);
}

void MapTable::SetDrawGrid(bool onoff)
{
	int rowNum = rowCount(), colNum = columnCount();
	setShowGrid(onoff);
	const int pixSize = (onoff)? L_PIXSIZE-1 : L_PIXSIZE;
	QPixmap pix;
	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < colNum; c++) {
			int idx = r * colNum + c;
			int iconIdx = mData[idx];
			if (iconIdx < 0 || !mIconTable->GetPixmap(pix, iconIdx)) continue;
			QTableWidgetItem *witem = item(r, c);
			if (witem) witem->setData(Qt::DecorationRole, pix);
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

int MapTable::GetIconIdx(int row, int col) const
{
	int rowNum = rowCount(), colNum = columnCount();
	if (row < 0 || col < 0 || row >= rowNum || col >= colNum)
		return -1;

	int idx = row * colNum + col;
	return mData[idx];
}

bool MapTable::SetPixmap(int pRow, int pCol, int pIconIdx, bool pIsSelect, bool pIsUpdate)
{
	int rowNum = rowCount(), colNum = columnCount();
	if (pRow < 0 || pCol < 0 || pRow >= rowNum || pCol >= colNum)
		return false;

	QTableWidgetItem *witem = item(pRow, pCol);
	int idx = pRow * colNum + pCol;
	if (pIconIdx < 0) {
		if (!pIsUpdate || mData[idx] == pIconIdx) {
			if (witem) witem->setData(Qt::DecorationRole, QPixmap(""));
			return false;
		}
	}

	QPixmap &pix = mTempPixmap;
	if (mIconTable->GetPixmap(pix, pIconIdx)) {
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
	QTableWidgetItem *witem = item(row, col);
	if (!witem) return;
	if (pShowFlg > 0) {
		witem->setData(Qt::DecorationRole, pixmap);
	} else if (pShowFlg == 0) {
		witem->setData(Qt::DecorationRole, QPixmap(""));
	}
}

void MapTable::DrawPixmapSelZone()
{
	if (mSelZone.empty()) return;
	Zone iconZone;
	if (!mIconTable->GetSelectZone(iconZone)) return;

	int rowNum = M_MAX(mSelZone.rows(), iconZone.rows());
	int colNum = M_MAX(mSelZone.cols(), iconZone.cols());
	int incRow = (mSelZone[0].r < mPressPnt.r)? -1 : 1;
	int incCol = (mSelZone[0].c < mPressPnt.c)? -1 : 1;
	for (int r = mSelZone[0].r; r < mSelZone[0].r+rowNum; r++) {
		for (int c = mSelZone[0].c; c < mSelZone[0].c+colNum; c++) {
			QPixmap pix;
			int ofsRow = (r-mSelZone[0].r) % iconZone.rows();
			int ofsCol = (c-mSelZone[0].c) % iconZone.cols();
			if (!mIconTable->GetCurPixmap(pix, ofsRow, ofsCol)) continue;
			SetPixmap(r, c, pix, true);
		}
	}
}

void MapTable::ResetSelZonePixmap(const Zone *pZone)
{
	for (int r = mSelZone[0].r; r <= mSelZone[1].r && r >= 0; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			if (pZone && pZone->contains(r, c)) continue;
			int idx = r * columnCount() + c;
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
			int idx = r * columnCount() + c;
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
			int idx = r * columnCount() + c;
			SetPixmap(r, c, mData[idx], false, false);
		}
	}

	int rowNum = rowCount(), colNum = columnCount();
	QTableWidgetSelectionRange range(0, 0, rowNum-1, colNum-1);
    setRangeSelected(range, false);
	mPressPnt.init();
	mSelZone.init();
	mMainWin->NotifyEdited();
}

void MapTable::SelectAll()
{
	if ((mAttr & L_Attr_SelectMode) == 0) return;

	int rowNum = rowCount(), colNum = columnCount();
	FinalizeMove();
	mSelZone.init(0, 0, rowNum-1, colNum-1);
	Select();
	QTableWidgetSelectionRange range(mSelZone[0].r, mSelZone[0].c, mSelZone[1].r, mSelZone[1].c);
    setRangeSelected(range, true);
	mMainWin->NotifyEdited();
}

void MapTable::Clear()
{
	if (mSelZone.empty()) return;

	int rowNum = rowCount(), colNum = columnCount();
	FinalizeMove();
	AddUndo(L_OPE_CLEAR);

	for (int r = mSelZone[0].r; r <= mSelZone[1].r; r++) {
		for (int c = mSelZone[0].c; c <= mSelZone[1].c; c++) {
			int idx = r * columnCount() + c;
			SetPixmap(r, c, -1);
		}
	}
	QTableWidgetSelectionRange range(0, 0, rowNum-1, colNum-1);
    setRangeSelected(range, false);
	mSelZone.init();
	mMainWin->NotifyEdited();
}

void MapTable::Move(int pOfsRow, int pOfsCol)
{
	int rowNum = rowCount(), colNum = columnCount();
	if		(mSelZone[0].r + pOfsRow < 0) pOfsRow = -mSelZone[0].r;
	else if (mSelZone[1].r + pOfsRow > rowNum-1) pOfsRow = rowNum-mSelZone[1].r-1;
	if		(mSelZone[0].c + pOfsCol < 0) pOfsCol = -mSelZone[0].c;
	else if (mSelZone[1].c + pOfsCol > colNum-1) pOfsCol = colNum-mSelZone[1].c-1;
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
	QTableWidgetSelectionRange range(0, 0, rowNum-1, colNum-1);
    setRangeSelected(range, false);
	QTableWidgetSelectionRange range2(mSelZone[0].r, mSelZone[0].c, mSelZone[1].r, mSelZone[1].c);
    setRangeSelected(range2, true);
	mAttr |= L_Attr_Moved;
	// Sleep(100);
}

void MapTable::Undo()
{
	FinalizeMove();
	if (mUndoStack.empty()) return;

	Point curPos(currentRow(), currentColumn());
	Journal preJnl = mUndoStack.back();
	mUndoStack.pop_back();
	if (!mUndoStack.empty() && !preJnl.IsSelect()) {
		preJnl.mCurPos = mUndoStack.back().mCurPos;
		preJnl.mSelZone = mUndoStack.back().mSelZone;
	}
	if (preJnl.mOperation == L_OPE_COPY) preJnl.mSelZone.init();

	int rowNum = rowCount(), colNum = columnCount();
	Journal redoJnl(rowNum, colNum, preJnl.mOperation, mData, mSelZone, curPos);
//	printf("undo %zd: (%d %d)\n", mUndoStack.size(), preJnl.mCurPos.r, preJnl.mCurPos.c);
	mRedoStack.push_back(redoJnl);
//	printf("add redo %zd: ", mRedoStack.size()); redoJnl.Dump();
	Init(preJnl.mRowNum, preJnl.mColNum, &preJnl.mData, &preJnl.mSelZone, &preJnl.mCurPos, preJnl.IsSelect());
	mMainWin->NotifyEdited();
}

void MapTable::Redo()
{
	if (mRedoStack.empty()) return;

	Journal &newJnl = mRedoStack.back();
	AddUndo(newJnl.mOperation, &newJnl.mCurPos);
	Init(newJnl.mRowNum, newJnl.mColNum, &newJnl.mData, &newJnl.mSelZone, &newJnl.mCurPos, newJnl.IsSelect());
	mRedoStack.pop_back();
//	printf("redo %zd: (%d %d)\n", mRedoStack.size(), newJnl.mCurPos.r, newJnl.mCurPos.c);
	mMainWin->NotifyEdited();
}

void MapTable::AddUndo(int ope, const Point *pCurPos)
{
	int rowNum = rowCount(), colNum = columnCount();
	Point curPos(currentRow(), currentColumn());
	if (pCurPos) curPos = *pCurPos;
	Journal jnl(rowNum, colNum, ope, mData, mSelZone, curPos);
	mUndoStack.push_back(jnl);
//	printf("add undo %zd: ", mUndoStack.size()); jnl.Dump();
	if (!pCurPos) mRedoStack.clear();
}

bool MapTable::IsSelectAll() const
{
	if ((mAttr & L_Attr_SelectMode) == 0) return false;
	int rowNum = rowCount(), colNum = columnCount();
	return (mSelZone[0].r == 0 && mSelZone[0].c == 0 &&
			mSelZone[1].r == rowNum-1 && mSelZone[1].c == colNum-1)?
			true : false;
}

void MapTable::mousePressEvent(QMouseEvent *event)
{
	if (event->button() != Qt::LeftButton) return;

	QModelIndex index = indexAt(event->pos());  // クリックされたセルのインデックスを取得
	if (!index.isValid()) return;  // 無効なセルなら無視
	int row = index.row(), col = index.column();

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
		DrawPixmapSelZone();
	}
}

void MapTable::GetIconMaxZone(Zone &zone) const
{
	Zone iconZone;
	if (!mIconTable->GetSelectZone(iconZone)) return;
	if (zone.rows() < iconZone.rows()) zone[1].r = zone[0].r + iconZone.rows() - 1;
	if (zone.cols() < iconZone.cols()) zone[1].c = zone[0].c + iconZone.cols() - 1;
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
	GetIconMaxZone(newZone);
	ResetSelZonePixmap(&newZone);

	mSelZone = newZone;
	if (mAttr & L_Attr_SelectMode) {
		Select();
	} else {
		DrawPixmapSelZone();
	}
}

// 入力を確定する
void MapTable::FinalizeInput()
{
	if (mSelZone.empty()) return;
	Zone iconZone;
	if (!mIconTable->GetSelectZone(iconZone)) return;

	bool isMultiIcon = (iconZone.rows() == 1 && iconZone.cols() == 1)? false : true;
	int iconIdx = (isMultiIcon)? -1 : mIconTable->GetCurIconIdx();

	AddUndo(L_OPE_INPUT);

	int rowNum = M_MAX(mSelZone.rows(), iconZone.rows());
	int colNum = M_MAX(mSelZone.cols(), iconZone.cols());
	for (int r = mSelZone[0].r; r < mSelZone[0].r+rowNum; r++) {
		for (int c = mSelZone[0].c; c < mSelZone[0].c+colNum; c++) {
			if (isMultiIcon) {
				int ofsRow = (r-mSelZone[0].r) % iconZone.rows();
				int ofsCol = (c-mSelZone[0].c) % iconZone.cols();
				iconIdx = mIconTable->GetCurIconIdx(ofsRow, ofsCol);
			}
			int idx = r * columnCount() + c;
			mData[idx] = iconIdx;
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
			int idx = r * columnCount() + c;
			mData[idx] = iconIdx;
		}
	}

	// 元の選択領域を初期化する
	if (ope == L_OPE_MOVE) {
		for (int r = mSelZone[0].r+ofsRow; r <= mSelZone[1].r+ofsRow; r++) {
			for (int c = mSelZone[0].c+ofsCol; c <= mSelZone[1].c+ofsCol; c++) {
				if (mSelZone.contains(r, c)) continue;
				int idx = r * columnCount() + c;
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
				mMainWin->NotifyEdited();
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
	if (e->type() == QEvent::KeyPress) {
		const int key = ((QKeyEvent *)e)->key();
		const int mod = ((QKeyEvent *)e)->modifiers();
		if (key == Qt::Key_Delete) {
			if (mAttr & L_Attr_SelectMode) Clear();
		} else if (key == Qt::Key_Escape) {
			if (mAttr & L_Attr_SelectMode) UnSelect();
		} else if (key == Qt::Key_Left || key == Qt::Key_Up || key == Qt::Key_Right || key == Qt::Key_Down) {
			if ((mAttr & L_Attr_SelectMode) && !mSelZone.empty()) {
				int ofsRow = 0, ofsCol = 0;
				if		(key == Qt::Key_Left)  ofsCol = -1;
				else if (key == Qt::Key_Up)	   ofsRow = -1;
				else if (key == Qt::Key_Right) ofsCol = 1;
				else if (key == Qt::Key_Down)  ofsRow = 1;
				Move(ofsRow, ofsCol);
			}
		} else if (key == Qt::Key_A && (mod & Qt::ControlModifier)) {
			if (mAttr & L_Attr_SelectMode) SelectAll();
		} else if (key == Qt::Key_Z && (mod & Qt::ControlModifier)) {
			Undo();
		} else if (key == Qt::Key_Y && (mod & Qt::ControlModifier)) {
			Redo();
		}
	}
	return QTableWidget::event(e);
}
