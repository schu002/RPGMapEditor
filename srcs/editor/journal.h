#ifndef journal_h
#define journal_h 1

#include <vector>
using namespace std;

#include "zone.h"

// ����
enum {
	L_OPE_OPEN = 1,	// �}�b�v�f�[�^�I�[�v��
	L_OPE_INPUT,	// �A�C�R������
};

// �W���[�i��
class Journal {
public:
	Journal(int pRowNum, int pColNum, int pOpe, const vector<int> &pData,
			const Zone &pSelZone, const Point &pCurPos);

public:
	int mRowNum, mColNum;	// �}�b�v�T�C�Y
	int mOperation;		// ����
	Zone mSelZone;		// �I��̈�
	Point mCurPos;		// �J�����grow,col�ʒu
	vector<int>	mData;	// �A�C�R���f�[�^
};
typedef vector<Journal>	JournalStack;

#endif
