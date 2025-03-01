#ifndef icontable_h
#define icontable_h 1

#include "zone.h"

class MainWindow;

class IconTable : public QTableWidget
{
	Q_OBJECT

	enum {
		L_Attr_MousePress	= 0x01, // マウスの左ボタンを押し下げてる状態かどうか
		L_Attr_Swap			= 0x02,	// アイコンの位置を変更した
	};

public:
	IconTable(QWidget *pParent, MainWindow *pMainWin);

	int Init(const string &pDir, const stringVector *pFiles = NULL, const vector<int> *pTable = NULL);
	void OutputFile(FILE *fp);

	int GetCurIconIdx() const;
	string GetCurIconFileName() const;
	const string & GetDir() const { return mIconDir; }
	bool GetPixmap(QPixmap &pPixmap, int pIconIdx) const;
	void Clear();

protected:
	bool eventFilter(QObject *obj, QEvent *e);

private slots:
	void slot_OnCurrentChanged(int row, int col);

private:
	int mAttr;
	Point mMovePnt;		// マウスの移動位置
	string mIconDir;		// アイコンフォルダ
	stringVector mFiles;	// アイコンファイル名リスト
	vector<int> mTable2ID;	// テーブル位置からファイルIDへのテーブル
	vector<int> mID2Table;	// ファイルIDからテーブル位置へのテーブル
	MainWindow *mMainWin;
};

#endif
