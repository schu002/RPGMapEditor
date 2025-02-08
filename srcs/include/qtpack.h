#ifndef qtpack_h
#define qtpack_h 1
#define QT_THREAD_SUPPORT 1
#include <stddef.h>

#include <qiconview.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qapplication.h>
#include <qmainwindow.h> 
#include <qworkspace.h> 

#include <qcache.h> 
#include <qpixmapcache.h> 

#include <qwidget.h> 
#include <qimage.h>
#include <qpixmap.h>
#include <qiconset.h>

#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qaccel.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtooltip.h>
#include <qspinbox.h>
#include <qcombobox.h>

#include <qhbox.h>
#include <qvbox.h>
#include <qdialog.h> 
#include <qtabdialog.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qprogressbar.h>

#include <qheader.h>
#include <qdragobject.h>
#include <qclipboard.h>

#include <qpainter.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qprinter.h>

#include <qsplitter.h>
#include <qstring.h>

#include <qlist.h>
#include <qlistview.h>
#include <qtable.h> 
#include <qtextview.h> 
#include <qtextbrowser.h> 
#include <qinputdialog.h> 
#include <qvalidator.h>
#include <qthread.h>
#include <qprogressdialog.h>
#include <qgrid.h>
#include <qvbuttongroup.h>
#include <qprocess.h>
#include <qtimer.h>
#include "qstyle.h"
#include "qdom.h"

#define M_QSTR(str)			QString::fromLocal8Bit((str).c_str())

#endif
