#ifndef icontable_h
#define icontable_h 1

#include "zone.h"

class MainWindow;

// 方向
enum {
	L_VEC_UP	= 1,
	L_VEC_DOWN,
	L_VEC_LEFT,
	L_VEC_RIGHT,
};

struct IconData {
	string mFileName;	// PNGファイル名
	int mTableIdx;		// テーブルIndex

	IconData() : mTableIdx(-1) {}
	IconData(const string &pFileName, int pTableIdx = -1) : mFileName(pFileName), mTableIdx(pTableIdx) {}
	bool operator == (const IconData &pData) const { return (mFileName == pData.mFileName); }
};

// std::hash<IconData>を特殊化
template <>
struct hash<IconData> {
	size_t operator()(const IconData &data) const { return std::hash<string>()(data.mFileName); }
};

typedef vector<IconData>	IconDataVector;
typedef hashSet<IconData>					IconDataSet;
typedef hashSet<IconData>::const_iterator	IconDataSetCItr;


class IconTable : public QTableWidget
{
	Q_OBJECT

public:
	IconTable(QWidget *pParent, MainWindow *pMainWin);

	int Init(const string &pDir, const IconDataVector *pDatas = NULL);
	void Close();
	const string & GetDir() const { return mIconDir; }
	void OutputFile(FILE *fp);
	void Move(int pVec);
	int GetSelectRowNum() const;
	int GetSelectColNum() const;

	string GetCurIconFileName() const;
	bool GetSelectZone(Zone &zone) const;
	bool GetPixmap(QPixmap &pPixmap, int pIconIdx) const;
	bool GetCurPixmap(QPixmap &pPixmap, int pOfsRow = 0, int pOfsCol = 0) const;
	int GetCurIconIdx(int pOfsRow = 0, int pOfsCol = 0) const;
	void Clear();

private:
	bool GetPixmap(QPixmap &pixmap, int row, int col) const;
	void moveOne(int row1, int col1, int ofsRow, int ofsCol);
	bool eventFilter(QObject *obj, QEvent *e);

private:
	string mIconDir;		// アイコンフォルダ
	IconDataVector mDatas;	// アイコンデータ
	vector<int> mTable2ID;	// テーブル位置からファイルIDへのテーブル
	MainWindow *mMainWin;
};

#endif
