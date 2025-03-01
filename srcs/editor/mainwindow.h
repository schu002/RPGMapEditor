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
	IconTable * GetIconTable() { return mIconTable; }
	MapTable * GetMapTable() { return mMapTable; }

	bool GetPixmap(QPixmap &pPixmap, int pIconIdx) const;
	void NotifyCurIconChanged();
	void NotifyIconEdited();
	void NotifySelectChanged();
	void NotifyEdited();

private slots:
	void onOpen();
	void onSave();
	void onExit();
	void onUndo();
	void onRedo();
	void onGrid();
	void onCopy();
	void onSelect();
	void onSelectAll();
	void onClear();
	void onSetting();

private:
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
	QAction *mGridAction;
	QAction *mSelectAction;
	QAction *mSelectAllAction;
	QAction *mCopyAction;
	QAction *mClearAction;
	QAction *mUndoAction;
	QAction *mRedoAction;

	QLabel *mCurPixmap;
	QLabel *mCurPixName;
	IconTable *mIconTable;
	MapTable *mMapTable;
	bool mIsModified;
};

#endif
