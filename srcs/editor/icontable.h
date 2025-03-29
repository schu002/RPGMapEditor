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

struct IconData {
	string mFileName;	// PNG�t�@�C����
	int mTableIdx;		// �e�[�u��Index

	IconData() : mTableIdx(-1) {}
	IconData(const string &pFileName, int pTableIdx = -1) : mFileName(pFileName), mTableIdx(pTableIdx) {}
	bool operator == (const IconData &pData) const { return (mFileName == pData.mFileName); }
};

// std::hash<IconData>����ꉻ
template <>
struct hash<IconData> {
	size_t operator()(const IconData &data) const { return std::hash<string>()(data.mFileName); }
};

typedef vector<IconData>	IconDataVector;
typedef hashMap<IconData, int>		IconDataMap;
typedef IconDataMap::const_iterator	IconDataMapCItr;


class IconTable : public QTableWidget
{
	Q_OBJECT

public:
	IconTable(QWidget *pParent, MainWindow *pMainWin);

	int Init(const string &pDir, const IconDataVector *pDatas = NULL, vector<int> *newIconIDVec = NULL);
	void Close();
	const string & GetDir() const { return mIconDir; }
	void OutputFile(FILE *fp) const;
	void Move(int pVec);
	int GetSelectRowNum() const;
	int GetSelectColNum() const;

	string GetCurIconFileName() const;
	bool GetSelectZone(Zone &zone) const;
	bool GetPixmap(QPixmap &pPixmap, int pIconIdx) const;
	bool GetCurPixmap(QPixmap &pPixmap, int pOfsRow = 0, int pOfsCol = 0) const;
	int GetCurIconIdx(int pOfsRow = 0, int pOfsCol = 0) const;
	int GetTableIdx(int pIconIdx) const;
	void Clear();
	void ResetIconIdx(vector<int> &pNewIconIDVec);

private:
	bool GetPixmap(QPixmap &pixmap, int row, int col) const;
	void moveOne(int row1, int col1, int ofsRow, int ofsCol);
	bool eventFilter(QObject *obj, QEvent *e);

private:
	string mIconDir;		// �A�C�R���t�H���_
	IconDataVector mDatas;	// �A�C�R���f�[�^
	vector<int> mTable2ID;	// �e�[�u���ʒu����t�@�C��ID�ւ̃e�[�u��
	MainWindow *mMainWin;
};

#endif
