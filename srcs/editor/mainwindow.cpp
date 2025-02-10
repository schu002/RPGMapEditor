#include "maindef.h"	
#include "mainwindow_moc.h"

// マップデータファイルで使用するキー
#define L_KEY_ICONDIR	"IconDir"
#define L_KEY_ICONFILE	"IconFile"
#define L_KEY_ICONSIZE	"IconSize"
#define L_KEY_MAPSIZE	"MapSize"
#define L_KEY_MAPDATA	"MapData"
#define L_CR			"<cr>"

namespace {
// トリミング関数
const string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
string rtrim(const string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
string trim(const std::string &s)
{
    return rtrim(ltrim(s));
}

};

MainWindow::MainWindow(const string &pCurDir, WFlags pFlag)
 :	QMainWindow(NULL, FALSE, pFlag), mFileMenu(NULL), mEditMenu(NULL),
	mMenuUndo(-1), mMenuRedo(-1), mMenuGrid(-1), mMenuSelect(-1),
	mSaveBtn(NULL), mSelectBtn(NULL), mGridBtn(NULL),
	mUndoBtn(NULL), mRedoBtn(NULL), mSettingBtn(NULL),
	mIconTable(NULL), mMapTable(NULL), mDataDir(pCurDir), mIsModified(false)
{
	mFileName = Message::TrC(MG_DefaultDataFile);

	// アイコン
	QPixmap iconApp("./lib/app.png");
	QPixmap iconOpen("./lib/fileopen.png");
	QPixmap iconSave("./lib/filesave.png");
	QPixmap iconClear("./lib/clear.png");
	QPixmap iconCopy("./lib/copy.png");
	QPixmap iconUndo("./lib/undo.png");
	QPixmap iconRedo("./lib/redo.png");
	QPixmap iconSelect("./lib/select.png");
	QPixmap iconGrid("./lib/grid.png");
	QPixmap iconSetup("./lib/setup.png");

	setIcon(iconApp);

	//Menu Bar
	mFileMenu = new QPopupMenu(this, "File");
	mFileMenu->insertItem(iconOpen, M_QSTR(Message::TrC(MG_File_Open)), this, SLOT(Open()), CTRL+Key_O);
	mFileMenu->insertItem(iconSave, M_QSTR(Message::TrC(MG_File_Save)), this, SLOT(Save()), CTRL+Key_S);
	mFileMenu->insertSeparator();
	mFileMenu->insertItem(M_QSTR(Message::TrC(MG_Exit)),  this, SLOT(Exit()));
	menuBar()->insertItem(M_QSTR(Message::TrC(MG_File)), mFileMenu);

	mEditMenu = new QPopupMenu(this, "Edit");
	mMenuUndo = mEditMenu->insertItem(iconUndo, M_QSTR(Message::TrC(MG_Undo)), this, SLOT(Undo()), CTRL+Key_Z);
	mMenuRedo = mEditMenu->insertItem(iconRedo, M_QSTR(Message::TrC(MG_Redo)), this, SLOT(Redo()), CTRL+Key_R);
	mEditMenu->insertSeparator();
	mMenuGrid = mEditMenu->insertItem(iconGrid, M_QSTR(Message::TrC(MG_ShowGrid) + Message::TrC(MG_ON)),
		this, SLOT(OnGridMenu()), CTRL+Key_G);
	mEditMenu->setItemChecked(mMenuGrid, true);
	mMenuSelect = mEditMenu->insertItem(iconSelect, M_QSTR(Message::TrC(MG_SelectMode) + Message::TrC(MG_COLON) + Message::TrC(MG_OFF)),
		this, SLOT(OnSelectMenu()), Key_S);
	mEditMenu->insertSeparator();
	mEditMenu->insertItem(iconSetup, M_QSTR(Message::TrC(MG_Setting)), this, SLOT(Setting()));
	menuBar()->insertItem(M_QSTR(Message::TrC(MG_Edit)), mEditMenu);

	//Tool Button
	QToolBar *toolbar = new QToolBar(this);
	toolbar->setMovingEnabled(FALSE);

	toolbar->addSeparator();
	QToolButton *openBtn = new QToolButton(iconOpen, M_QSTR(Message::TrC(MG_File_Open)),
					"", this, SLOT(Open()), toolbar, "Open");
	mSaveBtn = new QToolButton(iconSave, M_QSTR(Message::TrC(MG_File_Save)),
					"", this, SLOT(Save()), toolbar, "Save");
	toolbar->addSeparator();

	mUndoBtn = new QToolButton(iconUndo, M_QSTR(Message::TrC(MG_Undo)),
					"", this, SLOT(Undo()), toolbar, "Undo");
	mRedoBtn = new QToolButton(iconRedo, M_QSTR(Message::TrC(MG_Redo)),
					"", this, SLOT(Redo()), toolbar, "Redo");
	toolbar->addSeparator();

	mGridBtn = new QToolButton(iconGrid, M_QSTR(Message::TrC(MG_ShowGrid) + Message::TrC(MG_ON)),
					"", this, SLOT(OnGridBtn()), toolbar, "Grid");
	mGridBtn->setToggleButton(true);
	mGridBtn->setOn(true);
	mSelectBtn = new QToolButton(iconSelect, M_QSTR(Message::TrC(MG_SelectMode) + Message::TrC(MG_COLON) + Message::TrC(MG_OFF)),
					"", this, SLOT(OnSelectBtn()), toolbar, "Select");
	mSelectBtn->setToggleButton(true);
	mSelectBtn->setOn(false);
	toolbar->addSeparator();

	mSettingBtn = new QToolButton(iconSetup, M_QSTR(Message::TrC(MG_Setting)),
					"", this, SLOT(Setting()), toolbar, "Setting");

	QWidget *central = new QWidget(this);
	setCentralWidget(central);
	QHBoxLayout *topLayout = new QHBoxLayout(central, 10, 10);
	QVBoxLayout *vBoxLayout = new QVBoxLayout(topLayout, 5);
	QHBoxLayout *hBoxLayout = new QHBoxLayout(vBoxLayout, 5);

	mCurPixmap = new QLabel(central);
	mCurPixmap->setFixedWidth(32);
	mCurPixmap->setFixedHeight(32);
	hBoxLayout->addWidget(mCurPixmap);
	mCurPixName = new QLabel(central);
	mCurPixName->setFixedWidth(150);
	mCurPixName->setFixedHeight(20);
	hBoxLayout->addWidget(mCurPixName);

	mIconTable = new IconTable(central, this);
	vBoxLayout->addWidget(mIconTable);

	mMapTable = new MapTable(central, this);
	topLayout->addWidget(mMapTable);

	QLabel *dmyLbl1 = new QLabel(central);
	dmyLbl1->setFixedWidth(0);
	topLayout->addWidget(dmyLbl1);
	QLabel *dmyLbl2 = new QLabel(central);
	dmyLbl2->setFixedWidth(0);
	topLayout->addWidget(dmyLbl2);

	// mIconTable->ClearAll();
	statusBar()->message(M_QSTR(Message::TrC(MG_IconFileNotRegist)));

	LoadMapFile();
	SetUndoEnable(false);
	SetRedoEnable(false);
}

void MainWindow::Open()
{
	QString fname = QFileDialog::getOpenFileName(
                    mDataDir.data(), M_QSTR(Message::TrC(MG_MapDataFile)),
                    this, "Open", M_QSTR(Message::TrC(MG_OpenMapFile)));
	if (fname.isEmpty()) return;

	string str = fname.local8Bit().data();
	int idx = str.rfind('/');
	if (idx < 1) return;
	mDataDir = str.substr(0, idx);
	mFileName = str.substr(idx+1, str.size()-idx-1);
	LoadMapFile();
}

/*　保存　*/
void MainWindow::Save()
{
	WriteFile(mFileName);
}

/*　名前をつけて保存　*/
bool MainWindow::SaveAs()
{
	return FALSE;
}

/*　終了　*/
void MainWindow::Exit()
{
	if (mIsModified) {
		int ret = QMessageBox::question(this, M_QSTR(Message::TrC(MG_Confirm)),
								M_QSTR(Message::TrC(MG_ConfirmSave)),
								QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel) return;
		if (ret == QMessageBox::Yes) Save();
	}
	qApp->quit();
}

/*　元に戻す　*/
void MainWindow::Undo()
{
	if (!mMapTable->Undo()) {
		mEditMenu->setItemEnabled(mMenuUndo, false);
		mUndoBtn->setEnabled(false);
		mIsModified = false;
		SetTitle();
	}
	mEditMenu->setItemEnabled(mMenuRedo, true);
	mRedoBtn->setEnabled(true);
}

/*　やり直す　*/
void MainWindow::Redo()
{
	if (!mMapTable->Redo()) {
		mEditMenu->setItemEnabled(mMenuRedo, false);
		mRedoBtn->setEnabled(false);
	}
	mEditMenu->setItemEnabled(mMenuUndo, true);
	mUndoBtn->setEnabled(true);
	mIsModified = true;
	SetTitle();
}

void MainWindow::SetUndoEnable(bool onoff)
{
	mEditMenu->setItemEnabled(mMenuUndo, onoff);
	mUndoBtn->setEnabled(onoff);
}
	
void MainWindow::SetRedoEnable(bool onoff)
{
	mEditMenu->setItemEnabled(mMenuRedo, onoff);
	mRedoBtn->setEnabled(onoff);
}
	
void MainWindow::SetTitle()
{
	string title = string(Message::TrC(MG_RPGMap_Editor)) + "  -  " + mFileName;
	if (mIsModified) title += " *";
	title += string("  [") + mDataDir + string("]");
	// title = wkstr;
	setCaption(M_QSTR(title));
}

/*　グリッド表示　*/
void MainWindow::OnGridMenu()
{
	bool onoff = (mEditMenu->isItemChecked(mMenuGrid))? false : true;
	mEditMenu->setItemChecked(mMenuGrid, onoff);
	mGridBtn->setOn(onoff);
	_OnGrid(onoff);
}

void MainWindow::OnGridBtn()
{
	bool onoff = mGridBtn->isOn();
	mEditMenu->setItemChecked(mMenuGrid, onoff);
	_OnGrid(onoff);
}

void MainWindow::_OnGrid(bool onoff)
{
	if (!mMapTable) return;
	mMapTable->setShowGrid(onoff);
	int id = (onoff)? MG_ON : MG_OFF;
	string msg = Message::TrC(MG_ShowGrid) + Message::TrC(id);
	QToolTip::add(mGridBtn, M_QSTR(msg));
	mEditMenu->changeItem(mMenuGrid, M_QSTR(msg));
	statusBar()->message(M_QSTR(msg));
}

/*　選択モード　*/
void MainWindow::OnSelectMenu()
{
	bool onoff = (mEditMenu->isItemChecked(mMenuSelect))? false : true;
	mEditMenu->setItemChecked(mMenuSelect, onoff);
	mSelectBtn->setOn(onoff);
	_OnSelect(onoff);
}

void MainWindow::OnSelectBtn()
{
	bool onoff = mSelectBtn->isOn();
	mEditMenu->setItemChecked(mMenuSelect, onoff);
	_OnSelect(onoff);
}

void MainWindow::_OnSelect(bool onoff)
{
	if (!mMapTable) return;
	mCurPixmap->setEnabled(!onoff);
	mCurPixName->setEnabled(!onoff);
	mIconTable->setEnabled(!onoff);
	mMapTable->SetSelectMode(onoff);

	int id = (onoff)? MG_ON : MG_OFF;
	string msg = Message::TrC(MG_SelectMode) + Message::TrC(MG_COLON) + Message::TrC(id);
	QToolTip::add(mSelectBtn, M_QSTR(msg));
	mEditMenu->changeItem(mMenuSelect, M_QSTR(msg));
	id = (onoff)? MG_SelectMode : MG_InputMode;
	statusBar()->message(M_QSTR(Message::TrC(id)));
}

/*　設定　*/
void MainWindow::Setting()
{
	SettingDlg *dlg = new SettingDlg(this);
	int rowNum = mMapTable->GetRowNum(), colNum = mMapTable->GetColNum();
	dlg->Init(mIconDir, rowNum, colNum);
	if (dlg->exec() != QDialog::Accepted) return;

	rowNum = dlg->GetMapSizeRow();
	colNum = dlg->GetMapSizeCol();
	mMapTable->Init(rowNum, colNum);
	ChangeIconDir(dlg->GetIconDir().latin1());
	mIsModified = true;
	SetTitle();
}

void MainWindow::ChangeIconDir(const char *pIconDir, bool pMsgFlg)
{
	mIconDir = pIconDir;
	int imgCnt = mIconTable->OpenIconFiles(mIconDir);
	if (imgCnt == 0) {
		mCurPixmap->setPixmap(QPixmap());
		mCurPixName->setText("");
		statusBar()->message(M_QSTR(Message::TrC(MG_IconFileNotRegist)));
		QMessageBox mb(M_QSTR(Message::TrC(MG_Error)),
			M_QSTR(Message::TrC(MG_ImageFileNotFound, pIconDir)),
			QMessageBox::Critical, QMessageBox::Ok|QMessageBox::Default, 
			QMessageBox::NoButton, QMessageBox::NoButton);
		mb.exec();
		mIconDir = "";
	} else {
		if (pMsgFlg) statusBar()->message(M_QSTR(Message::TrC(MG_IconFileLoaded, imgCnt)), 3000);
	}
}

/*
	ファイルを読み込む
*/
int MainWindow::LoadMapFile()
{
	string fname = mDataDir + "/" + mFileName;
	FILE *fp = fopen(fname.c_str(), "r");
	if (!fp) return 0;

	int rowNum = L_DEFALUT_MAPSIZE_ROW, colNum = L_DEFALUT_MAPSIZE_COL;
	bool crFlg = false;
	string key, val;
	vector<int> mapData;
	char buf[4096];
	while (fgets(buf, 4096, fp) != NULL) {
		string str = buf;
		if (!crFlg) {
			int idx = str.find('=');
			if (idx < 1) continue;
			key = str.substr(0, idx);
			key = trim(key);
			if (key.empty()) continue;
			val = str.substr(idx+1, str.size()-idx-1);
			val = trim(val);
		} else {
			val = trim(str);
		}
		int len = val.size();
		crFlg = (val.substr(len-4, len-1) == L_CR)? true : false;
		if (crFlg) val = val.substr(0, len-5);
		// printf("key = (%s) val = (%s)\n", key.c_str(), val.c_str());
		if (key == L_KEY_ICONDIR) {
			ChangeIconDir(val.c_str(), false);
		} else if (key == L_KEY_MAPSIZE) {
			sscanf(val.c_str(), "%d %d", &rowNum, &colNum);
		} else if (key == L_KEY_MAPDATA) {
			ReadMapData(mapData, val);
		}
	}
	fclose(fp);
	mMapTable->Init(rowNum, colNum, &mapData);
	mIsModified = false;
	SetTitle();
	statusBar()->message(M_QSTR(Message::TrC(MG_MapFileOpened, fname.c_str())), 3000);
	return 1;
}

void MainWindow::ReadMapData(vector<int> &pMadData, const string &pStr) const
{
	// トークン分割
    const char del = ',';
    int idx = 0, last = pStr.find_first_of(del);
    while (idx < pStr.size()) {
        string subStr(pStr, idx, last-idx);
        subStr = trim(subStr);
        pMadData.push_back(atoi(subStr.c_str())-1);
        idx = last + 1;
        last = pStr.find_first_of(del, idx);
        if (last == string::npos) last = pStr.size();
    }
}

/*
	ファイルに書き込む
*/
int MainWindow::WriteFile(const string &pFileName)
{
	FILE *fp = fopen(pFileName.c_str(), "w");
	if (!fp) return 0;

	fprintf(fp, "%s = %s\n", L_KEY_ICONDIR, mIconDir.c_str());
	const stringVector &iconFiles = mIconTable->GetFiles();
	for (int i = 0; i < iconFiles.size(); i++) {
		fprintf(fp, "%s%d = %s\n", L_KEY_ICONFILE, i+1, iconFiles[i].c_str());
	}
	int rowNum = mMapTable->GetRowNum(), colNum = mMapTable->GetColNum();
	fprintf(fp, "%s = %d %d\n", L_KEY_MAPSIZE, rowNum, colNum);
	fprintf(fp, "%s =", L_KEY_MAPDATA);
	for (int r = 0; r < rowNum; r++) {
		if (r > 0) fprintf(fp, "\t");
		for (int c = 0; c < colNum; c++) {
			fprintf(fp, "%3d,", mMapTable->GetIconNum(r, c));
		}
		if (r < rowNum-1) fprintf(fp, "%s\n", L_CR);
	}
	fclose(fp);
	mIsModified = false;
	SetTitle();
	return 1;
}

bool MainWindow::GetPixmap(QPixmap &pPixmap, int pIconIdx) const
{
	return mIconTable->GetPixmap(pPixmap, pIconIdx);
}

void MainWindow::closeEvent(QCloseEvent *pEvent)
{
	Exit();
}

void MainWindow::NotifyCurIconChanged()
{
	if (mIconDir.empty()) {
		statusBar()->message(M_QSTR(Message::TrC(MG_IconFileNotRegist)));
		mMapTable->NotifyIconChanged();
		return;
	}

	string iconnm = mIconTable->GetCurIconFileName();
	string fname = (iconnm.empty())? "" : mIconDir + "/" + iconnm;
	QPixmap pix(fname.c_str());
	mCurPixmap->setPixmap(pix);
	mCurPixName->setText((iconnm.empty())? M_QSTR(Message::TrC(MG_NotSelect)) : iconnm.c_str());
	if (iconnm.empty()) {
		mMapTable->NotifyIconChanged();
		statusBar()->message(M_QSTR(Message::TrC(MG_IconFileNotSelected)));
	} else {
		int iconIdx = mIconTable->GetCurIndex();
		mMapTable->NotifyIconChanged(iconIdx, mCurPixmap->pixmap());
		statusBar()->message("");
	}
}

void MainWindow::NotifyEdited()
{
	mIsModified = true;
	SetTitle();
	SetUndoEnable(true);
	SetRedoEnable(false);
}
