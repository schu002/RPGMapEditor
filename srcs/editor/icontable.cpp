#include "maindef.h"
#include "icontable_moc.h"

#define L_NUM_ROW	10
#define L_NUM_COL	5

typedef hashMap<string, int>					string2intMap;
typedef hashMap<string, int>::const_iterator	string2intMapCItr;

IconTable::IconTable(QWidget *pParent, MainWindow *pMainWin)
	: QTableWidget(L_NUM_ROW, L_NUM_COL, pParent), mMainWin(pMainWin)
{
	horizontalHeader()->setVisible(false);
	verticalHeader()->setVisible(false); 
	setDragEnabled(false);
	setFixedWidth(L_NUM_COL * (L_PIXSIZE+6) + 20);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::ContiguousSelection);
	setStyleSheet(
		"QTableView::item { border: 0.1px dotted gray; }" // グリッド線
		"QTableView::item:selected { background: orange; }" // 選択時の色（デフォルトの青）
	);

	horizontalHeader()->setDefaultSectionSize(L_PIXSIZE+6);
	verticalHeader()->setDefaultSectionSize(L_PIXSIZE+6);

	viewport()->installEventFilter(this);
}

int IconTable::Init(const string &pDir, const IconDataVector *pDatas, vector<int> *newIconIDVec)
{
	if (pDir.empty()) return 0;

	// 現在の配置を覚えておく
	IconDataMap iconMap;
	const IconDataVector &datas = (pDatas)? *pDatas : mDatas;
	for (int i = 0; i < datas.size(); i++) {
		const IconData &data = datas[i];
		if (!data.mFileName.empty()) iconMap[data] = i;
	}

	Clear();

	bool isSameDir = (mIconDir.empty() || mIconDir == pDir)? true : false;
	mIconDir = pDir;
	if (pDatas) mDatas = *pDatas;

	// pDirフォルダにあるファイルを検索
	stringVector newFiles;
	FileIter fitr(pDir.c_str(), "*.png");
	while (fitr.Next()) {
		const char *fname = fitr.GetFileName();
		IconData data(fname);
		IconDataMapCItr itr = iconMap.find(data);
		if (!pDatas) {
			if (itr != iconMap.end()) data.mTableIdx = itr->first.mTableIdx; // 前回の表示位置
			mDatas.push_back(data);
		} else if (itr == iconMap.end()) {
			mDatas.push_back(data);
		}
	}

	int rowNum = mDatas.size()/columnCount()+3;
	rowNum = M_MAX(rowNum, L_NUM_ROW);

	setRowCount(rowNum);
	const int tableCnt = rowNum * columnCount();
	mTable2ID.resize(tableCnt);
	fill(mTable2ID.begin(), mTable2ID.end(), -1);

	for (int idx = 0; idx < mDatas.size(); idx++) {
		IconData &data = mDatas[idx];
		if (data.mFileName.empty()) continue;
		if (data.mTableIdx < 0) continue;
		mTable2ID[data.mTableIdx] = idx;
	}

	// まだ表示位置が決まっていないファイルはtableIdxの小さいセルから順に入れていく。
	int minTableIdx = -1;
	for (int idx = 0; idx < mDatas.size(); idx++) {
		IconData &data = mDatas[idx];
		if (data.mFileName.empty()) continue;
		if (data.mTableIdx >= 0) continue;
		for (int i = minTableIdx+1; i < mTable2ID.size(); i++) {
			if (mTable2ID[i] < 0) { minTableIdx = i; break; }
		}
		data.mTableIdx = minTableIdx;
		mTable2ID[minTableIdx] = idx;
	}

	// アイコンテーブルに表示する
	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < columnCount(); c++) {
			QTableWidgetItem *witem = item(r, c);
			if (!witem) {
				witem = new QTableWidgetItem();
				witem->setTextAlignment(Qt::AlignCenter);
				setItem(r, c, witem);
			}
	        string fname;
	        int tblIdx = r * columnCount() + c;
	        int iconIdx = mTable2ID[tblIdx];
	        if (iconIdx >= 0) {
	        	const IconData &data = mDatas[iconIdx];
				fname = mIconDir + "/" + data.mFileName;
	        }
			QPixmap pixmap(fname.c_str());
	        witem->setData(Qt::DecorationRole, pixmap);
		}
	}

	if (newIconIDVec && isSameDir) {
		newIconIDVec->resize(mDatas.size());
		fill(newIconIDVec->begin(), newIconIDVec->end(), -1);
		for (int i = 0; i < mDatas.size(); i++) {
			IconData &data = mDatas[i];
			IconDataMapCItr itr = iconMap.find(data);
			if (itr != iconMap.end()) {
				int oldIdx = itr->second;
				(*newIconIDVec)[oldIdx] = i;
			}
		}
	}

	setCurrentCell(0, 0);
	return mDatas.size();
}
	
bool compare_data(IconData &d1, IconData &d2) { return d1.mFileName < d2.mFileName; }

// アイコン番号をpngファイル名順に振り直す
void IconTable::ResetIconIdx(vector<int> &pNewIconIDVec)
{
	sort(mDatas.begin(), mDatas.end(), compare_data);

	pNewIconIDVec.resize(mDatas.size());
	fill(pNewIconIDVec.begin(), pNewIconIDVec.end(), -1);

	for (int idx = 0; idx < mDatas.size(); idx++) {
		IconData &data = mDatas[idx];
		int oldIdx = (data.mTableIdx >= 0)? mTable2ID[data.mTableIdx] : -1;
		if (oldIdx >= 0) {
			pNewIconIDVec[oldIdx] = idx;
			mTable2ID[data.mTableIdx] = idx;
		}
	}
}

void IconTable::Close()
{
	/* int rowNum = rowCount(), colNum = columnCount();
	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < colNum; c++) {
			QTableWidgetItem *witem = item(r, c);
			witem->setData(Qt::DecorationRole, QPixmap(""));
		}
	} */
	clear();
	mIconDir = "";
	mDatas.clear();
	mTable2ID.clear();
}

void IconTable::OutputFile(FILE *fp) const
{
	fprintf(fp, "%s = %s\n", L_KEY_ICONDIR, mIconDir.c_str());

	for (int i = 0; i < mDatas.size(); i++) {
		const IconData &data = mDatas[i];
		fprintf(fp, "%s%d = %s:%d\n", L_KEY_ICONFILE, i+1, data.mFileName.c_str(), data.mTableIdx);
	}
	fprintf(fp, "\n");
}

void IconTable::Clear()
{
	clear();
	mDatas.clear();
	mTable2ID.clear();
}

void IconTable::Move(int pVec)
{
	Zone selZone;
	if (!GetSelectZone(selZone)) return;
	int curRow = currentRow(), curCol = currentColumn();
	int rowNum = rowCount(), colNum = columnCount();
	int selRowNum = selZone.rows(), selColNum = selZone.cols();

	int ofsRow = 0, ofsCol = 0;
	if (pVec == L_VEC_UP) {
		ofsRow = (selZone[0].r > 0)? -1 : rowNum-selRowNum;
	} else if (pVec == L_VEC_DOWN) {
		ofsRow = (selZone[1].r < rowNum-1)? 1 : selRowNum-rowNum;
	} else if (pVec == L_VEC_LEFT) {
		ofsCol = (selZone[0].c > 0)? -1 : colNum-selColNum;
	} else if (pVec == L_VEC_RIGHT) {
		ofsCol = (selZone[1].c < colNum-1)? 1 : selColNum-colNum;
	}
	if (ofsRow == 0 && ofsCol == 0) return;

	int incRow = (ofsRow > 0)? -1 : 1;
	int incCol = (ofsCol > 0)? -1 : 1;
	int sRow = (incRow > 0)? selZone[0].r : selZone[1].r;
	int eRow = (incRow > 0)? selZone[1].r : selZone[0].r;
	int sCol = (incCol > 0)? selZone[0].c : selZone[1].c;
	int eCol = (incCol > 0)? selZone[1].c : selZone[0].c;
	for (int r = sRow; r != eRow+incRow; r += incRow) {
		for (int c = sCol; c != eCol+incCol; c += incCol) {
			moveOne(r, c, ofsRow, ofsCol);
		}
	}
	selZone.move(ofsRow, ofsCol);
	setCurrentCell(curRow+ofsRow, curCol+ofsCol);
	QTableWidgetSelectionRange range(selZone[0].r, selZone[0].c, selZone[1].r, selZone[1].c);
	clearSelection();
	setRangeSelected(range, true);
}

void IconTable::moveOne(int row1, int col1, int ofsRow, int ofsCol)
{
	int row2 = row1+ofsRow, col2 = col1+ofsCol;
	QTableWidgetItem *item1 = item(row1, col1);
	QTableWidgetItem *item2 = item(row2, col2);
	QPixmap pix1, pix2;
	GetPixmap(pix1, row1, col1);
	GetPixmap(pix2, row2, col2);
	// mMovePntと移動先のPixmapを交換する
	item1->setData(Qt::DecorationRole, pix2);
	item2->setData(Qt::DecorationRole, pix1);

	int idx1 = row1 * columnCount() + col1;
	int idx2 = row2 * columnCount() + col2;
	int iconIdx1 = mTable2ID[idx1];
	int iconIdx2 = mTable2ID[idx2];
	if (iconIdx1 >= 0) mDatas[iconIdx1].mTableIdx = idx2;
	if (iconIdx2 >= 0) mDatas[iconIdx2].mTableIdx = idx1;
	mTable2ID[idx1] = iconIdx2;
	mTable2ID[idx2] = iconIdx1;
}

int IconTable::GetSelectRowNum() const
{
	QList<QTableWidgetSelectionRange> ranges = selectedRanges();
	for (const QTableWidgetSelectionRange &range : ranges) {
		return range.bottomRow() - range.topRow() + 1;
	}
	return 0;
}

int IconTable::GetSelectColNum() const
{
	QList<QTableWidgetSelectionRange> ranges = selectedRanges();
	for (const QTableWidgetSelectionRange &range : ranges) {
		return range.rightColumn() - range.leftColumn() + 1;
	}
	return 0;
}

bool IconTable::GetSelectZone(Zone &zone) const
{
	QList<QTableWidgetSelectionRange> ranges = selectedRanges();
	for (const QTableWidgetSelectionRange &range : ranges) {
		zone[0].r = range.topRow();
		zone[1].r = range.bottomRow();
		zone[0].c = range.leftColumn();
		zone[1].c = range.rightColumn();
		return true;
	}
	zone.init();
	return false;
}

int IconTable::GetCurIconIdx(int pOfsRow, int pOfsCol) const
{
	Zone selZone;
	if (!GetSelectZone(selZone)) return -1;
	int row = selZone[0].r+pOfsRow, col = selZone[0].c+pOfsCol;
	if (row < 0 || col < 0 || row >= rowCount() || col >= columnCount())
		return -1;

	int idx = row * columnCount() + col;
	return (idx >= 0 && idx < mTable2ID.size())? mTable2ID[idx] : -1;
}

string IconTable::GetCurIconFileName() const
{
	int iconIdx = GetCurIconIdx();
	return (iconIdx >= 0 && iconIdx < mDatas.size())? mDatas[iconIdx].mFileName : "";
}

bool IconTable::GetPixmap(QPixmap &pixmap, int row, int col) const
{
	QTableWidgetItem *witem = item(row, col);
	if (!witem) return false;
	QVariant value = witem->data(Qt::DecorationRole);
    if (value.isValid() && value.canConvert<QPixmap>()) {
    	pixmap = qvariant_cast<QPixmap>(value);
    	return true;
    }
	return false;
}

bool IconTable::GetPixmap(QPixmap &pPixmap, int pIconIdx) const
{
	int tableIdx = (pIconIdx >= 0 && pIconIdx < mDatas.size())? mDatas[pIconIdx].mTableIdx : -1;
	if (tableIdx < 0) return false;

	int r = tableIdx / columnCount(), c = tableIdx % columnCount();
	return GetPixmap(pPixmap, r, c);
}

bool IconTable::GetCurPixmap(QPixmap &pPixmap, int pOfsRow, int pOfsCol) const
{
	Zone selZone;
	if (!GetSelectZone(selZone)) return false;
	int row = selZone[0].r+pOfsRow, col = selZone[0].c+pOfsCol;
	if (row < 0 || col < 0 || row >= rowCount() || col >= columnCount())
		return false;
	return GetPixmap(pPixmap, row, col);
}

bool IconTable::eventFilter(QObject *obj, QEvent *e)
{
	if (obj != viewport()) return false;
	if (e->type() != QEvent::MouseButtonRelease) return false;
	if (((QMouseEvent *)e)->button() != Qt::LeftButton) return false;

	QModelIndex index = indexAt(((QMouseEvent *)e)->pos());
	if (index.isValid()) {
		mMainWin->NotifyCurIconChanged();
	}
	return false;
}
