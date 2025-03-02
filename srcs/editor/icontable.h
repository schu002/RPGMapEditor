#ifndef icontable_h
#define icontable_h 1

#include "zone.h"

class MainWindow;

// ����
enum {
	L_VEC_UP	= 1,
	L_VEC_DOWN,
	L_VEC_LEFT,
	L_VEC_RIGHT,
};

struct SelectInfo
{
	int mIconIdx;
	QPixmap mPixmap;
};

class IconTable : public QTableWidget
{
	Q_OBJECT

public:
	IconTable(QWidget *pParent, MainWindow *pMainWin);

	int Init(const string &pDir, const stringVector *pFiles = NULL, const vector<int> *pTable = NULL);
	void Close();
	const string & GetDir() const { return mIconDir; }
	void OutputFile(FILE *fp);
	void Move(int pVec);
	int GetSelectRowNum() const;
	int GetSelectColNum() const;

	string GetCurIconFileName() const;
	bool GetSelectZone(Zone &zone) const;
	bool GetPixmap(QPixmap &pPixmap, int pIconIdx) const;
	bool GetCurPixmap(vector<SelectInfo> &pIconList) const;
	void Clear();

private:
	int GetCurIconIdx() const;
	void moveOne(int row1, int col1, int ofsRow, int ofsCol);
	bool eventFilter(QObject *obj, QEvent *e);

private:
	string mIconDir;		// �A�C�R���t�H���_
	stringVector mFiles;	// �A�C�R���t�@�C�������X�g
	vector<int> mTable2ID;	// �e�[�u���ʒu����t�@�C��ID�ւ̃e�[�u��
	vector<int> mID2Table;	// �t�@�C��ID����e�[�u���ʒu�ւ̃e�[�u��
	MainWindow *mMainWin;
};

#endif
