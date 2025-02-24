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
	setFixedWidth(L_NUM_COL * L_PIXSIZE + 25);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
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
	int cnt = 0, emptyIdx = mFiles.size();
	stringVector newFiles;
	FileIter fitr(pDir.c_str(), "*.png");
	while (fitr.Next()) {
		const char *fname = fitr.GetFileName();
		if (fileSet.empty()) {
			mFiles.push_back(fname);
		} else if (fileSet.find(fname) == fileSet.end()) {
			if (mFiles[cnt].empty()) mFiles[cnt] = fname;
			else newFiles.push_back(fname);
		}
		if (emptyIdx == mFiles.size() && mFiles[cnt].empty())
			emptyIdx = cnt;
		++cnt;
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

	// アイコンテーブルに表示する
	cnt = -1;
	int rowNum = mFiles.size()/columnCount()+1;
	int fileCnt = 0, tableCnt = 0;
	setRowCount(rowNum);
	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < columnCount(); c++) {
			if (++cnt >= mFiles.size()) break;
			int idx = r * columnCount() + c;
			int iconIdx = (pTable && idx < pTable->size())? (*pTable)[idx] : cnt;
			// if (iconIdx < 0) iconIdx = cnt;
			if (iconIdx >= 0 && mFiles[iconIdx].empty()) iconIdx = -1;
			mTable.push_back(iconIdx);
			if (iconIdx >= 0) {
				fileCnt = idx+1;
				tableCnt = mTable.size();
			}
			string fname = (iconIdx < 0)? "" : pDir + "/" + mFiles[iconIdx];
			QPixmap pixmap(fname.c_str());
			QLabel *label = new QLabel(this);
			if (!fname.empty()) label->setPixmap(pixmap.scaled(L_PIXSIZE, L_PIXSIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)); 
            label->setAlignment(Qt::AlignCenter);
            setCellWidget(r, c, label);
		}
	}
	setCurrentCell(0, 0);
	mFiles.resize(fileCnt);
	mTable.resize(tableCnt);
	return mFiles.size();
}
	
void IconTable::OutputFile(FILE *fp)
{
	fprintf(fp, "%s = %s\n", L_KEY_ICONDIR, mIconDir.c_str());
	for (int i = 0; i < mFiles.size(); i++) {
		fprintf(fp, "%s%d = %s\n", L_KEY_ICONFILE, i+1, mFiles[i].c_str());
	}
	for (int idx = 0; idx < mTable.size(); idx++) {
		int r = idx/columnCount(), c = idx % columnCount();
		if (c == 0) fprintf(fp, (r == 0)? "IconTable =" : "\t");
		fprintf(fp, "%3d,", mTable[idx]+1);
		if (c == columnCount()-1) {
			if (idx < mTable.size()-1) fprintf(fp, "%s\n", L_CR);
		}
	}
	fprintf(fp, "\n");
}

void IconTable::Clear()
{
	clear();
	mFiles.clear();
	mTable.clear();
}

void IconTable::slot_OnCurrentChanged(int row, int col)
{
	if (mMainWin) mMainWin->NotifyCurIconChanged();
}

int IconTable::GetCurIndex() const
{
	int row = currentRow(), col = currentColumn();
	return row * columnCount() + col;
}

string IconTable::GetCurIconFileName() const
{
	int idx = GetCurIndex();
	return (idx < 0 || idx >= mFiles.size())? "" : mFiles[idx];
}

bool IconTable::GetPixmap(QPixmap &pPixmap, int pIconIdx) const
{
	if (pIconIdx < 0 && pIconIdx >= mFiles.size()) {
		// pPixmap.fill(Qt::white);
		return false;
	}

	int r = pIconIdx / columnCount(), c = pIconIdx % columnCount();
	QLabel *label = qobject_cast<QLabel*>(cellWidget(r, c));
	if (!label) return false;
	pPixmap = label->pixmap(Qt::ReturnByValue);
	return true;
}
