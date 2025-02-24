#include "maindef.h"

#define L_MAPSIZE_ROW	30
#define L_MAPSIZE_COL	30
#define L_PIXSIZE	32

int main(int argc, char * argv[])
{
	QApplication app(argc, argv);
	string curDir = FileUtil::GetCurDir();
	string msgfile = curDir + "/mapeditor.hh";
	if (!MessageControl::Load(msgfile)) return 0;

	MainWindow *win = new MainWindow(curDir);
	win->resize(1200, 800);
	win->show();
	win->Initiate();
	return app.exec();
}
