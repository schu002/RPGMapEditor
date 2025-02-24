2018/11/14 来栖

■概要
Qt5モジュールを動かすのに必要なライブラリをかき集めるツール。


■VisualStudioプロジェクトへの組み込み
実行ファイルのプロジェクト（例:xleva_hydb.vcxproj）で、
ビルド後イベントに以下の記述を指定する。

-- デバッグ --
powershell -NoProfile -ExecutionPolicy unrestricted $(ProjectDir)..\sys\Qt\qt5\copy_qt5libs.ps1 -DestDir $(TargetDir) -Qt5ListXml $(ProjectDir)..\sys\Qt\qt5\$(Platform)\debug.xml -Qt5Dir $(QTDIR) -AppendDir $(ProjectDir)..\sys\Qt\qt5\$(Platform)\debug

-- リリース --
powershell -NoProfile -ExecutionPolicy unrestricted $(ProjectDir)..\sys\Qt\qt5\copy_qt5libs.ps1 -DestDir $(TargetDir) -Qt5ListXml $(ProjectDir)..\sys\Qt\qt5\$(Platform)\release.xml -Qt5Dir $(QTDIR) -AppendDir $(ProjectDir)..\sys\Qt\qt5\$(Platform)\release


実行すると、Build_VS14\Win32\Release_DLLに必要な以下のファイルがコピーされる。
qt.conf
qt5/
