#ifndef mainwindow_h
#define mainwindow_h 1

#include <string>
#include <algorithm>
using namespace std;

class IconTable;
class MapTable;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(const string &curDir);
	void Initiate();
	QPixmap GetNonePixmap() const { return mNonePixmap; }

	void NotifyCurIconChanged();
	void NotifyIconEdited();
	void NotifyEdited();

private slots:
	void onOpen();
	void onClose();
	void onSave();
	void onSaveAs();
	void onExport();
	void onExit();
	void onUndo();
	void onRedo();
	void onGrid();
	void onCopy();
	void onSelect();
	void onSelectAll();
	void onClear();
	void onSetting();
	void onUp();
	void onDown();
	void onLeft();
	void onRight();

private:
	bool SetMapFileName(const QString &pFileName);
	bool ConfirmSave();
	int LoadMapFile();
	void ReadTableData(vector<int> &pData, const string &pStr) const;
	int WriteFile(const string &pFileName);
	void closeEvent(QCloseEvent *pEvent);
	void SetTitle();
	void SetEditBtnEnable();

private:
	string mFileName;	// マップデータファイル
	string mDataDir;	// マップデータフォルダ

	QMenu *mFileMenu;
	QMenu *mEditMenu;
	QAction *mCloseAction;
	QAction *mSaveAction;
	QAction *mSaveAsAction;
	QAction *mExportAction;
	QAction *mGridAction;
	QAction *mSelectAction;
	QAction *mSelectAllAction;
	QAction *mCopyAction;
	QAction *mClearAction;
	QAction *mUndoAction;
	QAction *mRedoAction;
	QAction *mSettingAction;

	QPixmap mNonePixmap;	// アイコン未設定のセルに表示するPixmap
	IconTable *mIconTable;
	MapTable *mMapTable;
	bool mIsModified;
};

#endif
