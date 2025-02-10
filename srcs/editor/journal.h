#ifndef journal_h
#define journal_h 1

#include <vector>
using namespace std;

#include "zone.h"

// 操作
enum {
	L_OPE_INPUT = 1,	// アイコン入力
	L_OPE_CLEAR,		// クリア
};

// ジャーナル
class Journal {
public:
	Journal(int pRowNum, int pColNum, int pOpe, const vector<int> &pData,
			const Zone &pSelZone, const Point &pCurPos);

	bool IsSelect() const; // 選択状態のときに行う操作かどうか
	void Dump() const;

public:
	int mRowNum, mColNum;	// マップサイズ
	int mOperation;		// 操作
	Zone mSelZone;		// 選択領域
	Point mCurPos;		// カレントrow,col位置
	vector<int>	mData;	// アイコンデータ
};
typedef vector<Journal>	JournalStack;

#endif
