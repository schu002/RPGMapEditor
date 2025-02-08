#ifndef utilenv_h
#define utilenv_h 1

#include <string>
using namespace std;

class FileUtil
{
public:
	/*!
	 * @brief カレントディレクトリを得る。
	 * @param paddsepflg (i)：最後に"\ か /"をつけるか否か
	 *							0：つけない、１：つける
	 * @retval ディレクトリ名
	 */
	static string GetCurDir(int paddsepflg = 0);
};

#endif
