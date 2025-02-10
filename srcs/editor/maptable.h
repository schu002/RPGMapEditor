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
		L_Attr_JournalNow	= 0x02, // UndoまたはRedoの処理中
		L_Attr_SelectMode	= 0x04, // 選択モードかどうか
	};

public:
	MapTable(QWidget *pParent, MainWindow *pMainWin, int pRowNum = 0, int pColNum = 0);
	~MapTable();

	void Init(int pRowNum, int pColNum, vector<int> *pData = NULL,
			const Zone *pSelZone = NULL, const Point *pCurPos = NULL);

	// row colの位置のアイコン番号を取得する
	int GetIconNum(int row, int col) const;
	int GetRowNum() const { return mRowNum; }
	int GetColNum() const { return mColNum; }

	void SetSelectMode(bool onoff);
	int Undo();
	int Redo();
	bool IsSelect() const { return ((mAttr & L_Attr_SelectMode) && !mSelZone.empty())? true : false; }
	void NotifyIconChanged(int idx = -1, QPixmap *pixmap = NULL);

public slots:
	void slot_OnPressed(int row, int col, int button, const QPoint &mousePos);
	void slot_OnCurrentChanged(int row, int col);

private:
	bool SetPixmap(int pRow, int pCol, int pIconIdx);
	bool SetPixmap(const Zone &pZone, int pIconIdx);
	void SetPixmap(const Zone &pZone, const QPixmap &pPixmap);
	void ResetSelZonePixmap(const Zone *pZone);
	void ChangeSize(int pRowNum, int pColNum);
	void Select(int row, int col);
	bool Select(const Zone *pSelZone = NULL);
	void SelectAll();
	void UnSelect();
	void AddUndo(int ope, bool clearRedo = true);
	bool eventFilter(QObject *obj, QEvent *e);

private:
	int mAttr;
	int mCurIconIdx;
	int mRowNum, mColNum;
	QPixmap *mCurPixmap;
	Point mPressPnt;	// マウスを押した位置
	Zone mSelZone;
	vector<int>	mData;
	JournalStack mUndoStack;
	JournalStack mRedoStack;
	MainWindow *mMainWin;
};

#endif
