#ifndef icontable_h
#define icontable_h 1

#include <string>
#include <vector>
using namespace std;

class MainWindow;

class IconTable : public QTableWidget
{
	Q_OBJECT

public:
	IconTable(QWidget *pParent, MainWindow *pMainWin);

	int Init(const string &pDir, const stringVector *pFiles = NULL, const vector<int> *pTable = NULL);
	void OutputFile(FILE *fp);

	int GetCurIndex() const;
	string GetCurIconFileName() const;
	const string & GetDir() const { return mIconDir; }
	bool GetPixmap(QPixmap &pPixmap, int pIconIdx) const;
	void Clear();

private slots:
	void slot_OnCurrentChanged(int row, int col);

private:
	string mIconDir;		// �A�C�R���t�H���_
	stringVector mFiles;	// �A�C�R���t�@�C�������X�g
	vector<int> mTable;		// �A�C�R���t�@�C��ID������e�[�u��
	MainWindow *mMainWin;
};

#endif
