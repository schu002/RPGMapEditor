#ifndef GLOBAL_H
#define GLOBAL_H 1

#include <stdio.h>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
using namespace std;

typedef vector<string> stringVector;
typedef unordered_set<string> stringSet;

#define	L_OS_WINDOWS	1

#if _MSC_VER >= 1300
#define L_COMPILER_VSNET	1
#else
#define L_COMPILER_VC6		1
#endif

#define QT5 1

#if	_DEBUG
#define	L_DEBUG			1
#else
#define	L_DEBUG			0
#endif

/*
    マクロ定義
*/
#define	M_MAX(a,b)		((a)<(b)?(b):(a))
#define	M_MIN(a,b)		((a)>(b)?(b):(a))
#define	M_ABS(a)		((a)<0?-(a):a)

////////////////////////////////////////////////

#if defined(WIN32)
#pragma warning (disable:4786) //識別子の文字列の長さが制限値（256文字）を超えたので、切り捨てられた。（RogueWaveのテンプレート）
#pragma warning( disable:4275) //エクスポートされたクラスが、エクスポートされていないクラスから派生している。（RogueWaveのテンプレート）
#pragma warning( disable:4251) //指定された基本クラスは、__declspec(dllexport) キーワードを使って宣言されていない。（RogueWaveのテンプレート）
#pragma warning( disable:4800 ) // 'int' : ブール値を 'true' または 'false' に強制的に設定します (stl)
#endif

//////////////////////////////////////////////
#if L_COMPILER_VSNET
#pragma warning( disable:4996 ) // strcpy, sscanf 等のバッファオーバーフロー警告 
#pragma warning( disable:4018 ) // signed と unsigned の数値を比較しようとしました。
#endif

//////////////////////////////////////////////
#endif

