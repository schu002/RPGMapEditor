#ifndef GLOBAL_H
#define GLOBAL_H 1

#define	L_OS_WINDOWS	1

#if _MSC_VER >= 1300
#define L_COMPILER_VSNET	1
#else
#define L_COMPILER_VC6		1
#endif

/*
    �}�N����`
*/
#define	M_MAX(a,b)		((a)<(b)?(b):(a))
#define	M_MIN(a,b)		((a)>(b)?(b):(a))
#define	M_ABS(a)		((a)<0?-(a):a)

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

