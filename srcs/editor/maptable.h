#ifndef bmpfilledit_h
#define bmpfilledit_h 1

#include "journal.h"

#define L_DEFALUT_MAPSIZE_ROW	100
#define L_DEFALUT_MAPSIZE_COL	100


class MainWindow;

class MapTable : public QTable
{
	Q_OBJECT

private:
	enum {
		L_Attr_MousePress	= 0x01, // マウスの左ボタンを押し下げてる状態かどうか
		L_Attr_SelectMode	= 0x02, // 選択モードかどうか
	};

public:
	MapTable(QWidget *pParent, MainWindow *pMainWin, int pRowNum = 0, int pColNum = 0);
	~MapTable();

	void Init(int pRowNum, int pColNum, vector<int> *pData = NULL,
			const Zone *pSelZone = NULL, const Point *pCurPos = NULL,
			bool pIsSelect = true);
	void OutputFile(FILE *fp);

	// row colの位置のアイコン番号を取得する
	int GetIconIdx(int row, int col) const;
	int GetRowNum() const { return mRowNum; }
	int GetColNum() const { return mColNum; }

	void SetSelectMode(bool onoff);
	void SetCopyMode(bool onoff);
	int Undo();
	int Redo();
	void Clear();
	void SelectAll();
	bool IsSelectMode() const { return (mAttr & L_Attr_SelectMode)? true : false; }
	bool IsSelectZone() const { return ((mAttr & L_Attr_SelectMode) && !mSelZone.empty())? true : false; }
	bool IsCopyMode() const { return (mCopyPnt.empty())? false : true; }
	void NotifyIconChanged(int idx = -1, QPixmap *pixmap = NULL);

public slots:
	void slot_OnPressed(int row, int col, int button, const QPoint &mousePos);
	void slot_OnCurrentChanged(int row, int col);

private:
	bool SetPixmap(int pRow, int pCol, int pIconIdx, bool pIsSelect = false, bool pIsUpdate = true);
	bool SetPixmap(const Zone &pZone, int pIconIdx);
	void SetPixmap(const Zone &pZone, const QPixmap &pPixmap);
	void ResetSelZonePixmap(const Zone *pZone);
	void ChangeSize(int pRowNum, int pColNum);
	void FinalizeInput();
	void FinalizeCopy();
	void Select(int row, int col);
	bool Select(const Zone *pSelZone = NULL);
	void UnSelect();
	void Move(int key);
	void AddUndo(int ope, const Point *curPos = NULL);
	bool eventFilter(QObject *obj, QEvent *e);

private:
	int mAttr;
	int mCurIconIdx;
	int mRowNum, mColNum;
	QPixmap *mCurPixmap;
	Point mPressPnt;	// マウスを押した位置
	Point mCopyPnt;		// コピーモード開始位置
	Zone mSelZone;
	vector<int>	mData;
	JournalStack mUndoStack;
	JournalStack mRedoStack;
	MainWindow *mMainWin;
};

#endif
