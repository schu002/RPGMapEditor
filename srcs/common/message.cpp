#include "common/commondef.h"
#include "common/message.h"
#include <qregexp.h> 
#include <stdio.h>
#include <stdarg.h>

namespace {

/*!
	print用の内部関数
*/
void PrintMessage(const char *buf, int pcrlf = 1)
{
	printf("%s", buf);
	if (pcrlf) printf("\n");
	fflush(stdout);
}

/*!
	自動で改行しない以外はMessage::Printと同じ。Cからの呼び出し用
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
	メッセージ文字列を作成する。\n
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

	// テキストを読んだメッセージなので、改行は文字列として格納されている。
	// このため、"\\n" を "\n" の本当の改行コードに置き換える。
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
	sprintf形式で変換した文字列を表示する。\n
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
