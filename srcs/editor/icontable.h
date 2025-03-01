#ifndef icontable_h
#define icontable_h 1

#include "zone.h"

class MainWindow;

class IconTable : public QTableWidget
{
	Q_OBJECT

	enum {
		L_Attr_MousePress	= 0x01, // �}�E�X�̍��{�^�������������Ă��Ԃ��ǂ���
		L_Attr_Swap			= 0x02,	// �A�C�R���̈ʒu��ύX����
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
	Point mMovePnt;		// �}�E�X�̈ړ��ʒu
	string mIconDir;		// �A�C�R���t�H���_
	stringVector mFiles;	// �A�C�R���t�@�C�������X�g
	vector<int> mTable2ID;	// �e�[�u���ʒu����t�@�C��ID�ւ̃e�[�u��
	vector<int> mID2Table;	// �t�@�C��ID����e�[�u���ʒu�ւ̃e�[�u��
	MainWindow *mMainWin;
};

#endif
