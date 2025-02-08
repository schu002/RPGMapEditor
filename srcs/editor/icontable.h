#ifndef bmpfillview_h
#define bmpfillview_h 1

#include <string>
#include <vector>
using namespace std;
typedef vector<string> stringVector;

class MainWindow;

class IconTable : public QTable
{
	Q_OBJECT

public:
	IconTable(QWidget *pParent, MainWindow *pMainWin);
	~IconTable();

	int OpenIconFiles(const string &pIconDir);
	int GetCurIndex() const;
	string GetCurIconFileName() const;
	const stringVector & GetFiles() const { return mFiles; }
	bool GetPixmap(QPixmap &pPixmap, int pIconIdx) const;

	void Clear();

public slots:
	void slot_OnCurrentChanged(int row, int col);

private:
	stringVector mFiles;	// アイコンファイル名リスト
	MainWindow *mMainWin;
};

#endif
