#include "maindef.h"	
#include "mainwindow_moc.h"

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

string ConvToStr(const QString &pStr)
{
	QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");
	return codec->fromUnicode(pStr).constData();
}

};

MainWindow::MainWindow(const string &pCurDir)
 :	QMainWindow(NULL),
	mIconTable(NULL), mMapTable(NULL), mDataDir(pCurDir), mIsModified(false)
{
	mFileName = Message::TrC(MG_DefaultDataFile);

	// アイコン
	QIcon iconApp("./lib/app.png");
	QIcon iconOpen("./lib/open.png");
	QIcon iconSave("./lib/save.png");
	QIcon iconSaveAs("./lib/saveas.png");
	QIcon iconExport("./lib/export.png");
	QIcon iconClear("./lib/clear.png");
	QIcon iconCopy("./lib/copy.png");
	QIcon iconUndo("./lib/undo.png");
	QIcon iconRedo("./lib/redo.png");
	QIcon iconSelect("./lib/select.png");
	QIcon iconSelectAll("./lib/selectall.png");
	QIcon iconGrid("./lib/grid.png");
	QIcon iconSetup("./lib/setup.png");
	QIcon iconUp("./lib/up.png");
	QIcon iconDown("./lib/down.png");
	QIcon iconLeft("./lib/left.png");
	QIcon iconRight("./lib/right.png");
	mNonePixmap = QPixmap("./lib/__none.png");

	// ウィンドウのタイトル設定
	setWindowTitle(M_QSTR(Message::TrC(MG_RPGMap_Editor)));
	setWindowIcon(iconApp);

	QAction *openAction = new QAction(iconOpen, M_QSTR(Message::TrC(MG_File_Open)), this);
	mCloseAction = new QAction(M_QSTR(Message::TrC(MG_File_Close)), this);
	mSaveAction = new QAction(iconSave, M_QSTR(Message::TrC(MG_File_Save)), this);
	mSaveAsAction = new QAction(iconSaveAs, M_QSTR(Message::TrC(MG_File_SaveAs)), this);
	mExportAction = new QAction(iconExport, M_QSTR(Message::TrC(MG_File_Export)) + "(&E)...", this);
	QAction *exitAction = new QAction(M_QSTR(Message::TrC(MG_Exit)), this);
	mUndoAction = new QAction(iconUndo, M_QSTR(Message::TrC(MG_Undo)), this);
	mRedoAction = new QAction(iconRedo, M_QSTR(Message::TrC(MG_Redo)), this);
	QString msg = M_QSTR(Message::TrC(MG_ShowGrid)+Message::TrC(MG_ON));
	mGridAction = new QAction(iconGrid, msg, this);
	mGridAction->setCheckable(true);
	mGridAction->setChecked(true);
	mGridAction->setToolTip(msg);
	msg = M_QSTR(Message::TrC(MG_CopyMode)+Message::TrC(MG_OFF));
	mCopyAction = new QAction(iconCopy, msg, this);
	mCopyAction->setCheckable(true);
	mCopyAction->setChecked(false);
	mCopyAction->setToolTip(msg);
	msg = M_QSTR(Message::TrC(MG_SelectMode)+Message::TrC(MG_COLON)+Message::TrC(MG_OFF));
	mSelectAction = new QAction(iconSelect, msg, this);
	mSelectAction->setCheckable(true);
	mSelectAction->setChecked(false);
	mSelectAction->setToolTip(msg);
	mSelectAllAction = new QAction(iconSelectAll, M_QSTR(Message::TrC(MG_SelectAll)), this);
	mClearAction = new QAction(iconClear, M_QSTR(Message::TrC(MG_Clear)), this);
	mSettingAction = new QAction(iconSetup, M_QSTR(Message::TrC(MG_Setting) + "..."), this);

	// メニューバーの作成
	mFileMenu = menuBar()->addMenu(M_QSTR(Message::TrC(MG_File)));
	mFileMenu->addAction(openAction);
	mFileMenu->addAction(mCloseAction);
	mFileMenu->addAction(mSaveAction);
	mFileMenu->addAction(mSaveAsAction);
	mFileMenu->addAction(mExportAction);
	mFileMenu->addSeparator();
	mFileMenu->addAction(exitAction);

	mEditMenu = menuBar()->addMenu(M_QSTR(Message::TrC(MG_Edit)));
	mEditMenu->addAction(mUndoAction);
	mEditMenu->addAction(mRedoAction);
	mEditMenu->addSeparator();
	mEditMenu->addAction(mGridAction);
	mEditMenu->addAction(mSelectAction);
	mEditMenu->addAction(mCopyAction);
	mEditMenu->addSeparator();
	mEditMenu->addAction(mSelectAllAction);
	mEditMenu->addAction(mClearAction);
	mEditMenu->addSeparator();
	mEditMenu->addAction(mSettingAction);

	// ツールバーの作成
	QToolBar *toolBar = addToolBar("Main Toolbar");
	toolBar->addAction(openAction);
	toolBar->addAction(mSaveAction);
	toolBar->addAction(mSaveAsAction);
	toolBar->addSeparator();
	toolBar->addAction(mUndoAction);
	toolBar->addAction(mRedoAction);
	toolBar->addSeparator();
	toolBar->addAction(mGridAction);
	toolBar->addAction(mSelectAction);
	toolBar->addAction(mCopyAction);
	toolBar->addSeparator();
	toolBar->addAction(mSelectAllAction);
	toolBar->addAction(mClearAction);
	toolBar->addSeparator();
	toolBar->addAction(mSettingAction);

	// メニューアクションの処理
	connect(openAction, &QAction::triggered, this, &MainWindow::onOpen);
	connect(mCloseAction, &QAction::triggered, this, &MainWindow::onClose);
	connect(mSaveAction, &QAction::triggered, this, &MainWindow::onSave);
	connect(mSaveAsAction, &QAction::triggered, this, &MainWindow::onSaveAs);
	connect(mExportAction, &QAction::triggered, this, &MainWindow::onExport);
	connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);
	connect(mUndoAction, &QAction::triggered, this, &MainWindow::onUndo);
	connect(mRedoAction, &QAction::triggered, this, &MainWindow::onRedo);
	connect(mGridAction, &QAction::triggered, this, &MainWindow::onGrid);
	connect(mCopyAction, &QAction::triggered, this, &MainWindow::onCopy);
	connect(mSelectAction, &QAction::triggered, this, &MainWindow::onSelect);
	connect(mSelectAllAction, &QAction::triggered, this, &MainWindow::onSelectAll);
	connect(mClearAction, &QAction::triggered, this, &MainWindow::onClear);
	connect(mSettingAction, &QAction::triggered, this, &MainWindow::onSetting);

	QWidget *centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);
	QHBoxLayout *topLayout = new QHBoxLayout;
	QVBoxLayout *vBoxLayout = new QVBoxLayout;
	QHBoxLayout *hBoxLayout = new QHBoxLayout;

	// 移動ボタン
	QLabel *dmyLbl = new QLabel(centralWidget);
	dmyLbl->setFixedSize(70, 20);
	QPushButton *upBtn = new QPushButton(iconUp, "", centralWidget);
	QPushButton *downBtn = new QPushButton(iconDown, "", centralWidget);
	QPushButton *leftBtn = new QPushButton(iconLeft, "", centralWidget);
	QPushButton *rightBtn = new QPushButton(iconRight, "", centralWidget);
	upBtn->setFixedSize(24, 24);
	downBtn->setFixedSize(24, 24);
	leftBtn->setFixedSize(24, 24);
	rightBtn->setFixedSize(24, 24);
	hBoxLayout->addWidget(dmyLbl);
	hBoxLayout->addWidget(upBtn);
	hBoxLayout->addWidget(downBtn);
	hBoxLayout->addWidget(leftBtn);
	hBoxLayout->addWidget(rightBtn);
	QObject::connect(upBtn, &QPushButton::clicked, this, &MainWindow::onUp);
	QObject::connect(downBtn, &QPushButton::clicked, this, &MainWindow::onDown);
	QObject::connect(leftBtn, &QPushButton::clicked, this, &MainWindow::onLeft);
	QObject::connect(rightBtn, &QPushButton::clicked, this, &MainWindow::onRight);

	// アイコンテーブル
	mIconTable = new IconTable(centralWidget, this);
	vBoxLayout->addLayout(hBoxLayout);
	vBoxLayout->addWidget(mIconTable);

	// マップテーブル
	mMapTable = new MapTable(centralWidget, this, mIconTable);
	topLayout->addLayout(vBoxLayout);
	topLayout->addWidget(mMapTable);
	centralWidget->setLayout(topLayout);
	SetEditBtnEnable();

	mUndoAction->setEnabled(false);
	mRedoAction->setEnabled(false);
	mCopyAction->setEnabled(false);
}

void MainWindow::onOpen()
{
	if (!ConfirmSave()) return;

	QString fname = QFileDialog::getOpenFileName(this,
		 				M_QSTR(Message::TrC(MG_OpenMapFile)),
						mDataDir.c_str(),
						M_QSTR(Message::TrC(MG_MapDataFile)));
	if (!SetMapFileName(fname)) return;

	mIconTable->setEnabled(true);
	mMapTable->setEnabled(true);
	LoadMapFile();
	mCloseAction->setEnabled(true);
	mSaveAction->setEnabled(true);
	mSaveAsAction->setEnabled(true);
	mExportAction->setEnabled(true);
	mSelectAction->setEnabled(true);
	mSettingAction->setEnabled(true);
}

void MainWindow::onClose()
{
	if (!ConfirmSave()) return;

	mMapTable->Close();
	mIconTable->Close();
	mFileName = "";
	mIsModified = false;
	setWindowTitle(M_QSTR(Message::TrC(MG_RPGMap_Editor)));
	SetEditBtnEnable();
}

void MainWindow::onSave()
{
	WriteFile(mFileName);
}

void MainWindow::onSaveAs()
{
	QString fname = QFileDialog::getSaveFileName(this,
            M_QSTR(Message::TrC(MG_File_SaveAs)), mDataDir.c_str(), M_QSTR(Message::TrC(MG_MapDataFile)));
	if (!SetMapFileName(fname)) return;

	WriteFile(mDataDir + "/" + mFileName);
	string str = ConvToStr(fname);
	statusBar()->showMessage(M_QSTR(Message::TrC(MG_SaveMapData, str.c_str())));
}

void MainWindow::onExport()
{
	QString filePath = QFileDialog::getSaveFileName(this,
            M_QSTR(Message::TrC(MG_File_Export)), mDataDir.c_str(), "Image Files (*.png)");
	if (filePath.isEmpty()) return;
	if (!filePath.endsWith(".png", Qt::CaseInsensitive)) filePath += ".png";

	if (ExportFile(*mMapTable, filePath)) {
		string str = ConvToStr(filePath);
		QMessageBox::information(this, M_QSTR(Message::TrC(MG_File_Export)),
			M_QSTR(Message::TrC(MG_ExportMapData, str.c_str())));
	}
}

bool MainWindow::ExportFile(const QTableWidget &pTableWgt, const QString &pFileName, int pColNum) const
{
	if (pFileName.isEmpty()) return false;

	int rowNum = pTableWgt.rowCount(), colNum = pTableWgt.columnCount();
	int newRows = rowNum, newCols = colNum;
	if (pColNum > 0 && pColNum != colNum) {
		int cnt = rowNum * colNum;
		newRows = (cnt % pColNum)? cnt/pColNum+1 : cnt/pColNum;
		newCols = pColNum;
	}
	QImage finalImage(L_PIXSIZE*newCols, L_PIXSIZE*newRows, QImage::Format_ARGB32);
    finalImage.fill(Qt::white);
    QPainter painter(&finalImage);

	for (int r = 0; r < rowNum; r++) {
		for (int c = 0; c < colNum; c++) {
			QTableWidgetItem *witem = pTableWgt.item(r, c);
			QVariant value = witem->data(Qt::DecorationRole);
			QPixmap pixmap;
	        if (value.isValid() && value.canConvert<QPixmap>()) {
	        	pixmap = qvariant_cast<QPixmap>(value);
	        }
			// 画像を適切な位置に配置
			int row = r, col = c;
			if (pColNum > 0 && pColNum != colNum) {
				int num = r*colNum + c;
				row = num/pColNum, col = num%pColNum;
			}
            painter.drawPixmap(col * L_PIXSIZE, row * L_PIXSIZE, pixmap);
		}
	}
	painter.end();

	finalImage.save(pFileName, "PNG");
	return true;
}

void MainWindow::onExit()
{
	if (!ConfirmSave()) return;
	close();
}

bool MainWindow::SetMapFileName(const QString &pFileName)
{
	mDataDir = mFileName = "";
	if (pFileName.isEmpty()) return false;

	string str = ConvToStr(pFileName);
	int idx = str.rfind('/');
	if (idx < 1) return false;
	mDataDir = str.substr(0, idx);
	mFileName = str.substr(idx+1, str.size()-idx-1);
	return true;
}

/*　元に戻す　*/
void MainWindow::onUndo()
{
	mMapTable->Undo();
}

/*　やり直す　*/
void MainWindow::onRedo()
{
	mMapTable->Redo();
}

void MainWindow::onGrid()
{
	bool onoff = (mMapTable->showGrid())? false : true;
	mMapTable->SetDrawGrid(onoff);
	int id = (onoff)? MG_ON : MG_OFF;
	QString msg = M_QSTR(Message::TrC(MG_ShowGrid) + Message::TrC(id));
	mGridAction->setText(msg);
	mGridAction->setToolTip(msg);
	statusBar()->showMessage(msg);
}

/*　コピーモード　*/
void MainWindow::onCopy()
{
	bool onoff = (mMapTable->IsCopyMode())? false : true;
	int id = (onoff)? MG_ON : MG_OFF;
	QString msg = M_QSTR(Message::TrC(MG_CopyMode) + Message::TrC(id));
	mCopyAction->setText(msg);
	mCopyAction->setToolTip(msg);
	statusBar()->showMessage(msg);
	mMapTable->SetCopyMode(onoff);
}

/*　選択モード　*/
void MainWindow::onSelect()
{
	if (!mMapTable) return;

	bool onoff = (mSelectAction->isChecked())? false : true;
	int id = (onoff)? MG_ON : MG_OFF;
	QString msg = M_QSTR(Message::TrC(MG_SelectMode) + Message::TrC(MG_COLON) + Message::TrC(id));
	mSelectAction->setText(msg);
	mSelectAction->setToolTip(msg);
	SetEditBtnEnable();
	mMapTable->SetSelectMode(!onoff);

	id = (onoff)? MG_InputMode : MG_SelectMode;
	statusBar()->showMessage(M_QSTR(Message::TrC(id)));
}

/*　全て選択　*/
void MainWindow::onSelectAll()
{
	mMapTable->SelectAll();
}

/*　クリア　*/
void MainWindow::onClear()
{
	mMapTable->Clear();
}

/*  設定  */
void MainWindow::onSetting()
{
	SettingDlg *dlg = new SettingDlg(this);
	const string &dir = mIconTable->GetDir();
	int rowNum = mMapTable->rowCount(), colNum = mMapTable->columnCount();
	dlg->Init(dir, rowNum, colNum);
	if (dlg->exec() != QDialog::Accepted) return;

	string iconDir(dlg->GetIconDir().toUtf8().constData());
	vector<int> newIconID;
	int imgCnt = mIconTable->Init(iconDir, NULL, &newIconID);
	if (imgCnt == 0) {
		statusBar()->showMessage(M_QSTR(Message::TrC(MG_IconFileNotRegist)));
		QMessageBox::critical(this, M_QSTR(Message::TrC(MG_Error)),
			M_QSTR(Message::TrC(MG_ImageFileNotFound, iconDir.c_str())));
	} else {
		statusBar()->showMessage(M_QSTR(Message::TrC(MG_IconFileLoaded, imgCnt)), 3000);
	}

	mMapTable->ResetIconIdx(newIconID);
	rowNum = dlg->GetMapSizeRow();
	colNum = dlg->GetMapSizeCol();
	mMapTable->ChangeSize(rowNum, colNum);

	mIsModified = true;
	SetTitle();
}

void MainWindow::onUp()
{
	mIconTable->Move(L_VEC_UP);
}

void MainWindow::onDown()
{
	mIconTable->Move(L_VEC_DOWN);
}

void MainWindow::onLeft()
{
	mIconTable->Move(L_VEC_LEFT);
}

void MainWindow::onRight()
{
	mIconTable->Move(L_VEC_RIGHT);
}

void MainWindow::Initiate()
{
	LoadMapFile();
}

bool MainWindow::ConfirmSave()
{
	if (!mIsModified) return true;
	if (mFileName.empty()) return true;

	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, M_QSTR(Message::TrC(MG_Confirm)),
								M_QSTR(Message::TrC(MG_ConfirmSave)),
								QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	if (reply == QMessageBox::Cancel) return false;
	if (reply == QMessageBox::Yes) onSave();
	return true;
}

void MainWindow::SetTitle()
{
	string title = string(Message::TrC(MG_RPGMap_Editor)) + "  -  " + mFileName;
	if (mIsModified) title += " *";
	title += string("  [") + mDataDir + string("]");
	// title = wkstr;
	setWindowTitle(M_QSTR(title));
}

/*
	ファイルを読み込む
*/
int MainWindow::LoadMapFile()
{
#define L_DEFAULT_ICONNUM	100	// デフォルトのアイコン数

	string fname = mDataDir + "/" + mFileName;
	FILE *fp = fopen(fname.c_str(), "r");
	if (!fp) {
		SetTitle();
		return 0;
	}

	int rowNum = L_DEFALUT_MAPSIZE_ROW, colNum = L_DEFALUT_MAPSIZE_COL;
	int iconFileLen = strlen(L_KEY_ICONFILE), maxID = -1;
	bool crFlg = false;
	string key, val, iconDir;
	vector<int> mapData;
	IconDataVector iconDatas(L_DEFAULT_ICONNUM);
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
		crFlg = (len > 3 && val.substr(len-4, len-1) == L_CR)? true : false;
		if (crFlg) val = val.substr(0, len-5);
		if (key == L_KEY_ICONDIR) {
			iconDir = val;
		} else if (key.substr(0, iconFileLen) == L_KEY_ICONFILE) {
			string str = key.substr(iconFileLen, key.size()-iconFileLen);
			int id = atoi(str.c_str())-1;
			if (id < 0) continue;

			IconData iconData(val);
			if (iconDatas.size() < id+1) iconDatas.resize(id+1);
			int idx = val.find_last_of(':');
			if (idx > 0) {
				iconData.mFileName = trim(val.substr(0, idx));
				string substr = trim(val.substr(idx+1, val.size()-idx-1));
				if (!substr.empty()) iconData.mTableIdx = atoi(substr.c_str());
			}
			iconDatas[id] = iconData;
			maxID = M_MAX(maxID, id);
		} else if (key == L_KEY_MAPSIZE) {
			sscanf(val.c_str(), "%d %d", &rowNum, &colNum);
		} else if (key == L_KEY_MAPDATA) {
			ReadTableData(mapData, val);
		}
	}
	fclose(fp);

	if (maxID > 0) iconDatas.resize(maxID+1);
	mIconTable->Init(iconDir, &iconDatas);
	mMapTable->Init(rowNum, colNum, &mapData);
	mIsModified = false;
	SetTitle();
	statusBar()->showMessage(M_QSTR(Message::TrC(MG_MapFileOpened, fname.c_str())), 3000);
	return 1;
}

void MainWindow::ReadTableData(vector<int> &pData, const string &pStr) const
{
	// トークン分割
	const char del = ',';
	int idx = 0, last = pStr.find_first_of(del);
	while (idx < pStr.size()) {
		string subStr(pStr, idx, last-idx);
		subStr = trim(subStr);
		pData.push_back(atoi(subStr.c_str())-1);
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

	ResetIconIdx();

	mIconTable->OutputFile(fp);
	mMapTable->OutputFile(fp);
	fclose(fp);
	mIsModified = false;
	SetTitle();
	return 1;
}

// アイコン番号をpngファイル名順に振り直す
void MainWindow::ResetIconIdx()
{
	vector<int> newIconID;
	mIconTable->ResetIconIdx(newIconID);
	mMapTable->ResetIconIdx(newIconID);
}

void MainWindow::closeEvent(QCloseEvent *pEvent)
{
	onExit();
}

void MainWindow::NotifyCurIconChanged()
{
	const string &iconDir = mIconTable->GetDir();
	if (iconDir.empty()) {
		statusBar()->showMessage(M_QSTR(Message::TrC(MG_IconFileNotRegist)));
		return;
	}

	string iconnm = mIconTable->GetCurIconFileName();
	if (iconnm.empty()) {
		statusBar()->showMessage(M_QSTR(Message::TrC(MG_IconFileNotSelected)));
	} else {
		statusBar()->showMessage(iconnm.c_str());
	}
}

void MainWindow::NotifyIconEdited()
{
	mIsModified = true;
	SetTitle();
}

void MainWindow::NotifyEdited()
{
	SetEditBtnEnable();
}

void MainWindow::SetEditBtnEnable()
{
	if (mFileName.empty()) {
		mCloseAction->setEnabled(false);
		mSaveAction->setEnabled(false);
		mSaveAsAction->setEnabled(false);
		mExportAction->setEnabled(false);
		mUndoAction->setEnabled(false);
		mRedoAction->setEnabled(false);
		mSelectAction->setEnabled(false);
		mSelectAllAction->setEnabled(false);
		mCopyAction->setEnabled(false);
		mClearAction->setEnabled(false);
		mSettingAction->setEnabled(false);
		mIconTable->setEnabled(false);
		mMapTable->setEnabled(false);
		return;
	}

	mUndoAction->setEnabled(mMapTable->CanUndo());
	mRedoAction->setEnabled(mMapTable->CanRedo());
	mIsModified = (mMapTable->CanUndo())? true : false;
	SetTitle();

	bool selMode = mSelectAction->isChecked();
	bool editFlg = (selMode && mMapTable->IsSelectZone())? true : false;
	mSelectAllAction->setEnabled(selMode);
	mCopyAction->setEnabled(editFlg);
	mClearAction->setEnabled(editFlg);
	mIconTable->setEnabled(!selMode);
}
