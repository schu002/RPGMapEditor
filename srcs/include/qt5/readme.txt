2018/11/14 ����

���T�v
Qt5���W���[���𓮂����̂ɕK�v�ȃ��C�u�����������W�߂�c�[���B


��VisualStudio�v���W�F�N�g�ւ̑g�ݍ���
���s�t�@�C���̃v���W�F�N�g�i��:xleva_hydb.vcxproj�j�ŁA
�r���h��C�x���g�Ɉȉ��̋L�q���w�肷��B

-- �f�o�b�O --
powershell -NoProfile -ExecutionPolicy unrestricted $(ProjectDir)..\sys\Qt\qt5\copy_qt5libs.ps1 -DestDir $(TargetDir) -Qt5ListXml $(ProjectDir)..\sys\Qt\qt5\$(Platform)\debug.xml -Qt5Dir $(QTDIR) -AppendDir $(ProjectDir)..\sys\Qt\qt5\$(Platform)\debug

-- �����[�X --
powershell -NoProfile -ExecutionPolicy unrestricted $(ProjectDir)..\sys\Qt\qt5\copy_qt5libs.ps1 -DestDir $(TargetDir) -Qt5ListXml $(ProjectDir)..\sys\Qt\qt5\$(Platform)\release.xml -Qt5Dir $(QTDIR) -AppendDir $(ProjectDir)..\sys\Qt\qt5\$(Platform)\release


���s����ƁABuild_VS14\Win32\Release_DLL�ɕK�v�Ȉȉ��̃t�@�C�����R�s�[�����B
qt.conf
qt5/
