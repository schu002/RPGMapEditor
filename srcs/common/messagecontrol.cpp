#include "common/commondef.h"
#include "common/messagecontrol.h"
#include <fstream>

#undef DEBUG
#define DEBUG if(0)

namespace {

const string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string &s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == string::npos)? "" : s.substr(start);
}

string rtrim(const string &s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == string::npos)? "" : s.substr(0, end + 1);
}

string trim(const string &s)
{
	return rtrim(ltrim(s));
}

bool check_int(const string &str, int *val = NULL)
{
	if (std::all_of(str.cbegin(), str.cend(), isdigit)) {
		if (val) *val = stoi(str);
		return true;
	}
	return false;
}

};

#define L_SepPathStr ";"

MessageControl * MessageControl::mMyself = NULL;


int MessageControl::Load(const string &pMsgFile)
{
	if (!mMyself) mMyself = new MessageControl();
	return mMyself->_Load(pMsgFile);
}

int MessageControl::_Load(const string &pMsgFile)
{
	ifstream ifs(pMsgFile.c_str());
	if (ifs.fail()) return R_ABNORMAL;

	int code = 0, enumFlg = 0;
	string line;
	while (getline(ifs, line)) {
		line = trim(line);
		if (line.size() < 5) continue;
		if (line[0] == '/' && line[1] == '/') continue;

		if (line.substr(0, 4) == "enum") {
			enumFlg = 1;
		} else if (line[0] == '}') {
			enumFlg = 0;
		} else {
			if (enumFlg) AddMessage(line, code);
		}
	}
	ifs.close();
	return R_NORMAL;
}

void MessageControl::AddMessage(const string &pLine, int &pCode)
{
	int level = -1;
	int idx = pLine.find(",");
	if (idx < 2 || idx == string::npos) return;
	string str = pLine.substr(0, idx);
	str = trim(str);
	string msg = pLine.substr(idx+1, pLine.size()-idx-1);
	msg = trim(msg);

	idx = str.find("=");
	if (idx > 1) {
		string wkstr = str.substr(idx+1, str.size()-idx-1);
		wkstr = trim(wkstr);
		int code;
		if (!check_int(wkstr, &code)) return;
		pCode = code;
		str = str.substr(0, idx);
		str = trim(str);
	} else {
		pCode++;
	}

	if (msg.size() > 3 && msg[0] == '/' && msg[1] == '/') {
		msg = msg.substr(2, msg.size()-2);
		msg = trim(msg);
		idx = msg.find(":");
		if (idx >= 0 && idx < 5) {
			string wkstr = msg.substr(0, idx);
			wkstr = trim(wkstr);
			if (wkstr.empty()) {
				msg = msg.substr(idx+1, msg.size()-idx-1);
				msg = trim(msg);
			} else if (wkstr.size() == 1) {
				if		(wkstr[0] == 'I') level = L_MSG_INFO;
				else if (wkstr[0] == 'W') level = L_MSG_WARN;
				else if (wkstr[0] == 'E') level = L_MSG_ERROR;
				else if (wkstr[0] == 'F') level = L_MSG_FATAL;
				msg = msg.substr(idx+1, msg.size()-idx-1);
				msg = trim(msg);
			}
		}
	}

	if (!msg.empty()) mMsgList.insert(Message(pCode, level, msg.c_str()));
}

const Message * MessageControl::Find(int pcode)
{
	Message srmsg(pcode, L_MSG_INFO, "");
	MessageList::const_iterator itr = mMyself->mMsgList.find(srmsg);
	if (itr == mMyself->mMsgList.end()) return NULL;
	const Message &msg = *itr;
	return &msg;  // const íœiVersion‚Ìˆá‚¢‚ð‹zŽûj
}

void MessageControl::Clear()
{
	mMyself->mMsgList.clear();
}
