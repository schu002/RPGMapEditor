#include "maindef.h"
#include "icontable_moc.h"

#define L_KEY_ICONDIR	"IconDir"
#define L_KEY_ICONFILE	"IconFile"
#define L_KEY_ICONTABLE	"IconTable"
#define L_CR			"<cr>"
#define L_NUM_ROW		10
#define L_NUM_COL		5
#define L_PIXSIZE		32

IconTable::IconTable(QWidget *pParent, MainWindow *pMainWin)
	: QTableWidget(L_NUM_ROW, L_NUM_COL, pParent), mMainWin(pMainWin)
{
	horizontalHeader()->setVisible(false);
	verticalHeader()->setVisible(false); 
//	setReadOnly(true);
	setDragEnabled(false);
//	setSelectionMode(QTable::Single);
//	setHScrollBarMode(QScrollView::AlwaysOff);
	setFixedWidth(L_NUM_COL * (L_PIXSIZE+3) + 25);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::ContiguousSelection);
	setStyleSheet(
		"QTableView::item { border: 0.1px dotted gray; }" // グリッド線
		"QTableView::item:selected { background: orange; }" // 選択時の色（デフォルトの青）
	);

	for (int r = 0; r < L_NUM_ROW; r++) {
		setRowHeight(r, L_PIXSIZE+6);
	}
	for (int c = 0; c < L_NUM_COL; c++) {
		setColumnWidth(c, L_PIXSIZE+6);
	}

	viewport()->installEventFilter(this);
}

int IconTable::Init(const string &pDir, const stringVector *pFiles, const vector<int> *pTable)
{
	if (pDir.empty()) return 0;

	Clear();

	mIconDir = pDir;
	stringSet fileSet;
	// 設定ファイルのアイコンファイル名
	if (pFiles) {
		mFiles = *pFiles;
		for (int i = 0; i < mFiles.size(); i++) {
			if (!mFiles[i].empty()) fileSet.insert(mFiles[i]);
		}
	}

	// pDirフォルダにあるファイルを検索
	int fileNum = 0, emptyIdx = mFiles.size();
	stringVector newFiles;
	FileIter fitr(pDir.c_str(), "*.png");
	while (fitr.Next()) {
		const char *fname = fitr.GetFileName();
		if (fileSet.empty()) {
			mFiles.push_back(fname);
		} else if (fileSet.find(fname) == fileSet.end()) {
			if (mFiles[fileNum].empty()) mFiles[fileNum] = fname;
			else newFiles.push_back(fname);
		}
		if (emptyIdx == mFiles.size() && mFiles[fileNum].empty())
			emptyIdx = fileNum;
		++fileNum;
	}

	// 新たに見つかったファイルを追加
	for (int i = 0; i < newFiles.size(); i++) {
		bool addFlg = false;
		const string &fname = newFiles[i];
		while (emptyIdx < mFiles.size()) {
			if (mFiles[emptyIdx].empty()) {
				mFiles[emptyIdx++] = fname;
				addFlg = true;
				break;
			}
			emptyIdx++;
		}
		if (!addFlg) mFiles.push_back(fname);
	}

	int rowNum = fileNum/columnCount()+3;
	if (rowNum < mFiles.size()/columnCount()) rowNum = mFiles.size()/columnCount();
	const int tableCnt = rowNum * columnCount();
	mID2Table.resize(tableCnt);
	mTable2ID.resize(tableCnt);
	fill(mID2Table.begin(), mID2Table.end(), -1);
	fill(mTable2ID.begin(), mTable2ID.end(), -1);

	// アイコンテーブルに表示する
	setRowCount(rowNum);
	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < columnCount(); c++) {
			int idx = r * columnCount() + c;
			if (idx >= mFiles.size()) break;
			int iconIdx = (pTable && idx < pTable->size())? (*pTable)[idx] : idx;
			if (iconIdx >= 0 && mFiles[iconIdx].empty()) iconIdx = -1;
			mTable2ID[idx] = iconIdx;
			string fname = (iconIdx < 0)? "" : pDir + "/" + mFiles[iconIdx];
			QPixmap pixmap(fname.c_str());
			QLabel *label = new QLabel(this);
			if (!fname.empty()) {
				label->setPixmap(pixmap.scaled(L_PIXSIZE, L_PIXSIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		        label->setAlignment(Qt::AlignCenter);
		        mID2Table[iconIdx] = idx;
			}
		    setCellWidget(r, c, label);
		}
	}
	setCurrentCell(0, 0);
	return mFiles.size();
}
	
void IconTable::Close()
{
	int rowNum = rowCount(), colNum = columnCount();
	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < colNum; c++) {
			QLabel *label = qobject_cast<QLabel *>(cellWidget(r, c));
			if (label) label->setPixmap(QPixmap(""));
		}
	}
	mIconDir = "";
	mFiles.clear();
	mTable2ID.clear();
	mID2Table.clear();
}

void IconTable::OutputFile(FILE *fp)
{
	fprintf(fp, "%s = %s\n", L_KEY_ICONDIR, mIconDir.c_str());
	for (int i = 0; i < mFiles.size(); i++) {
		fprintf(fp, "%s%d = %s\n", L_KEY_ICONFILE, i+1, mFiles[i].c_str());
	}
	for (int idx = 0; idx < mTable2ID.size(); idx++) {
		int r = idx/columnCount(), c = idx % columnCount();
		if (c == 0) fprintf(fp, (r == 0)? "IconTable =" : "\t");
		fprintf(fp, "%3d,", mTable2ID[idx]+1);
		if (c == columnCount()-1) {
			if (idx < mTable2ID.size()-1) fprintf(fp, "%s\n", L_CR);
		}
	}
	fprintf(fp, "\n");
}

void IconTable::Clear()
{
	clear();
	mFiles.clear();
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
	QLabel *label1 = qobject_cast<QLabel *>(cellWidget(row1, col1));
	QLabel *label2 = qobject_cast<QLabel *>(cellWidget(row2, col2));
	QPixmap pix1 = (label1)? label1->pixmap(Qt::ReturnByValue) : QPixmap("");
	QPixmap pix2 = (label2)? label2->pixmap(Qt::ReturnByValue) : QPixmap("");
	// mMovePntと移動先のPixmapを交換する
	if (label2) label2->setPixmap(pix1);
	if (label1) label1->setPixmap(pix2);

	int idx1 = row1 * columnCount() + col1;
	int idx2 = row2 * columnCount() + col2;
	int iconIdx1 = mTable2ID[idx1];
	int iconIdx2 = mTable2ID[idx2];
	if (iconIdx1 >= 0) mID2Table[iconIdx1] = idx2;
	if (iconIdx2 >= 0) mID2Table[iconIdx2] = idx1;
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

int IconTable::GetCurIconIdx() const
{
	int row = currentRow(), col = currentColumn();
	int idx = row * columnCount() + col;
	return (idx >= 0 && idx < mTable2ID.size())? mTable2ID[idx] : -1;
}

string IconTable::GetCurIconFileName() const
{
	int iconIdx = GetCurIconIdx();
	return (iconIdx >= 0 && iconIdx < mFiles.size())? mFiles[iconIdx] : "";
}

bool IconTable::GetPixmap(QPixmap &pPixmap, int pIconIdx) const
{
	int tableIdx = (pIconIdx >= 0 && pIconIdx < mID2Table.size())? mID2Table[pIconIdx] : -1;
	if (tableIdx < 0) {
		// pPixmap.fill(Qt::white);
		return false;
	}

	int r = tableIdx / columnCount(), c = tableIdx % columnCount();
	QLabel *label = qobject_cast<QLabel*>(cellWidget(r, c));
	if (!label) return false;
	pPixmap = label->pixmap(Qt::ReturnByValue);
	return true;
}

bool IconTable::GetCurPixmap(vector<SelectInfo> &pIconList) const
{
	Zone selZone;
	if (!GetSelectZone(selZone)) {
		pIconList.clear();
		return false;
	}

	int cnt = 0;
	pIconList.resize(selZone.rows() * selZone.cols());
	for (int r = selZone[0].r; r <= selZone[1].r; r++) {
		for (int c = selZone[0].c; c <= selZone[1].c; c++) {
			SelectInfo &selInfo = pIconList[cnt++];
			int idx = r * columnCount() + c;
			selInfo.mIconIdx = (idx >= 0 && idx < mTable2ID.size())? mTable2ID[idx] : -1;
			QLabel *label = qobject_cast<QLabel*>(cellWidget(r, c));
			selInfo.mPixmap = (label)? label->pixmap(Qt::ReturnByValue) : QPixmap("");
		}
	}
	return true;
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
