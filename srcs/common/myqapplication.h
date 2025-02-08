#ifndef fguiqapplication_h
#define fguiqapplication_h 1

#define _USE_MATH_DEFINES
#include <math.h>
#include "qtpack.h"
#include "qwindowsstyle.h"

class MyQApplication : public QApplication
{
public :
	MyQApplication(int &argc, char **argv);
};

class MyWinStyle : public QWindowsStyle
{
public:
	// グラデーションの方向
	enum GradationDirection
	{
		GradationTop2Bottom,	// 上から下
		GradationLeft2Right,	// 左から右
	};

public:
	MyWinStyle();
	virtual ~MyWinStyle();

protected:
	static void drawGradation(QPainter *p, int x, int y, int w, int h, const QColor &colorStart, const QColor &colorEnd, GradationDirection direction = GradationTop2Bottom);
	static QColor calcGradation(const QColor &colorStart, const QColor &colorEnd, int value, int scale);
};

class MyWinStyleNew : public MyWinStyle
{
public:
	MyWinStyleNew();
	virtual ~MyWinStyleNew();
	virtual void polish(QWidget *w);
	virtual void unPolish(QWidget *w);
	virtual void polish(QApplication *app);
	virtual void unPolish(QApplication *app);
	virtual void drawItem(QPainter *p, const QRect &r, int flags, const QColorGroup &g,
		bool enabled, const QPixmap *pixmap, const QString &text,
		int len = -1, const QColor *penColor = 0) const;
	virtual void drawPrimitive(PrimitiveElement pe, QPainter *p, const QRect &r,
		const QColorGroup &cg, SFlags flags = QStyle::Style_Default,
		const QStyleOption &opt = QStyleOption::Default) const;
	virtual void drawControl(ControlElement element, QPainter *p, const QWidget *widget,
		const QRect &r, const QColorGroup &cg, SFlags how = Style_Default,
		const QStyleOption &opt = QStyleOption::Default) const;
	virtual void drawControlMask(ControlElement element, QPainter *p, const QWidget *widget,
		const QRect &r, const QStyleOption &opt = QStyleOption::Default) const;
	virtual QRect subRect(SubRect subrect, const QWidget *widget) const;
	virtual void drawComplexControl(ComplexControl control, QPainter *p,
		const QWidget *widget, const QRect &r, const QColorGroup &cg,
		SFlags how = Style_Default, SCFlags sub = SC_All, SCFlags subActive = SC_None,
		const QStyleOption &opt = QStyleOption::Default) const;
	virtual void drawComplexControlMask(ComplexControl control, QPainter *p,
		const QWidget *widget, const QRect &r,
		const QStyleOption &opt = QStyleOption::Default) const;
	virtual QRect querySubControlMetrics(ComplexControl control, const QWidget *widget,
		QStyle::SubControl subcontrol, const QStyleOption &opt = QStyleOption::Default) const;
	virtual QStyle::SubControl querySubControl(ComplexControl control, const QWidget *widget,
		const QPoint &pos, const QStyleOption &opt = QStyleOption::Default) const;
	virtual int pixelMetric(PixelMetric metric, const QWidget* widget = 0) const;
	virtual QSize sizeFromContents(ContentsType contents, const QWidget *widget,
		const QSize &contentsSize, const QStyleOption &opt = QStyleOption::Default) const;
	virtual int styleHint(StyleHint stylehint, const QWidget *widget = 0,
		const QStyleOption &opt = QStyleOption::Default,
		QStyleHintReturn *returnData = 0) const;

private:
	void drawToolbarGradation(QPainter *p, const QColorGroup &cg, const QRect &r, bool isHorizontal = true) const;
	void drawTextBox(QPainter *p, const QColorGroup &cg, int x, int y, int w, int h, bool isFill = true) const;
	void drawButton(QPainter *p, const QColorGroup &cg, int x, int y, int w, int h, bool isEnabled = true, bool isOn = false, GradationDirection direction = GradationTop2Bottom) const;

private:
	QPalette mPaletteOld;
	QColorGroup *mColorGroupActive;
	QColorGroup *mColorGroupDisabled;
	QColorGroup *mColorGroupInactive;
};

#endif
