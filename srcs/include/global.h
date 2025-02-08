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
	g++�ł͊֐��A�h���X��n���ɂ͊֐�����&��t���Ȃ���΂Ȃ�Ȃ��B
	Sunprc CC�ł͂��ꂪ�G���[�ɂȂ�̂ŁA�ȉ��̃}�N���ŋz������B
*/
#if	__GNUC__
#define	M_FUNCADDR(name)	&name
#else
#define	M_FUNCADDR(name)	name
#endif

/*
    �}�N����`
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
#pragma warning (disable:4786) //���ʎq�̕�����̒����������l�i256�����j�𒴂����̂ŁA�؂�̂Ă�ꂽ�B�iRogueWave�̃e���v���[�g�j
#pragma warning( disable:4275) //�G�N�X�|�[�g���ꂽ�N���X���A�G�N�X�|�[�g����Ă��Ȃ��N���X����h�����Ă���B�iRogueWave�̃e���v���[�g�j
#pragma warning( disable:4251) //�w�肳�ꂽ��{�N���X�́A__declspec(dllexport) �L�[���[�h���g���Đ錾����Ă��Ȃ��B�iRogueWave�̃e���v���[�g�j
#pragma warning( disable:4800 ) // 'int' : �u�[���l�� 'true' �܂��� 'false' �ɋ����I�ɐݒ肵�܂� (stl)
#endif

//////////////////////////////////////////////
#if L_COMPILER_VSNET
#pragma warning( disable:4996 ) // strcpy, sscanf ���̃o�b�t�@�I�[�o�[�t���[�x�� 
#pragma warning( disable:4018 ) // signed �� unsigned �̐��l���r���悤�Ƃ��܂����B
#endif

//////////////////////////////////////////////
#endif

