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
	 * @brief メッセージをロードする
	 * @param  pmsgfile (i) メッセージファイル名称
	 * @retval R_NORMAL：正常
	 *		   R_ABNORMAL：ファイルOpen/Readエラー
	 */
	static int Load(const string &pMsgFile);

	/*!
	 * @brief 指定したメッセージコードからメッセージオブジェクトを取得する．
	 * @param pcode (i) メッセージコード
	 * @retval メッセージオブジェクトのポインタ
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
