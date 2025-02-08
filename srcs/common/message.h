#ifndef message_h
#define message_h 1

enum {
	L_MSG_INFO = 0,
	L_MSG_WARN,
	L_MSG_ERROR,
	L_MSG_FATAL,
	L_MSG_ALL
};

class Message
{
public:
	/*!
	 * @brief コンストラクタ
	 * @param pCode  (i) メッセージコード
	 * @param pLevel (i) メッセージレベル
	 * @param pMsg　 (i) メッセージ
	 */
	Message(int pCode, int pLevel, const char *pMsg);

	//	等価演算子
	bool operator ==(const Message &pMsg) const { return (mCode == pMsg.mCode); }

	//	比較演算子
	bool operator <(const Message &pMsg) const { return (mCode < pMsg.mCode); }

	/*!
	 * @brief  メッセージの取得
	 * @retval メッセージ
	 */
	const char * GetMessage() const { return mMessage.data(); }

	//	メッセージレベルを返す．
	int GetLevel() const { return mLevel; }

	//	メッセージコードを返す．
	int GetCode() const { return mCode; }

	/*!
	 * @brief メッセージコードと書式より文字列を作成するstatic関数。
	 * @param  pcode (i) メッセージコード
	 * @param  pstr1 [... pstr6] (i) 文字列
	 * @retval メッセージ文字列
	 */
	static string TrC(int pcode, ...);

	static void Print(const char *pformat, ...);

private:
	int mCode;	// メッセージコード
	int mLevel;	// メッセージレベル
	string mMessage;
};

#endif
