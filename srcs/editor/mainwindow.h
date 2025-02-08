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
	void SetTitle(bool isModified = false);
	void NotifyCurIconChanged();
	void NotifyEdited();

public slots:
	void Open();
	void Save();
	bool SaveAs();
	void Exit();
	void Setting();

private:
	void ChangeIconDir(const char *pIconDir, bool pMsgFlg = true);
	int LoadMapFile();
	void ReadMapData(vector<int> &pMadData, const string &pStr) const;
	int WriteFile(const string &pFileName);
	void closeEvent(QCloseEvent *pEvent);

private:
	string mFileName;	// マップデータファイル
	string mDataDir;	// マップデータフォルダ
	string mIconDir;	// アイコンフォルダ

	QPopupMenu *mFileMenu;
	QPopupMenu *mEditMenu;
	QToolButton *mSaveBtn;
	QToolButton *mSettingBtn;

	QLabel *mCurPixmap;
	QLabel *mCurPixName;
	IconTable *mIconTable;
	MapTable *mMapTable;
};

#endif
