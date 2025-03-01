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
	: QTableWidget(L_NUM_ROW, L_NUM_COL, pParent), mMainWin(pMainWin), mAttr(0)
{
	horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false); 
//	setReadOnly(true);
	setDragEnabled(false);
//	setSelectionMode(QTable::Single);
//	setHScrollBarMode(QScrollView::AlwaysOff);
	setFixedWidth(L_NUM_COL * L_PIXSIZE + 25);
	// setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setStyleSheet(
		"QTableView::item { border: 0.1px dotted gray; }" // グリッド線
		"QTableView::item:selected { background: #B4D4FF; }" // 選択時の色（デフォルトの青）
	);

	for (int r = 0; r < L_NUM_ROW; r++) {
		setRowHeight(r, L_PIXSIZE);
	}
	for (int c = 0; c < L_NUM_COL; c++) {
		setColumnWidth(c, L_PIXSIZE);
	}

	connect(this, &QTableWidget::currentCellChanged, this, &IconTable::slot_OnCurrentChanged);
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

bool IconTable::eventFilter(QObject *obj, QEvent *e)
{
	if (obj != viewport()) return false;
	if (e->type() != QEvent::MouseButtonPress &&
		e->type() != QEvent::MouseButtonRelease) return false;

	QModelIndex index = indexAt(((QMouseEvent *)e)->pos());
	if (!index.isValid()) return false;

	const int r = index.row(), c = index.column();
	if (e->type() == QEvent::MouseButtonPress) {
		mMovePnt.init(r, c);
		mAttr |= L_Attr_MousePress;
	} else if (e->type() == QEvent::MouseButtonRelease) {
		if ((mAttr & L_Attr_MousePress) == 0) return false;
		if (mMovePnt.empty()) return false;
		if (mAttr & L_Attr_Swap) {
			mMainWin->NotifyIconEdited();
		} else {
			mMainWin->NotifyCurIconChanged();
		}
		mAttr &= ~(L_Attr_MousePress|L_Attr_Swap);
	}
	return false;
}

void IconTable::slot_OnCurrentChanged(int row, int col)
{
	if ((mAttr & L_Attr_MousePress) == 0) return;
	if (mMovePnt.empty()) return;
	if (mMovePnt.r == row && mMovePnt.c == col) return;
	// printf("current (%d %d)\n", row, col);

	QLabel *label1 = qobject_cast<QLabel *>(cellWidget(mMovePnt.r, mMovePnt.c));
	QLabel *label2 = qobject_cast<QLabel *>(cellWidget(row, col));
	QPixmap pix1 = (label1)? label1->pixmap(Qt::ReturnByValue) : QPixmap("");
	QPixmap pix2 = (label2)? label2->pixmap(Qt::ReturnByValue) : QPixmap("");
	// mMovePntと移動先のPixmapを交換する
	if (label2) label2->setPixmap(pix1);
	if (label1) label1->setPixmap(pix2);

	int idx1 = mMovePnt.r * columnCount() + mMovePnt.c;
	int idx2 = row * columnCount() + col;
	int iconIdx1 = mTable2ID[idx1];
	int iconIdx2 = mTable2ID[idx2];
	if (iconIdx1 >= 0) mID2Table[iconIdx1] = idx2;
	if (iconIdx2 >= 0) mID2Table[iconIdx2] = idx1;
	mTable2ID[idx1] = iconIdx2;
	mTable2ID[idx2] = iconIdx1;

	mMovePnt.init(row, col);
	mAttr |= L_Attr_Swap;
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
