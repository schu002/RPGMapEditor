#ifndef GLOBAL_H
#define GLOBAL_H 1

#define	L_TRUE		1
#define	L_FALSE		0

#define	L_OS_WINDOWS	1

#if _MSC_VER >= 1300
#define L_COMPILER_VSNET	1
#else
#define L_COMPILER_VC6		1
#endif

#define L_ON			1
#define L_OFF			0

#define	R_NORMAL		1
#define R_ABNORMAL		0

#define	L_INT32MIN		0x80000000
#define	L_INT32MAX		0x7fffffff

/*
	g++では関数アドレスを渡すには関数名に&を付けなければならない。
	Sunprc CCではそれがエラーになるので、以下のマクロで吸収する。
*/
#if	__GNUC__
#define	M_FUNCADDR(name)	&name
#else
#define	M_FUNCADDR(name)	name
#endif

/*
    マクロ定義
*/
#define M_BZERO(d,n)    memset((char *)(d),0,n)
#define M_BCMP(s,d,n)   memcmp((char *)(d),(char *)(s),n)
#define	M_BCOPY(s,d,n)	memmove((char *)(d),(char *)(s),n)
#define	M_BFILL(v,d,n)	memset((char *)(d),v,n)
#define	M_MAX(a,b)		((a)<(b)?(b):(a))
#define	M_MIN(a,b)		((a)>(b)?(b):(a))
#define	M_ABS(a)		((a)<0?-(a):a)
#define	M_FTOI(x)		(int)(((x)<0.0)?((x)-0.5):((x)+0.5))
#define	M_FTOI64(x)		(T_INT64)(((x)<0.0)?((x)-0.5):((x)+0.5))
#define	M_FTOI_UP(x)	(((int)(x) == (x))? (int)(x) : ((int)(x)+(((x)>=0.0)? 1 : -1)))
#define	M_CENTER(a,b)	((a)+M_FTOI(((double)(b)-(double)(a))/2.0));

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

