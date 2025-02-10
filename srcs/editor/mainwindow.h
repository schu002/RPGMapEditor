#ifndef bmpfill_h
#define bmpfill_h 1

#include <string>
#include <algorithm>
using namespace std;

class IconTable;
class MapTable;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(const string &curDir, WFlags pflag = 0);
	MapTable * GetMapTable() { return mMapTable; }
	IconTable * GetIconTable() { return mIconTable; }

	bool GetPixmap(QPixmap &pPixmap, int pIconIdx) const;
	void SetTitle();
	void SetUndoEnable(bool onoff);
	void SetRedoEnable(bool onoff);
	void NotifyCurIconChanged();
	void NotifySelectChanged();
	void NotifyEdited();

public slots:
	void Open();
	void Save();
	bool SaveAs();
	void Exit();

	void Undo();
	void Redo();
	void OnGridMenu();
	void OnGridBtn();
	void OnSelectMenu();
	void OnSelectBtn();
	void OnSelectAll();
	void Clear();
	void Setting();

private:
	void ChangeIconDir(const char *pIconDir, bool pMsgFlg = true);
	int LoadMapFile();
	void ReadMapData(vector<int> &pMadData, const string &pStr) const;
	int WriteFile(const string &pFileName);
	void closeEvent(QCloseEvent *pEvent);
	void _OnGrid(bool onoff);
	void _OnSelect(bool onoff);
	void SetEditBtnEnable();

private:
	string mFileName;	// マップデータファイル
	string mDataDir;	// マップデータフォルダ
	string mIconDir;	// アイコンフォルダ

	QPopupMenu *mFileMenu;
	QPopupMenu *mEditMenu;
	int mMenuUndo, mMenuRedo, mMenuGrid, mMenuSelect, mMenuSelectAll, mMenuClear;
	QToolButton *mSaveBtn;
	QToolButton *mRedoBtn;
	QToolButton *mUndoBtn;
	QToolButton *mGridBtn;
	QToolButton *mSelectBtn;
	QToolButton *mSelectAllBtn;
	QToolButton *mClearBtn;
	QToolButton *mSettingBtn;

	QLabel *mCurPixmap;
	QLabel *mCurPixName;
	IconTable *mIconTable;
	MapTable *mMapTable;
	bool mIsModified;
};

#endif
