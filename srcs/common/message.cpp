#include "common/commondef.h"
#include "common/message.h"
#include <qregexp.h> 
#include <stdio.h>
#include <stdarg.h>

namespace {

/*!
	print�p�̓����֐�
*/
void PrintMessage(const char *buf, int pcrlf = 1)
{
	printf("%s", buf);
	if (pcrlf) printf("\n");
	fflush(stdout);
}

/*!
	�����ŉ��s���Ȃ��ȊO��Message::Print�Ɠ����BC����̌Ăяo���p
*/
extern "C" void Message_Print(const char *pformat, ...)
{
	va_list marker;
	char buffer[8192];
	va_start(marker, pformat);
	vsprintf(buffer, pformat, marker);
	va_end(marker);
	PrintMessage(buffer, 0);
}

};

Message::Message(int pCode, int pLevel, const char *pMsg)
{
	mCode = pCode;
	mLevel = pLevel;
	mMessage = pMsg;
}

/*!
	\fn const char * Message::TrC(int pcode, ...)
	���b�Z�[�W��������쐬����B\n
*/
string Message::TrC(int pcode, ...)
{
	const Message *msg = MessageControl::Find(pcode);
	if (!msg) {
		char outmes[512];
		sprintf(outmes, "#W: message code <%d> not found", pcode);
		return outmes;
	}

	string str;
	switch (msg->mLevel) {
	case L_MSG_INFO:	str = "#I: "; break;
	case L_MSG_WARN:	str = "#W: "; break;
	case L_MSG_ERROR:	str = "#E: "; break;
	case L_MSG_FATAL:	str = "#F: "; break;
	}
	str += msg->GetMessage();

	// �e�L�X�g��ǂ񂾃��b�Z�[�W�Ȃ̂ŁA���s�͕�����Ƃ��Ċi�[����Ă���B
	// ���̂��߁A"\\n" �� "\n" �̖{���̉��s�R�[�h�ɒu��������B
	QString qstr = str.c_str();
	qstr.replace(QRegExp("\\\\n"), "\n");
	const char *format = qstr.latin1();

	va_list arg;
	va_start(arg, pcode);
	char buffer[4096];
	vsprintf(buffer, format, arg);
	va_end(arg);

	str = buffer;
	return str;
}

/*!
	sprintf�`���ŕϊ������������\������B\n
*/
void Message::Print(const char *pformat, ...)
{
	va_list marker;
	char buffer[8192];
	va_start(marker, pformat);
	vsprintf(buffer, pformat, marker);
	va_end(marker);

	PrintMessage(buffer);
}
