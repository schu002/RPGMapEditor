#include "maindef.h"
#include "icontable_moc.h"

#define L_NUM_ROW		10
#define L_NUM_COL		5
#define L_PIXSIZE		32

IconTable::IconTable(QWidget *pParent, MainWindow *pMainWin)
	: QTable(L_NUM_ROW, L_NUM_COL, pParent), mMainWin(pMainWin)
{
	setLeftMargin(0);
	setTopMargin(0);
	setReadOnly(true);
	setDragEnabled(false);
	setSelectionMode(QTable::Single);
	setHScrollBarMode(QScrollView::AlwaysOff);
	setFixedWidth(L_NUM_COL * L_PIXSIZE + 25);

	for (int r = 0; r < L_NUM_ROW; r++) {
		setRowHeight(r, L_PIXSIZE);
		setRowReadOnly(r, true);
	}
	for (int c = 0; c < L_NUM_COL; c++) {
		setColumnWidth(c, L_PIXSIZE);
		setColumnReadOnly(c, true);
	}

	connect(this, SIGNAL(currentChanged(int,int)), this, SLOT(slot_OnCurrentChanged(int,int)));
}

IconTable::~IconTable()
{
}

int IconTable::OpenIconFiles(const string &pIconDir)
{
	if (pIconDir.empty()) return 0;

	Clear();

	FileIter fitr(pIconDir.c_str(), "*.png");
	while (fitr.Next()) {
		const char *fname = fitr.GetFileName();
		if (fname) mFiles.push_back(fname);
	}
	if (mFiles.empty()) return 0;

	int rowNum = mFiles.size()/numCols()+1, cnt = 0;
	setNumRows(rowNum);
	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < numCols(); c++) {
			if (cnt >= mFiles.size()) break;
			// printf("%d %d : %s\n", r, c, mFiles[cnt].c_str());
			string fname = pIconDir + "/" + mFiles[cnt++];
			setPixmap(r, c, QPixmap(fname.c_str()));
		}
		if (cnt >= mFiles.size()) break;
	}
	setCurrentCell(0, 0);
	slot_OnCurrentChanged(0, 0);
	return mFiles.size();
}

void IconTable::Clear()
{
	for (int r = 0; r < numRows(); r++) {
		for (int c = 0; c < numCols(); c++) {
			clearCell(r, c);
		}
	}
	mFiles.clear();
}

void IconTable::slot_OnCurrentChanged(int row, int col)
{
	if (mMainWin) mMainWin->NotifyCurIconChanged();
}

int IconTable::GetCurIndex() const
{
	int row = currentRow(), col = currentColumn();
	return row * numCols() + col;
}

string IconTable::GetCurIconFileName() const
{
	int idx = GetCurIndex();
	return (idx < 0 || idx >= mFiles.size())? "" : mFiles[idx];
}

bool IconTable::GetPixmap(QPixmap &pPixmap, int pIconIdx) const
{
	if (pIconIdx < 0 && pIconIdx >= mFiles.size()) return false;

	int r = pIconIdx / numCols(), c = pIconIdx % numCols();
	pPixmap = pixmap(r, c);
	return true;
}
