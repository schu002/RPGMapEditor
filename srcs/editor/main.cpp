#include "maindef.h"
#include "qtpack.h"
#include "qwindowsstyle.h"

int main(int argc, char* argv[])
{
	MyQApplication app(argc, argv);
	string curDir = FileUtil::GetCurDir();
	string msgfile = curDir + "/mapeditor.hh";
	if (!MessageControl::Load(msgfile)) return 0;

	MainWindow *win = new MainWindow(curDir, Qt::WType_Dialog|Qt::WStyle_Minimize);
	win->show();
//	win->GetMapTable()->setFocus();
//	win->resize(800, 500);
	app.setMainWidget(win);
	app.exec();

    return 0;
}
