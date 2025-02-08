#ifndef messagecontrol_h
#define messagecontrol_h 1

#include <set>
using namespace std;
typedef set<Message> MessageList;

#include "common/message.h"

class MessageControl
{
public:
	/*!
	 * @brief ���b�Z�[�W�����[�h����
	 * @param  pmsgfile (i) ���b�Z�[�W�t�@�C������
	 * @retval R_NORMAL�F����
	 *		   R_ABNORMAL�F�t�@�C��Open/Read�G���[
	 */
	static int Load(const string &pMsgFile);

	/*!
	 * @brief �w�肵�����b�Z�[�W�R�[�h���烁�b�Z�[�W�I�u�W�F�N�g���擾����D
	 * @param pcode (i) ���b�Z�[�W�R�[�h
	 * @retval ���b�Z�[�W�I�u�W�F�N�g�̃|�C���^
	 */
	static const Message * Find(int pcode);

	static void Clear();

private:
	MessageControl() {}
	int _Load(const string &pMsgFile);
	void AddMessage(const string &pLine, int &pCode);

private:
	string mPrefixOfEngMes;
	MessageList mMsgList;
	static MessageControl *mMyself;
};

#endif
