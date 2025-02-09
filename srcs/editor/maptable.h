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
		L_Attr_MousePress	= 0x01, // �}�E�X�̍��{�^�������������Ă��Ԃ��ǂ���
		L_Attr_JournalNow	= 0x02, // Undo�܂���Redo�̏�����
	};

public:
	MapTable(QWidget *pParent, MainWindow *pMainWin, int pRowNum = 0, int pColNum = 0);
	~MapTable();

	void Init(int pRowNum, int pColNum, vector<int> *pData = NULL,
			const Zone *pSelZone = NULL, const Point *pCurPos = NULL);

	// row col�̈ʒu�̃A�C�R���ԍ����擾����
	int GetIconNum(int row, int col) const;
	int GetRowNum() const { return mRowNum; }
	int GetColNum() const { return mColNum; }

	int Undo();
	int Redo();
	void NotifyIconChanged(int idx = -1, QPixmap *pixmap = NULL);

public slots:
	void slot_OnPressed(int row, int col, int button, const QPoint &mousePos);
	void slot_OnCurrentChanged(int row, int col);

private:
	bool SetPixmap(int pRow, int pCol, int pIconIdx);
	bool SetPixmap(const Zone &pZone, int pIconIdx);
	void SetPixmap(const Zone &pZone, const QPixmap &pPixmap);
	void ChangeSize(int pRowNum, int pColNum);
	void AddUndo(int ope, bool clearRedo = true);
	bool eventFilter(QObject *obj, QEvent *e);

private:
	int mAttr;
	int mCurIconIdx;
	int mRowNum, mColNum;
	QPixmap *mCurPixmap;
	Point mPressPnt;	// �}�E�X���������ʒu
	Zone mSelZone;
	vector<int>	mData;
	JournalStack mUndoStack;
	JournalStack mRedoStack;
	MainWindow *mMainWin;
};

#endif
