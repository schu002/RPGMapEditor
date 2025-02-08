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
	 * @brief �R���X�g���N�^
	 * @param pCode  (i) ���b�Z�[�W�R�[�h
	 * @param pLevel (i) ���b�Z�[�W���x��
	 * @param pMsg�@ (i) ���b�Z�[�W
	 */
	Message(int pCode, int pLevel, const char *pMsg);

	//	�������Z�q
	bool operator ==(const Message &pMsg) const { return (mCode == pMsg.mCode); }

	//	��r���Z�q
	bool operator <(const Message &pMsg) const { return (mCode < pMsg.mCode); }

	/*!
	 * @brief  ���b�Z�[�W�̎擾
	 * @retval ���b�Z�[�W
	 */
	const char * GetMessage() const { return mMessage.data(); }

	//	���b�Z�[�W���x����Ԃ��D
	int GetLevel() const { return mLevel; }

	//	���b�Z�[�W�R�[�h��Ԃ��D
	int GetCode() const { return mCode; }

	/*!
	 * @brief ���b�Z�[�W�R�[�h�Ə�����蕶������쐬����static�֐��B
	 * @param  pcode (i) ���b�Z�[�W�R�[�h
	 * @param  pstr1 [... pstr6] (i) ������
	 * @retval ���b�Z�[�W������
	 */
	static string TrC(int pcode, ...);

	static void Print(const char *pformat, ...);

private:
	int mCode;	// ���b�Z�[�W�R�[�h
	int mLevel;	// ���b�Z�[�W���x��
	string mMessage;
};

#endif
