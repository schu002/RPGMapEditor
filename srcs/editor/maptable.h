#ifndef bmpfilledit_h
#define bmpfilledit_h 1

#include "journal.h"

#define L_DEFALUT_MAPSIZE_ROW	100
#define L_DEFALUT_MAPSIZE_COL	100


class MainWindow;
class IconTable;

class MapTable : public QTableWidget
{
	Q_OBJECT

private:
	enum {
		L_Attr_MousePress	= 0x01, // マウスの左ボタンを押し下げてる状態かどうか
		L_Attr_SelectMode	= 0x02, // 選択モードかどうか
		L_Attr_CopyMode		= 0x04, // コピーモードかどうか
		L_Attr_Moved		= 0x08, // 矢印キーによる移動/コピーを行ったかどうか
		L_Attr_DragMove		= 0x10, // マウスドラッグによる移動処理中
	};

public:
	MapTable(QWidget *pParent, MainWindow *pMainWin, IconTable *pIconTable, int pRowNum = 0, int pColNum = 0);

	void Init(int pRowNum, int pColNum, vector<int> *pData = NULL,
			const Zone *pSelZone = NULL, const Point *pCurPos = NULL,
			bool pIsSelect = true);
	void ChangeSize(int pRowNum, int pColNum);
	void ResetIconIdx(const vector<int> &pNewIconIDVec);
	void Close();
	void OutputFile(FILE *fp);

	// row colの位置のアイコンファイルIDを取得する
	int GetIconIdx(int row, int col) const;

	void SetDrawGrid(bool onoff);
	void SetSelectMode(bool onoff);
	void SetCopyMode(bool onoff);
	void Undo();
	void Redo();
	void Clear();
	void SelectAll();
	bool IsSelectMode() const { return (mAttr & L_Attr_SelectMode)? true : false; }
	bool IsSelectZone() const { return ((mAttr & L_Attr_SelectMode) && !mSelZone.empty())? true : false; }
	bool IsSelectAll() const;
	bool IsCopyMode() const { return (mAttr & L_Attr_CopyMode)? true : false; }
	bool CanUndo() const { return (mUndoStack.empty())? false : true; }
	bool CanRedo() const { return (mRedoStack.empty())? false : true; }

public slots:
	void slot_OnCurrentChanged(int row, int col, int preRow, int preCol);

protected:
	void mousePressEvent(QMouseEvent *event) override;

private:
	bool SetPixmap(int row, int col, int pIconIdx, bool pIsSelect = false, bool pIsUpdate = true);
	void SetPixmap(int row, int col, const QPixmap &pixmap, int pShowFlg = -1);
	void DrawPixmapSelZone();
	void ResetSelZonePixmap(const Zone *pZone);
	void FinalizeInput();
	void FinalizeMove();
	void Select(int row, int col);
	bool Select(const Zone *pSelZone = NULL);
	void UnSelect();
	void Move(int pOfsRow, int pOfsCol);
	void AddUndo(int ope, const Point *curPos = NULL);
	void GetIconMaxZone(Zone &zone) const;
	bool eventFilter(QObject *obj, QEvent *e);
	bool event(QEvent *e);

private:
	int mAttr;
	int mCurIconIdx;
	Point mPressPnt;	// マウスを押した位置
	Point mMovePnt;		// 移動開始位置
	Zone mSelZone;
	vector<int>	mData;
	JournalStack mUndoStack;
	JournalStack mRedoStack;
	MainWindow *mMainWin;
	IconTable *mIconTable;
	QSize	mPixSize;
	mutable QPixmap mTempPixmap;
};

#endif
