#ifndef utilenv_h
#define utilenv_h 1

#include <string>
using namespace std;

class FileUtil
{
public:
	/*!
	 * @brief �J�����g�f�B���N�g���𓾂�B
	 * @param paddsepflg (i)�F�Ō��"\ �� /"�����邩�ۂ�
	 *							0�F���Ȃ��A�P�F����
	 * @retval �f�B���N�g����
	 */
	static string GetCurDir(int paddsepflg = 0);
};

#endif
