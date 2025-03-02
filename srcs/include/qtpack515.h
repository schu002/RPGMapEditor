#ifndef qtpack515_h
#define qtpack515_h 1

#define QT_THREAD_SUPPORT 1
#include <QtWidgets/QDockWidget>
#include <QtWidgets/qapplication.h>
#include <QtGui/qguiapplication.h>
#include <QtWidgets/qdesktopwidget.h>

#include <QtCore/QQueue>
#include <QtCore/QStack>
#include <QtCore/QDateTime>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QTextStream>
#include <QtCore/QLibraryInfo>
#include <QtCore/QPluginLoader>
#include <QtCore/QEvent>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtCore/QCache>
#include <QtCore/QSocketNotifier>
#include <QtCore/QTranslator>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPdfWriter>
#include <QtCore/qtimer.h>
#include <QtCore/qmimedata.h>
#include <QtCore/qtextcodec.h>
#include <QtCore/QPointer>
#include <QtCore/QMetaProperty>
#include <QtCore/QUuid>
#include <QtCore/QLockFile>
//#include <QtWidgets/QMouseEventTransition>
//#include <QtWidgets/QKeyEventTransition>
#include <QtXml/qdom.h>

#include <QtWidgets/QStylePainter>
#include <QtWidgets/QAction>
#include <QtWidgets/QLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
//#include <QtWidgets/QFile>
//#include <QtWidgets/QDataStream>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
//#include <QtWidgets/QSignalMapper>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/qmainwindow.h> 
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qspinbox.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qbuttongroup.h>
#include <QtWidgets/qradiobutton.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qmdiarea.h>
#include <QtWidgets/qmdisubwindow.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qslider.h>
#include <QtGui/QPixmapCache>
#include <QtGui/QBitmap>
#include <QtGui/QImageReader>
#include <QtGui/QImageWriter>
#include <QtGui/QMovie>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/qtablewidget.h> 
#include <QtWidgets/qtreewidget.h> 
#include <QtGui/QTextBlock> 
#include <QtWidgets/qtooltip.h> 
#include <QtWidgets/qprogressdialog.h> 
#include <QtWidgets/qprogressbar.h> 
#include <QtWidgets/qcolormap.h> 
#include <QtGui/qpalette.h> 
#include <QtWidgets/qinputdialog.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qstylefactory.h>
#include <QtWidgets/qcommonstyle.h>
#include <QtWidgets/qproxystyle.h>
#include <QtWidgets/QHeaderView>
#include <QtGui/qfontdatabase.h> 
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QDrag>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtGui/QScreen>
#include <QtGui/QOpenGLFramebufferObject> 
#include <QtGui/QOpenGLPaintDevice> 
#include <QtOpenGL/QGLWidget> 
#include <QtWidgets/QOpenGLWidget> 
#include <QtWidgets/QItemDelegate> 
#include <QtWidgets/QWidgetAction> 
#include <QtWidgets/QStackedWidget> // Add 180611 QT5移植
#include <QtWidgets/QGraphicsScene> // Add 180621 QT5移植
#include <QtWidgets/QGraphicsLineItem> // Add 180621 QT5移植
#include <QtWidgets/QGraphicsRectItem> // Add 180621 QT5移植
#include <QtWidgets/QGraphicsView> // Add 180621 QT5移植
#include <QtWidgets/QFontDialog>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QStyledItemDelegate>
//#include <QtWidgets/Qdebug.h>
#include <QtCore/QUrlQuery>
#include <QtQuick/QQuickImageProvider>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickPaintedItem>
#include <QtQuickWidgets/QQuickWidget>


/// Add 180724 内部ブラウザ新規作成
#include <QtCore/QUrl> 
#include <QtCore/QUrlQuery>
#include <QtCore/QStandardPaths>
#include <QtCore/QState>
#include <QtCore/QStateMachine>
#include <QtCore/QSignalTransition>
#include <QtCore/QMetaObject>
/// Add 180724 

/// Add 180810 C++ <--> HTML/js 双方向通信
#include <QtCore/QJsonDocument> 
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QTextCodec>
// #include <QtWebChannel/QWebChannel>
// #include <QtWebChannel/QWebChannelAbstractTransport>
// #include <QtWebSockets/QWebSocket>
// #include <QtWebSockets/QWebSocketServer>
/// Add 180810

//
// 国際化（Internationalization）対応
//    QTで日本語を取り扱うには，Unicodeに変換する必要がある。
//
//  Localization（日本語など） -> Unicode
//     static QString::fromLocal8Bit ( const char *, int len=-1 ) 
// #define MQ_toUniStr QString::fromLocal8Bit
#define M_QSTR(str)		QString::fromLocal8Bit((str).c_str())

//  Unicode -> Localization（日本語など）
//     QCString QString::local8Bit ()

#if L_OS_UNIX
#include <QtX11Extras/QX11Info>
#endif

// #include	"qtproperty.h"
// #include	"qt3support/qt3supportpack.h"

#endif
