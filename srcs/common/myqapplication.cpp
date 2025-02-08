#include "common/commondef.h"
#include "common/myqapplication.h"

#define L_STYLE_FLAG_NO_FILL_BACKGROUND	0x80000000	// ←QStyle::StyleFlagsで使われていない値

MyQApplication::MyQApplication(int &argc, char **argv)
 : QApplication(argc, argv)
{
	// ライブラリパスから、インストールディレクトリを省く。
	// デフォルトでインストールディレクトリだから、実行ディレクトリだけにする。
	QStringList liblist;
	liblist.append(applicationDirPath());
	setLibraryPaths(liblist);

	QStyle *style = new MyWinStyleNew;
	QApplication::setStyle(style);
}

MyWinStyle::MyWinStyle()
	: QWindowsStyle()
{
}

MyWinStyle::~MyWinStyle()
{
}

void MyWinStyle::drawGradation(QPainter* p, int x, int y, int w, int h, const QColor& colorStart, const QColor& colorEnd
							, MyWinStyle::GradationDirection direction)
{
	QPen pen_org = p->pen();
	int num = (direction == MyWinStyle::GradationTop2Bottom ? h : w);

	for (int line = 0 ; line < num ; line++) {
		QColor color = calcGradation(colorStart, colorEnd, line, num);
		p->setPen(color);
		if (direction == MyWinStyle::GradationTop2Bottom) {
			p->drawLine(x, y + line, x + w, y + line);
		} else {
			p->drawLine(x + line, y, x + line, y + h);
		}
	}
	p->setPen(pen_org);
}

QColor MyWinStyle::calcGradation(const QColor& colorStart, const QColor& colorEnd, int value, int scale)
{
	if (value <= 0) return colorStart;
	if (value >= scale) return colorEnd;

	// 線形補間でグラーデーションの生成
	int r = (colorStart.red  () * ((scale-1) - value) + colorEnd.red  () * value) / (scale-1);
	int g = (colorStart.green() * ((scale-1) - value) + colorEnd.green() * value) / (scale-1);
	int b = (colorStart.blue () * ((scale-1) - value) + colorEnd.blue () * value) / (scale-1);
	return QColor(r, g, b);
}


MyWinStyleNew::MyWinStyleNew()
	: MyWinStyle()
	, mColorGroupActive(NULL), mColorGroupDisabled(NULL), mColorGroupInactive(NULL)
{
}
MyWinStyleNew::~MyWinStyleNew()
{
}

void MyWinStyleNew::polish(QWidget* w)
{
	QWindowsStyle::polish(w);
}

void MyWinStyleNew::unPolish(QWidget* w)
{
	QWindowsStyle::unPolish(w);
}

void MyWinStyleNew::polish(QApplication* app)
{
	// ベースのブラシ（色）を設定していく
	this->mPaletteOld = app->palette();
	QWindowsStyle::polish(app);

	// ■以下の色情報は、iniファイルか何かに書きたい→そうすればコンパイルなしに色を変えられる
	QColor color_active_fore       (0x1e, 0x21, 0x22);	// 文字色（ラベル、タブ）、ツールチップ縁
	QColor color_active_button     (0xeb, 0xec, 0xec);	// ボタン背景色
	QColor color_active_light      (0xfd, 0xfe, 0xfe);	// ウィジェット縁明
	QColor color_active_mid        (0xd3, 0xd4, 0xd4);	// ウィジェット縁暗
	QColor color_active_dark       (0xc8, 0xd0, 0xd8);	// MDIウィンドウ背景色
	QColor color_active_text       (0x1e, 0x21, 0x22);	// 文字色（fore以外。ボタン、テキストボックス、メニュー）
	QColor color_active_bright_text(0xfe, 0xff, 0xff);	// 謎
	QColor color_active_base       (0xfe, 0xff, 0xff);	// テキストボックス背景色
	QColor color_active_background (0xeb, 0xec, 0xec);	// ダイアログ背景色
	QColor color_active_shadow     (0x88, 0x8c, 0x8e);	// 影
	QColor color_active_highlight  (0xb4, 0xd4, 0xff);	// ハイライト背景色
	QColor color_active_highlight_t(0x1e, 0x21, 0x22);	// ハイライト文字色

	QColorGroup* p_cgp_active = new QColorGroup(QBrush(color_active_fore), QBrush(color_active_button)
		, QBrush(color_active_light), QBrush(color_active_dark), QBrush(color_active_mid)
		, QBrush(color_active_text), QBrush(color_active_bright_text), QBrush(color_active_base)
		, QBrush(color_active_background));
	p_cgp_active->setColor(QColorGroup::Shadow, color_active_shadow);
	p_cgp_active->setColor(QColorGroup::Highlight, color_active_highlight);
	p_cgp_active->setColor(QColorGroup::HighlightedText, color_active_highlight_t);

	QColor color_disabled_fore       (color_active_fore       .light(400));
	QColor color_disabled_button     (color_active_button     );
	QColor color_disabled_light      (color_active_light      );
	QColor color_disabled_dark       (color_active_dark       .light(150));
	QColor color_disabled_mid        (color_active_mid        );
	QColor color_disabled_text       (color_active_text       .light(400));
	QColor color_disabled_bright_text(color_active_bright_text.light(400));
	QColor color_disabled_base       (color_active_base       .dark(110));
	QColor color_disabled_background (color_active_background );
	QColor color_disabled_shadow     (color_active_shadow     );
	QColorGroup* p_cgp_disabled = new QColorGroup(QBrush(color_disabled_fore), QBrush(color_disabled_button)
		, QBrush(color_disabled_light), QBrush(color_disabled_dark), QBrush(color_disabled_mid)
		, QBrush(color_disabled_text), QBrush(color_disabled_bright_text), QBrush(color_disabled_base)
		, QBrush(color_disabled_background));
	p_cgp_disabled->setColor(QColorGroup::Shadow, color_disabled_shadow);

	// 非アクティブ
	QColorGroup* p_cgp_inactive = NULL;
	{
		QColor color_active_highlight  (0xda, 0xdc, 0xe0);	// ハイライト背景色
		p_cgp_inactive = new QColorGroup((*p_cgp_active));
		p_cgp_inactive->setColor(QColorGroup::Highlight, color_active_highlight);
	}

	//app->setPalette(QPalette(*p_cgp_active, *p_cgp_disabled, *p_cgp_inactive), TRUE);
	app->setPalette(QPalette(*p_cgp_active, *p_cgp_disabled, p_cgp_inactive ? (*p_cgp_inactive) : (*p_cgp_active)), TRUE);
	this->mColorGroupActive = p_cgp_active;
	this->mColorGroupDisabled = p_cgp_disabled;
	this->mColorGroupInactive = p_cgp_inactive;
}

void MyWinStyleNew::unPolish(QApplication* app)
{
	QWindowsStyle::unPolish(app);

	app->setPalette(this->mPaletteOld, TRUE);
	if (this->mColorGroupActive) { delete this->mColorGroupActive; this->mColorGroupActive = NULL; }
	if (this->mColorGroupDisabled) { delete this->mColorGroupDisabled; this->mColorGroupDisabled = NULL; }
	if (this->mColorGroupInactive) { delete this->mColorGroupInactive; this->mColorGroupInactive = NULL; }
}

void MyWinStyleNew::drawItem(QPainter* p, const QRect& r, int flags, const QColorGroup& g
	, bool enabled, const QPixmap* pixmap, const QString& text
	, int len, const QColor* penColor) const
{
	QWindowsStyle::drawItem(p, r, flags, g, enabled, pixmap, text, len, penColor);
}

void MyWinStyleNew::drawPrimitive(PrimitiveElement pe, QPainter* p, const QRect& r
	, const QColorGroup& cg, SFlags flags, const QStyleOption& opt) const
{
	switch (pe) {
	case QStyle::PE_ArrowUp:						// スクロールバー▲
		// fall through
	case QStyle::PE_ArrowDown:					// スクロールバー▼
		// fall through
	case QStyle::PE_ArrowRight:					// スクロールバー >
		// fall through
	case QStyle::PE_ArrowLeft:					// スクロールバー <
		{
			//QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
			QColor color_dark = cg.foreground();
			QColor color_light = cg.dark();	// lightとdarkでおかしいように見えるかもしれないが、これで意図は合ってる
			QColor color_mix_dark  = calcGradation(color_dark, color_light, 32, 256);
			QColor color_mix_mid   = calcGradation(color_dark, color_light, 64, 256);
			QColor color_mix_light = calcGradation(color_dark, color_light, 96, 256);
			color_dark = calcGradation(color_dark, color_light, 48, 256);
			QPointArray pts_dark;
			QPointArray pts_mix_dark;
			QPointArray pts_mix_mid;
			QPointArray pts_mix_light;
			switch (pe) {
			case QStyle::PE_ArrowUp:
				pts_dark.setPoints(3, -4,1, -1,-2, 1,0);
				pts_mix_dark.setPoints(2, -3,1, 2,1);
				pts_mix_mid.setPoints(2, -1,-1, 0,0);
				pts_mix_light.setPoints(2, -2,0, -1,0);
				break;
			case QStyle::PE_ArrowDown:
				pts_dark.setPoints(3, -4,-2, -1,1, 1,-1);
				pts_mix_dark.setPoints(2, -3,-2, 2,-2);
				pts_mix_mid.setPoints(2, -1,0, 0,-1);
				pts_mix_light.setPoints(2, -2,-1, -1,-1);
				break;
			case QStyle::PE_ArrowRight:
				pts_dark.setPoints(3, -2,-4, 1,-1, -1,1);
				pts_mix_dark.setPoints(2, -2,-3, -2,2);
				pts_mix_mid.setPoints(2, 0,-1, -1,0);
				pts_mix_light.setPoints(2, -1,-2, -1,-1);
				break;
			case QStyle::PE_ArrowLeft:
				pts_dark.setPoints(3, 1,-4, -2,-1, 0,1);
				pts_mix_dark.setPoints(2, 1,-3, 1,2);
				pts_mix_mid.setPoints(2, -1,-1, 0,0);
				pts_mix_light.setPoints(2, 0,-2, 0,-1);
				break;
			}
			p->save();
			p->translate( pixelMetric( PM_ButtonShiftHorizontal ), pixelMetric( PM_ButtonShiftVertical ) );
			p->translate( r.x() + r.width() / 2, r.y() + r.height() / 2 );
			p->setPen(color_dark); p->drawPolyline(pts_dark);
			p->setPen(color_mix_dark); p->drawPolyline(pts_mix_dark);
			p->setPen(color_mix_mid); p->drawPolyline(pts_mix_mid);
			p->setPen(color_mix_light); p->drawPolyline(pts_mix_light);
			p->restore();
		}
		break;
	case QStyle::PE_Panel:						// 枠　テキストボックス、リストボックス、リストビュー、スクロールビュー
		{
			if ((flags & QStyle::Style_Sunken)) {
				QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
			}
			else {
				QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
			}
		}
		break;
	case QStyle::PE_ButtonCommand:				// 通常のボタン枠
		{
			int m = 1;	// 外周マージン
			int x = r.x() + m;
			int y = r.y() + m;
			int w = r.width() - 1 - m * 2;
			int h = r.height() - 1 - m * 2;
			h = h - 1;
			w = w - 1;
			if (!(flags & L_STYLE_FLAG_NO_FILL_BACKGROUND) && ((flags & Style_Raised) || (flags & QStyle::Style_On) || (flags & QStyle::Style_Down))) {
				// 枠外の背景色潰し（外周分）
				p->setPen(cg.background());
				for (int margin = 0 ; margin < m ; margin++) {
					p->drawRect(r.x() + margin, r.y() + margin, r.width() - 1 - margin * 2, r.height() - 1 - margin * 2);
				}
			}
			if ((flags & QStyle::Style_On) || (flags & QStyle::Style_Down)) {
				// トグルがONのとき、Raisedかどうかのチェックは不要

				// 通常時と凹み時、色のバランスのせいか1pxずれて見えるのでその補正をかける
				x = x + 1;
				y = y + 1;
				{
					// 枠外の背景色潰し（オフセット分）
					p->setPen(cg.background());
					QPointArray pts(3);
					pts.setPoints(3, x - 1, y + h, x - 1, y - 1, x + w, y - 1);
					p->drawPolyline(pts);
					pts.setPoints(3, x - 2, y + h + 1, x + w + 1, y + h + 1, x + w + 1, y - 2);
					p->drawPolyline(pts);
				}
				this->drawButton(p, cg, x, y, w, h, (flags & QStyle::Style_Enabled) != 0, true);
			}
			else {
				if (!(flags & L_STYLE_FLAG_NO_FILL_BACKGROUND)) {
					// 枠外の背景色潰し（オフセット分）
					p->setPen(cg.background());
					QPointArray pts(3);
					pts.setPoints(3, x - 1, y + h + 2, x + w + 2, y + h + 2, x + w + 2, y - 1);
					p->drawPolyline(pts);
				}
				if ((flags & Style_Raised)) {
					this->drawButton(p, cg, x, y, w, h, (flags & QStyle::Style_Enabled) != 0, false);
				}
				else {
					// 有効/無効に関わらず枠線は不要
					QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
				}
			}
			if (!(flags & L_STYLE_FLAG_NO_FILL_BACKGROUND)) {
				if ((flags & Style_Raised) || (flags & QStyle::Style_On) || (flags & QStyle::Style_Down)) {
					// 枠外の背景色潰し（角丸め分）
						p->setPen(cg.background());
						p->drawPoint(x, y);
						p->drawPoint(x + w, y);
						p->drawPoint(x, y + h);
						p->drawPoint(x + w, y + h);
				}
				else {
					p->setPen(cg.background());
					for (int margin = 0 ; margin < 2 ; margin++) {
						p->drawRect(r.x() + margin, r.y() + margin, r.width() - margin * 2, r.height() - margin * 2);
					}
				}
			}
		}
		break;
	case QStyle::PE_PanelLineEdit:				// 枠　テキストボックス（標準だと中でQStyle::PE_Panelを呼んでる感じがする）
		if ((flags & QStyle::Style_Sunken)) {
			this->drawTextBox(p, cg, r.x(), r.y(), r.width(), r.height(), false);
		}
		else {
			QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
		}
		break;
	case QStyle::PE_GroupBoxFrame:	// グループボックス
		if ((flags & QStyle::Style_Sunken)) {
			int m = 1;
			int c = 2;
			int x = r.x() + m;
			int y = r.y() + m;
			int w = r.width() - 1 - m * 2;
			int h = r.height() - 1 - m * 2;
			p->setPen(cg.dark());
			{
				int idx = 0;
				QPointArray pts(9);
				pts.setPoint(idx++, x, y + c);
				pts.setPoint(idx++, x, y + h - c);
				pts.setPoint(idx++, x + c, y + h);
				pts.setPoint(idx++, x + w - c, y + h);
				pts.setPoint(idx++, x + w, y + h - c);
				pts.setPoint(idx++, x + w, y + c);
				pts.setPoint(idx++, x + w - c, y);
				pts.setPoint(idx++, x + c, y);
				pts.setPoint(idx++, x, y + c);
				p->drawPolyline(pts);
			}
			break;
		}
		else {
			QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
		}
		break;
	case QStyle::PE_Indicator:					// チェックボックス
		{
			int x = r.x() + 1;
			int y = r.y() + 1;
			int w = r.width() - 2;
			int h = r.height() - 2;
			QColor col_border = (flags & QStyle::Style_Enabled) ? cg.shadow() : cg.dark();
			{
				QColor col_wk = calcGradation(cg.base(), col_border, 12, 256);	// アンチエイリアス用中間色の計算
				p->setPen(col_wk);
				p->setBrush(cg.base());
				p->drawRect(x + 1, y + 1, w - 2, h - 2);
			}
			QPointArray pts(9);
			{
				QColor col_wk = calcGradation(cg.dark(), col_border, 48, 256);	// アンチエイリアス用中間色の計算
				p->setPen(col_wk);
				pts.setPoints(5
					, x, y
					, x + w - 1, y
					, x + w - 1, y + h - 1
					, x, y + h - 1
					, x, y
					);
				p->drawPolyline(pts, 0, 5);
			}
			{
				QColor col_wk = calcGradation(cg.background(), col_border, 6, 256);	// アンチエイリアス用中間色の計算
				p->setPen(col_wk);
				pts.setPoints(9
					, x, y - 1
					, x + w - 1, y - 1
					, x + w, y
					, x + w, y + h - 1
					, x + w - 1, y + h
					, x, y + h
					, x - 1, y + h - 1
					, x - 1, y + 1
					, x, y - 1
					);
				p->drawPolyline(pts);
			}

			if ((flags & QStyle::Style_On)) {
				// チェックマーク
				p->setPen(cg.text());
				p->setBrush(cg.text());
				QPointArray pts(7);
				int idx = 0;
				pts.setPoint(idx++, x + 2, y + 4);
				pts.setPoint(idx++, x + 4, y + 6);
				pts.setPoint(idx++, x + 8, y + 2);
				pts.setPoint(idx++, x + 8, y + 4);
				pts.setPoint(idx++, x + 4, y + 8);
				pts.setPoint(idx++, x + 2, y + 6);
				pts.setPoint(idx++, x + 2, y + 4);
				p->drawPolygon(pts);
			}
		}
		break;
	case QStyle::PE_ExclusiveIndicator:			// ラジオボタン
		{
			int x = r.x();
			int y = r.y();
			int w = r.width();
			int h = r.height();
			QColor col_border = (flags & QStyle::Style_Enabled) ? cg.shadow() : cg.dark();
			{
				QColor col_wk = calcGradation(cg.dark(), col_border, 48, 256);	// アンチエイリアス用中間色の計算
				p->setPen(col_wk);
				p->setBrush(cg.base());
				p->drawRoundRect(x, y, w, h, 99, 99);	// drawEllipseだとガチャガチャした円になるのでこっちにした
				col_wk = calcGradation(cg.base(), col_border, 32, 256);	// アンチエイリアス用中間色の計算
				p->setPen(col_wk);
				p->drawRoundRect(x + 1, y + 1, w - 2, h - 2, 99, 99);	// drawEllipseだとガチャガチャした円になるのでこっちにした
			}

			if ((flags & QStyle::Style_On)) {
				// ●マーク
				p->setPen(cg.text());
				p->setBrush(cg.text());
				QPointArray pts(9);
				int idx = 0;
				int wc = w / 2;
				int hc = h / 2;
				pts.setPoint(idx++, x + wc - 2, y + hc - 1);
				pts.setPoint(idx++, x + wc - 1, y + hc - 2);
				pts.setPoint(idx++, x + wc - 0, y + hc - 2);
				pts.setPoint(idx++, x + wc + 1, y + hc - 1);
				pts.setPoint(idx++, x + wc + 1, y + hc + 0);
				pts.setPoint(idx++, x + wc + 0, y + hc + 1);
				pts.setPoint(idx++, x + wc - 1, y + hc + 1);
				pts.setPoint(idx++, x + wc - 2, y + hc + 0);
				pts.setPoint(idx++, x + wc - 2, y + hc - 1);
				p->drawPolygon(pts);
				// 自前で適当に計算してアンチエイリアスをかける
				// ガチでやるならピクセルにかかる円の面積求めて、
				// そこから色を計算するんだけどさすがにそれは面倒なので。
				{
					int r, g, b;
					r = (cg.text().red  () * 1 + cg.base().red  () * 7) / 8;
					g = (cg.text().green() * 1 + cg.base().green() * 7) / 8;
					b = (cg.text().blue () * 1 + cg.base().blue () * 7) / 8;
					p->setPen(QColor(r,g,b));
					p->drawPoint(x + wc - 1, y + hc - 3);
					p->drawPoint(x + wc - 0, y + hc - 3);
					p->drawPoint(x + wc - 1, y + hc + 2);
					p->drawPoint(x + wc - 0, y + hc + 2);
					p->drawPoint(x + wc - 3, y + hc - 1);
					p->drawPoint(x + wc - 3, y + hc - 0);
					p->drawPoint(x + wc + 2, y + hc - 1);
					p->drawPoint(x + wc + 2, y + hc - 0);
					// アンチエイジングじゃないよ
					r = (cg.text().red  () * 5 + cg.base().red  () * 11) / 16;
					g = (cg.text().green() * 5 + cg.base().green() * 11) / 16;
					b = (cg.text().blue () * 5 + cg.base().blue () * 11) / 16;
					p->setPen(QColor(r,g,b));
					p->drawPoint(x + wc - 2, y + hc - 2);
					p->drawPoint(x + wc - 2, y + hc + 1);
					p->drawPoint(x + wc + 1, y + hc - 2);
					p->drawPoint(x + wc + 1, y + hc + 1);
				}
			}
		}
		break;
	case QStyle::PE_DockWindowHandle:				// ドッキングウィンドウおよびツールバーのつまみ（分離するときに掴むやつ）
		{
			bool is_gradation_horizontal = true;
			int x_base;
			int y_base;
			int dx;
			int dy;
			int dsize = 4;
			int num = 0;
			QRect re(r.x(), r.y(), r.width(), r.height());
			if ((flags & QStyle::Style_Horizontal)) {
				x_base = r.x() + r.width() / 2 - 1;
				y_base = r.y() + 1 + 2;
				dx = 0;
				dy = dsize;
				num = (r.height() - dsize) / dsize;
				if ((r.height() % dsize) > 1) y_base++;
			} else {
				is_gradation_horizontal = false;
				x_base = r.x() + 1 + 2;
				y_base = r.y() + r.height() / 2 - 1;
				dx = dsize;
				dy = 0;
				num = (r.width() - dsize) / dsize;
				if ((r.width() % dsize) > 1) x_base++;
			}
			// ツールバーと、ドッキングウィンドウでグラデーションの付け方を変える
			// 見分け付けられるようなフラグがなかったのでこれで…
			if (r.width() > 10 && r.height() > 10) {
				is_gradation_horizontal = !is_gradation_horizontal;
			}

			this->drawToolbarGradation(p, cg, re, is_gradation_horizontal);

			p->setPen(cg.shadow());
			for (int idx = 0 ; idx < num ; idx++) {
				int x = x_base + dx * idx;
				int y = y_base + dy * idx;
				p->drawRect(x, y, 2, 2);
			}
			p->setPen(cg.light());
			QPointArray pts(3);
			for (int idx = 0 ; idx < num ; idx++) {
				int x = x_base + dx * idx;
				int y = y_base + dy * idx;
				pts.setPoints(3, x + 1, y + 2, x + 2, y + 2, x + 2, y + 1);
				p->drawPolyline(pts);
			}
		}
		break;
	case QStyle::PE_DockWindowSeparator:			// ツールバーのセパレータ
		{
			p->setPen(cg.dark());
			int m = 2;
			if ((flags & QStyle::Style_Horizontal)) {
				this->drawToolbarGradation(p, cg, r);
				p->drawLine(r.x() + r.width() / 2, r.y() + m, r.x() + r.width() / 2, r.y() + r.height() - 1 - m);
			} else {
				this->drawToolbarGradation(p, cg, r, false);
				p->drawLine(r.x() + m, r.y() + r.height() / 2, r.x() + r.width() - 1 - m, r.y() + r.height() / 2);
			}
		}
		break;
	case QStyle::PE_ButtonTool:					// ツールバーボタンの凹みおよびマウスオーバー、MDI子ウィンドウおよびドッキングウィンドウの×ボタン
		{
			QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
		}
		break;
	case QStyle::PE_SpinWidgetUp:					// スピンボックス▲
		{
			int x = r.x();
			int y = r.y() + 1;
			int w = r.width() - 2;
			int h = r.height() - 2;
			int r_wk = (cg.base().red  () + cg.dark().red  ()) / 2;
			int g_wk = (cg.base().green() + cg.dark().green()) / 2;
			int b_wk = (cg.base().blue () + cg.dark().blue ()) / 2;
			QColor col_e(r_wk, g_wk, b_wk);
			drawGradation(p, x, y, w, h, cg.base(), col_e);
			p->setPen(cg.foreground());
			p->setBrush(cg.foreground());
			QPointArray pts(4);
			int x_base = x + w / 2;
			int y_base = y + 1;
			pts.setPoints(4, x_base, y_base, x_base + 2, y_base + 2, x_base - 2, y_base + 2, x_base, y_base);
			p->drawPolygon(pts);
		}
		break;
	case QStyle::PE_SpinWidgetDown:				// スピンボックス▼
		{
			int x = r.x();
			int y = r.y();
			int w = r.width() - 2;
			int h = r.height() - 1;
			int r_wk = (cg.base().red  () + cg.dark().red  ()) / 2;
			int g_wk = (cg.base().green() + cg.dark().green()) / 2;
			int b_wk = (cg.base().blue () + cg.dark().blue ()) / 2;
			QColor col_e(r_wk, g_wk, b_wk);
			drawGradation(p, x, y, w, h, cg.base(), col_e);
			p->setPen(cg.foreground());
			p->setBrush(cg.foreground());
			QPointArray pts(4);
			int x_base = x + w / 2;
			int y_base = y + h - 3;
			pts.setPoints(4, x_base, y_base, x_base + 2, y_base - 2, x_base - 2, y_base - 2, x_base, y_base);
			p->drawPolygon(pts);
		}
		break;
	case QStyle::PE_ScrollBarAddLine:				// スクロールバー▼>領域
		// fall through
	case QStyle::PE_ScrollBarSubLine:				// スクロールバー▲<領域
		{
			this->drawButton(p, cg, r.x(), r.y(), r.width() - 1, r.height() - 1
					, (flags & QStyle::Style_Enabled) != 0, (flags & QStyle::Style_Down) != 0
					, (flags & QStyle::Style_Horizontal) ? MyWinStyle::GradationTop2Bottom : MyWinStyle::GradationLeft2Right);
			QStyle::PrimitiveElement pe_wk;
			if (pe == QStyle::PE_ScrollBarAddLine) {
				pe_wk = (flags & QStyle::Style_Horizontal) ? QStyle::PE_ArrowRight : QStyle::PE_ArrowDown;
			}
			else {
				pe_wk = (flags & QStyle::Style_Horizontal) ? QStyle::PE_ArrowLeft : QStyle::PE_ArrowUp;
			}
			{
				// 枠外の背景色潰し（角丸め分）
					p->setPen(cg.background());
					p->drawPoint(r.x(), r.y());
					p->drawPoint(r.x() + r.width() - 1, r.y());
					p->drawPoint(r.x(), r.y() + r.height() - 1);
					p->drawPoint(r.x() + r.width() - 1, r.y() + r.height() - 1);
			}
			this->drawPrimitive(pe_wk, p, r, cg, flags, opt);
		}
		break;
	case QStyle::PE_ScrollBarSlider:				// スクロールバーのスライダ
		{
			this->drawButton(p, cg, r.x(), r.y(), r.width() - 1, r.height() - 1
					, (flags & QStyle::Style_Enabled) != 0, false
					, (flags & QStyle::Style_Horizontal) ? MyWinStyle::GradationTop2Bottom : MyWinStyle::GradationLeft2Right);
			{
				// 枠外の背景色潰し（角丸め分）
					p->setPen(cg.background());
					p->drawPoint(r.x(), r.y());
					p->drawPoint(r.x() + r.width() - 1, r.y());
					p->drawPoint(r.x(), r.y() + r.height() - 1);
					p->drawPoint(r.x() + r.width() - 1, r.y() + r.height() - 1);
			}
		}
		break;
	case QStyle::PE_ScrollBarAddPage:				// スクロールバーのスライダ下側領域（クリックするとページ送りになる領域）
		// fall through
	case QStyle::PE_ScrollBarSubPage:				// スクロールバーのスライダ上側領域（クリックするとページ送りになる領域）
		{
			QColor col((flags & QStyle::Style_Down) ? cg.shadow() : cg.light());
			int r_bg = (cg.background().red  () + col.red  ()) / 2;
			int g_bg = (cg.background().green() + col.green()) / 2;
			int b_bg = (cg.background().blue () + col.blue ()) / 2;
			p->fillRect(r, QBrush(QColor(r_bg, g_bg, b_bg)));
		}
		break;
	default:
		QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
		break;
	}
}

void MyWinStyleNew::drawControl(ControlElement element, QPainter* p, const QWidget* widget
	, const QRect& r, const QColorGroup& cg, SFlags how, const QStyleOption& opt) const
{
	switch (element) {
	case QStyle::CE_TabBarTab:	// タブの耳
		{
			const QTabBar* tb = (const QTabBar*)widget;
			switch (tb->shape()) {
			case QTabBar::RoundedAbove:
			case QTabBar::RoundedBelow:
				{
					// 光と影のグラデーションかける
					int x = r.x() + 1;
					int y = r.y() + 1;
					int w = r.width() - 2;
					int h = r.height() - 2;

					if ((how & QStyle::Style_Selected)) {
						int height = 8;
						if (tb->shape() == QTabBar::RoundedAbove) {
							drawGradation(p, x, y, w, height, cg.light(), cg.background());
						} else {
							drawGradation(p, x, y + h - height, w, height, cg.background(), cg.light());
						}
					} else {
						int height = 8;
						x += 2;
						y += 2;
						w -= 4;
						h -= 4;
						if (tb->shape() == QTabBar::RoundedAbove) {
							drawGradation(p, x, y + h - height + 1, w, height, cg.background(), cg.dark());
						} else {
							drawGradation(p, x, y, w, height, cg.dark(), cg.background());
						}
					}
				}
				// 枠線は元のをそのまま使う
				break;
			default:
				break;
			}
			QWindowsStyle::drawControl(element, p, widget, r, cg, how, opt);
		}
		break;
	case QStyle::CE_PushButton:
		{
			const QPushButton *button = (const QPushButton *) widget;
			const QWidget* parent_widget = button->parentWidget();
			if (parent_widget && parent_widget->inherits("QToolBar")) {
				this->drawToolbarGradation(p, cg, r);
				if ((how & Style_Raised) || (how & QStyle::Style_On) || (how & QStyle::Style_Down)) {
					drawPrimitive(PE_ButtonCommand, p, r, cg, (SFlags)(how | L_STYLE_FLAG_NO_FILL_BACKGROUND));
				}
			} else {
				QWindowsStyle::drawControl(element, p, widget, r, cg, how, opt);
			}
		}
		break;
	default:
		QWindowsStyle::drawControl(element, p, widget, r, cg, how, opt);
		break;
	}
}

void MyWinStyleNew::drawControlMask(ControlElement element, QPainter* p, const QWidget* widget
	, const QRect& r, const QStyleOption& opt) const
{
	QWindowsStyle::drawControlMask(element, p, widget, r, opt);
}

QRect MyWinStyleNew::subRect(QStyle::SubRect subrect, const QWidget* widget) const
{
	return QWindowsStyle::subRect(subrect, widget);
}

void MyWinStyleNew::drawComplexControl(ComplexControl control, QPainter* p
	, const QWidget* widget, const QRect& r, const QColorGroup& cg
	, SFlags how, SCFlags sub, SCFlags subActive, const QStyleOption& opt) const
{
	switch(control) {
	case QStyle::CC_SpinWidget:	// スピンボックス
		{
			QPen pen = p->pen();
			QBrush brush = p->brush();
			{
				const QSpinWidget * sw = (const QSpinWidget *) widget;
				this->drawTextBox(p, cg, r.x(), r.y(), r.width(), r.height());
				if ( sub & SC_SpinWidgetUp ) {
					PrimitiveElement pe = QStyle::PE_SpinWidgetUp;
					SFlags flags = Style_Default | Style_Enabled;
					if (subActive == SC_SpinWidgetUp ) {
						flags |= Style_On;
						flags |= Style_Sunken;
					}
					else {
						flags |= Style_Raised;
					}
					QRect re = sw->upRect();
					QColorGroup cg_wk = sw->isUpEnabled() ? cg : sw->palette().disabled();
					if (sw->isEnabled() && !sw->isUpEnabled()) {
						cg_wk.setColor(QColorGroup::Base, cg.base());
						cg_wk.setColor(QColorGroup::Foreground, cg.mid());
					}
					drawPrimitive(pe, p, re, cg_wk, flags);
				}
				if ( sub & SC_SpinWidgetDown ) {
					PrimitiveElement pe = QStyle::PE_SpinWidgetDown;
					SFlags flags = Style_Default | Style_Enabled;
					if (subActive == SC_SpinWidgetDown ) {
						flags |= Style_On;
						flags |= Style_Sunken;
					}
					else {
						flags |= Style_Raised;
					}
					QRect re = sw->downRect();
					QColorGroup cg_wk = sw->isDownEnabled() ? cg : sw->palette().disabled();
					if (sw->isEnabled() && !sw->isDownEnabled()) {
						cg_wk.setColor(QColorGroup::Base, cg.base());
						cg_wk.setColor(QColorGroup::Foreground, cg.mid());
					}
					drawPrimitive(pe, p, re, cg_wk, flags);
				}
			}
			p->setBrush(brush);
			p->setPen(pen);
			//QWindowsStyle::drawComplexControl(control, p, widget, r, cg, how, sub, subActive, opt);
		}
		break;
	case QStyle::CC_ComboBox:	// コンボボックス
		{
			QPen pen = p->pen();
			QBrush brush = p->brush();
			{
				const QComboBox * cb = (const QComboBox *)widget;
				this->drawTextBox(p, cg, r.x(), r.y(), r.width(), r.height());
				{
					QRect ar = QStyle::visualRect(querySubControlMetrics( CC_ComboBox, widget
													, SC_ComboBoxArrow ), widget);
					{
						QColor col_s(cg.base());
						int r_wk = (cg.dark().red  () + cg.base().red  ()) / 2;
						int g_wk = (cg.dark().green() + cg.base().green()) / 2;
						int b_wk = (cg.dark().blue () + cg.base().blue ()) / 2;
						QColor col_e(QColor(r_wk, g_wk, b_wk));
						int height = 8;
						drawGradation(p, ar.x(), ar.y() + ar.height() - height - 1, ar.width() - 2, height, col_s, col_e);
					}
					ar.addCoords(2, 2, -2, -2);
					SFlags flags = Style_Default;
					if (widget->isEnabled()) flags |= Style_Enabled;
					if (subActive == SC_ComboBoxArrow) flags |= Style_Sunken;
					drawPrimitive(PE_ArrowDown, p, ar, cg, flags);

					if (cb->hasFocus() && !cb->editable()) {
						p->setPen(cg.highlightedText());
						p->setBackgroundColor(cg.highlight());
						QRect re = QStyle::visualRect(subRect(SR_ComboBoxFocusRect, cb), widget);
						this->drawPrimitive(PE_FocusRect, p, re, cg, Style_FocusAtBorder, QStyleOption(cg.highlight()));
					}
				}
			}
			p->setBrush(brush);
			p->setPen(pen);
		}
		break;
	case QStyle::CC_ToolButton:	// ツールバーのボタン
		{
			QWidget* parent = widget->parentWidget();
			if (parent && parent->inherits("QToolBar")) {
				QToolBar* toolbar = (QToolBar*)parent;
				this->drawToolbarGradation(p, cg, r, toolbar->orientation() == Qt::Horizontal);

				if (!(how & QStyle::Style_Raised)) {
					if ((how & QStyle::Style_Down) || (how & QStyle::Style_On)) {
						int r_bg = cg.button().red  () + (cg.light().red  () - cg.button().red  ()) * 3 / 4;
						int g_bg = cg.button().green() + (cg.light().green() - cg.button().green()) * 3 / 4;
						int b_bg = cg.button().blue () + (cg.light().blue () - cg.button().blue ()) * 3 / 4;
						int r_frame = (cg.dark().red  () + cg.shadow().red  ()) / 2;
						int g_frame = (cg.dark().green() + cg.shadow().green()) / 2;
						int b_frame = (cg.dark().blue () + cg.shadow().blue ()) / 2;
						p->setPen(QColor(r_frame, g_frame, b_frame));
						p->setBrush(QColor(r_bg, g_bg, b_bg));
						p->drawRect(r);
					}
				}
				else {
					QWindowsStyle::drawComplexControl(control, p, widget, r, cg, how, sub, subActive, opt);
				}
			}
			else {
				QWindowsStyle::drawComplexControl(control, p, widget, r, cg, how, sub, subActive, opt);
			}
			if (sub & SC_ToolButtonMenu) {
				QRect button, menuarea;
				menuarea = visualRect( querySubControlMetrics(control, widget, SC_ToolButtonMenu, opt), widget );
				SFlags mflags = how;
				if (subActive & SC_ToolButtonMenu) mflags |= Style_Down;
				if (mflags & (Style_Down | Style_On | Style_Raised))
					drawPrimitive(PE_ButtonDropDown, p, menuarea, cg, mflags, opt);
				drawPrimitive(PE_ArrowDown, p, menuarea, cg, mflags, opt);
			}
		}
		break;
	case QStyle::CC_Slider:	// スライダ
		{
			const QSlider *sl = (const QSlider *) widget;
			SCFlags sub_wk = (SCFlags)(sub & (~QStyle::SC_SliderHandle));	// ハンドルは除外して、それ以外は以前のスタイルで描く
			QWindowsStyle::drawComplexControl(control, p, widget, r, cg, how, sub_wk, subActive, opt);
			if (sub & SC_SliderHandle) {
				QRect rect = querySubControlMetrics(QStyle::CC_Slider, widget, QStyle::SC_SliderHandle, opt);

				// これから描く領域をクリア（塗りつぶし）
				p->fillRect(rect, cg.brush(QColorGroup::Background));

				// フォーカス枠
				if ( how & Style_HasFocus ) {
					QRect re = subRect( SR_SliderFocusRect, sl );
					drawPrimitive( PE_FocusRect, p, re, cg );
				}

				QStyle::Orientation ori = sl->orientation();
				bool is_tick_above = sl->tickmarks() == QSlider::Above;
				bool is_tick_below = sl->tickmarks() == QSlider::Below;

				// ハンドルの描画
				int x = rect.x();
				int y = rect.y();
				int w = rect.width();
				int h = rect.height();
				QColor col_edge = cg.dark();
				QColor col_shadow = cg.shadow();
				QColor col_light = cg.light();
				QRegion region;
				p->save();
				if ((is_tick_above && !is_tick_below) || (!is_tick_above && is_tick_below)) {
					// 目盛り差し用の尖った形状（<], [>）で描く
					QPointArray pts(8);
					if (ori == QStyle::Horizontal) {
						if (is_tick_above) {
							// ↑
							{
								pts.setPoints(8
									, x + w / 2, y
									, x + (w-1), y + w / 2
									, x + (w-1), y + (h-1) - 1
									, x + (w-1) - 1, y + (h-1)
									, x + 1, y + (h-1)
									, x, y + (h-1) - 1
									, x, y + w / 2
									, x + w / 2, y
									);
								p->setPen(col_edge);
								p->drawPolyline(pts);
							}
							{
								pts.setPoints(4
									, x + w / 2, y + 1
									, x + (w-1) - 1, y + w / 2
									, x + (w-1) - 1, y + (h-1) - 1
									, x + 1, y + (h-1) - 1
									);
								p->setPen(col_shadow);
								p->drawPolyline(pts, 0, 4);
							}
							{
								pts.setPoints(3
									, x + w / 2 - 1, y + 2
									, x + 1, y + w / 2
									, x + 1, y + (h-1) - 2
									);
								p->setPen(col_light);
								p->drawPolyline(pts, 0, 3);
							}

							// グラデーション用のクリッピング
							pts.setPoints(6
								, x + w / 2, y + 2
								, x + (w-1) - 1, y + w / 2
								, x + (w-1) - 1, y + (h-1) - 1
								, x + 2, y + (h-1) - 1
								, x + 2, y + w / 2
								, x + w / 2, y + 2
								);
							region = QRegion(pts);
						}
						else {	// is_tick_below
							// ↓
							{
								pts.setPoints(8
									, x + w / 2, y + (h-1)
									, x, y + (h-1) - w / 2
									, x, y + 1
									, x + 1, y
									, x + (w-1) - 1, y
									, x + (w-1), y + 1
									, x + (w-1), y + (h-1) - w / 2
									, x + w / 2, y + (h-1)
									);
								p->setPen(col_edge);
								p->drawPolyline(pts);
							}
							{
								pts.setPoints(3
									, x + w / 2, y + (h-1) - 1
									, x + (w-1) - 1, y + (h-1) - w / 2
									, x + (w-1) - 1, y + 1
									);
								p->setPen(col_shadow);
								p->drawPolyline(pts, 0, 3);
							}
							{
								pts.setPoints(4
									, x + w / 2 - 1, y + (h-1) - 2
									, x + 1, y + (h-1) - w / 2
									, x + 1, y + 1
									, x + (w-1) - 2, y + 1
									);
								p->setPen(col_light);
								p->drawPolyline(pts, 0, 4);
							}

							// グラデーション用のクリッピング
							pts.setPoints(6
								, x + w / 2, y + (h-1) - 1
								, x + (w-1) - 1, y + (h-1) - w /2
								, x + (w-1) - 1, y + 2
								, x + 2, y + 2
								, x + 2, y + (h-1) - w /2
								, x + w / 2, y + (h-1) - 1
								);
							region = QRegion(pts);
						}
					}
					else {
						if (is_tick_above) {
							// <]
							{
								pts.setPoints(8
									, x, y + h / 2
									, x + h / 2, y
									, x + (w-1) - 1, y
									, x + (w-1), y + 1
									, x + (w-1), y + (h-1) - 1
									, x + (w-1) - 1, y + (h-1)
									, x + h / 2, y + (h-1)
									, x, y + (h/2)
									);
								p->setPen(col_edge);
								p->drawPolyline(pts);
							}
							{
								pts.setPoints(4
									, x + 1, y + h / 2
									, x + h / 2, y + (h-1) - 1
									, x + (w-1) - 1, y + (h-1) - 1
									, x + (w-1) - 1, y + 1
									);
								p->setPen(col_shadow);
								p->drawPolyline(pts, 0, 4);
							}
							{
								pts.setPoints(3
									, x + 2, y + h / 2 - 1
									, x + h / 2, y + 1
									, x + (w-1) - 2, y + 1
									);
								p->setPen(col_light);
								p->drawPolyline(pts, 0, 3);
							}

							// グラデーション用のクリッピング
							pts.setPoints(6
								, x + 2, y + h / 2
								, x + h / 2, y + 2
								, x + (w-1) - 1, y + 2
								, x + (w-1) - 1, y + (h-1) - 1
								, x + h / 2, y + (h-1) - 1
								, x + 2, y + h / 2
								);
							region = QRegion(pts);
						}
						else {	// is_tick_below
							// [>
							{
								pts.setPoints(8
									, x + (w-1), y + h / 2
									, x + (w-1) - h / 2, y + (h-1)
									, x + 1, y + (h-1)
									, x, y + (h-1) - 1
									, x, y + 1
									, x + 1, y
									, x + (w-1) - h / 2, y
									, x + (w-1), y + h / 2
									);
								p->setPen(col_edge);
								p->drawPolyline(pts);
							}
							{
								pts.setPoints(3
									, x + 1, y + (h-1) - 1
									, x + (w-1) - h / 2, y + (h-1) - 1
									, x + (w-1) - 1, y + h / 2
									);
								p->setPen(col_shadow);
								p->drawPolyline(pts, 0, 3);
							}
							{
								pts.setPoints(4
									, x + 1, y + (h-1) - 2
									, x + 1, y + 1
									, x + (w-1) - h / 2, y + 1
									, x + (w-1) - 2, y + h / 2 - 1
									);
								p->setPen(col_light);
								p->drawPolyline(pts, 0, 4);
							}

							// グラデーション用のクリッピング
							pts.setPoints(6
								, x + (w-1) - 1, y + h / 2
								, x + (w-1) - h / 2, y + 2
								, x + 2, y + 2
								, x + 2, y + (h-1) - 1
								, x + (w-1) - h / 2, y + (h-1) - 1
								, x + (w-1) - 1, y + h / 2
								);
							region = QRegion(pts);
						}
					}
				}
				else {
					// 四角いバー（[]）で描く
					QPointArray pts(9);
					{
						pts.setPoints(9
							, x + 1, y
							, x + (w-1) - 1, y
							, x + (w-1), y + 1
							, x + (w-1), y + (h-1) - 1
							, x + (w-1) - 1, y + (h-1)
							, x + 1, y + (h-1)
							, x, y + (h-1) - 1
							, x, y + 1
							, x + 1, y
							);
						p->setPen(col_edge);
						p->drawPolyline(pts);
					}
					{
						pts.setPoints(3
							, x + 1, y + (h-1) - 1
							, x + (w-1) - 1, y + (h-1) - 1
							, x + (w-1) - 1, y + 1
							);
						p->setPen(col_shadow);
						p->drawPolyline(pts, 0, 3);
					}
					{
						pts.setPoints(3
							, x + 1, y + (h-1) - 2
							, x + 1, y + 1
							, x + (w-1) - 2, y + 1
							);
						p->setPen(col_light);
						p->drawPolyline(pts, 0, 3);
					}
					region = QRegion(QRect(x + 2, y + 2, w - 2 * 2, h - 2 * 2));
				}

				// グラデーション
				if (!region.isEmpty()) {
					p->setClipping(true);
					p->setClipRegion(region);
					int size = 8;
					if ((h - 2 * 2) < size) size = h - 2 * 2;
					QColor col_s = cg.background();
					QColor col_e = calcGradation(col_s, cg.dark(), size, 8);
					drawGradation(p, x + 1, y + (h - size), w - 1 * 2, size, col_s, col_e);
				}
				p->restore();
			}
		}
		break;
	default:
		QWindowsStyle::drawComplexControl(control, p, widget, r, cg, how, sub, subActive, opt);
		break;
	}
}

void MyWinStyleNew::drawComplexControlMask(ComplexControl control, QPainter* p
	, const QWidget* widget, const QRect& r, const QStyleOption& opt) const
{
	QWindowsStyle::drawComplexControlMask(control, p, widget, r, opt);
}

QRect MyWinStyleNew::querySubControlMetrics(ComplexControl control, const QWidget* widget
	, QStyle::SubControl subcontrol, const QStyleOption& opt) const
{
	return QWindowsStyle::querySubControlMetrics(control, widget, subcontrol, opt);
}

QStyle::SubControl MyWinStyleNew::querySubControl(ComplexControl control, const QWidget* widget
	, const QPoint& pos, const QStyleOption& opt) const
{
	return QWindowsStyle::querySubControl(control, widget, pos, opt);
}

int MyWinStyleNew::pixelMetric(PixelMetric metric, const QWidget* widget) const
{
	return QWindowsStyle::pixelMetric(metric, widget);
}

QSize MyWinStyleNew::sizeFromContents(ContentsType contents, const QWidget* widget
	, const QSize& contentsSize, const QStyleOption& opt) const
{
	return QWindowsStyle::sizeFromContents(contents, widget, contentsSize, opt);
}

int MyWinStyleNew::styleHint(StyleHint stylehint, const QWidget* widget
	, const QStyleOption& opt, QStyleHintReturn* returnData) const
{
	return QWindowsStyle::styleHint(stylehint, widget, opt, returnData);
}

void MyWinStyleNew::drawToolbarGradation(QPainter* p, const QColorGroup& cg, const QRect& r, bool isHorizontal) const
{
	// ツールバー用のグラデーションをかける
	// 対象 = ツールバーのボタン、ツールバーのセパレータ、ツールバーのハンドル
	QPen pen_org = p->pen();
	{
		QColor col(this->mColorGroupActive ? this->mColorGroupActive->light() : cg.light());
		int r_wk = (col.red() + cg.button().red()) / 2;
		int g_wk = (col.green() + cg.button().green()) / 2;
		int b_wk = (col.blue() + cg.button().blue()) / 2;
		QColor col_s(r_wk, g_wk, b_wk);
		QColor col_e(cg.button());
		int len = 8;
		if (isHorizontal) {
			drawGradation(p, r.x(), r.y(), r.width(), len, col_s, col_e);
		}
		else {
			drawGradation(p, r.x(), r.y(), len, r.height(), col_s, col_e, MyWinStyle::GradationLeft2Right);
		}
	}
	{
		QColor col(this->mColorGroupActive ? this->mColorGroupActive->dark() : cg.dark());
		int r_wk = (col.red() + cg.button().red()) / 2;
		int g_wk = (col.green() + cg.button().green()) / 2;
		int b_wk = (col.blue() + cg.button().blue()) / 2;
		QColor col_s(cg.button());
		QColor col_e(r_wk, g_wk, b_wk);
		int len = 8;
		if (isHorizontal) {
			drawGradation(p, r.x(), r.y() + r.height() - len, r.width(), len, col_s, col_e);
		}
		else {
			drawGradation(p, r.x() + r.width() - len, r.y(), len, r.height(), col_s, col_e, MyWinStyle::GradationLeft2Right);
		}
	}
	p->setPen(pen_org);
}

void MyWinStyleNew::drawTextBox(QPainter* p, const QColorGroup& cg, int pX, int pY, int pW, int pH, bool isFill) const
{
	// テキストボックス系の描画
	// 対象 = テキストボックス、コンボボックス、スピンボックス
	int m = 1;	// 外周マージン
	int c = 1;	// 面取り量
	int x = pX + m - 1;
	int y = pY + m;
	int w = pW - 1 - m * 2 + 1;
	int h = pH - 1 - m * 2;
	p->setPen(cg.dark());
	if (isFill) p->setBrush(cg.base()); {
		int idx = 0;
		QPointArray pts(9);
		pts.setPoint(idx++, x, y + c);
		pts.setPoint(idx++, x, y + h - c);
		pts.setPoint(idx++, x + c, y + h);
		pts.setPoint(idx++, x + w - c, y + h);
		pts.setPoint(idx++, x + w, y + h - c);
		pts.setPoint(idx++, x + w, y + c);
		pts.setPoint(idx++, x + w - c, y);
		pts.setPoint(idx++, x + c, y);
		pts.setPoint(idx++, x, y + c);
		if (isFill) p->drawPolygon(pts);
		else p->drawPolyline(pts);
	}
	{
		p->setPen(cg.background());
		for (int margin = 1 ; margin <= m ; margin++) {
			int idx = 0;
			QPointArray pts(5);
			pts.setPoint(idx++, x - margin, y - margin);
			pts.setPoint(idx++, x - margin, y + h + margin);
			pts.setPoint(idx++, x + w + margin, y + h + margin);
			pts.setPoint(idx++, x + w + margin, y - margin);
			pts.setPoint(idx++, x - margin, y - margin);
			p->drawPolyline(pts);
		}
	}
	{
		// 本当は面取りのサイズ分で潰していくべきだけど、
		// 面取りが機能していない（QLineEdit::drawContentで潰される）のでこれでいいや
		p->setPen(cg.background());
		p->drawPoint(x, y);
		p->drawPoint(x, y + h);
		p->drawPoint(x + w, y + h);
		p->drawPoint(x + w, y);
	}
}

void MyWinStyleNew::drawButton(QPainter *p, const QColorGroup &cg, int x, int y, int w, int h,
							bool isEnabled, bool isOn, MyWinStyle::GradationDirection direction) const
{
	// ボタン系の描画
	// 対象 = プッシュボタン、スクロールバーのスライダ
	if (isOn) {
		{
			// 背景色
			int r_bg = (cg.button().red() + cg.light().red()) / 2;
			int g_bg = (cg.button().green() + cg.light().green()) / 2;
			int b_bg = (cg.button().blue() + cg.light().blue()) / 2;

			int r_wk = (cg.dark().red() + r_bg) / 2;
			int g_wk = (cg.dark().green() + g_bg) / 2;
			int b_wk = (cg.dark().blue() + b_bg) / 2;
			QColor col_s(r_wk, g_wk, b_wk);	// 開始色
			QColor col_e(r_bg, g_bg, b_bg);	// 終了色
			int size = 8;
			if (direction == MyWinStyle::GradationTop2Bottom) {
				if (size > (h-2)/2) {
					size = (h-2)/2 - 1;
					QColor col = calcGradation(col_s, col_e, (8-1-size), 8);
					col_s = col;
				}
				p->fillRect(x + 2, y + 2 + size, w - 2, h - 2 - size + 1, QBrush(QColor(r_bg, g_bg, b_bg)));
				drawGradation(p, x + 2, y + 2, w - 3, size, col_s, col_e, direction);
			}
			else {
				if (size > (w-2)/2) {
					size = (w-2)/2 - 1;
					QColor col = calcGradation(col_s, col_e, (8-1-size), 8);
					col_s = col;
				}
				p->fillRect(x + 2 + size, y + 2, w - 2 - size + 1, h - 2, QBrush(QColor(r_bg, g_bg, b_bg)));
				drawGradation(p, x + 2, y + 2, size, h - 3, col_s, col_e, direction);
			}
		}
		{
			// ┏
			int idx = 0;
			QPointArray pts(4);
			pts.setPoint(idx++, x, y + h - 2);
			pts.setPoint(idx++, x, y + 1);
			pts.setPoint(idx++, x + 1, y);
			pts.setPoint(idx++, x + w - 2, y);
			p->setPen(cg.shadow());
			p->drawPolyline(pts);
		}
		{
			// ┌
			int idx = 0;
			QPointArray pts(3);
			pts.setPoint(idx++, x + 1, y + h - 2);
			pts.setPoint(idx++, x + 1, y + 1);
			pts.setPoint(idx++, x + w - 2, y + 1);
			p->setPen(isEnabled ? cg.dark() : cg.light());
			p->drawPolyline(pts);
		}
		{
			// ┘
			int idx = 0;
			QPointArray pts(6);
			pts.setPoint(idx++, x, y + h - 1);
			pts.setPoint(idx++, x + 1, y + h);
			pts.setPoint(idx++, x + w - 1, y + h);
			pts.setPoint(idx++, x + w, y + h - 1);
			pts.setPoint(idx++, x + w, y + 1);
			pts.setPoint(idx++, x + w - 1, y);
			p->setPen(cg.light());
			p->drawPolyline(pts);
		}
		{
			// 左下と右上のドット
			p->setPen(cg.mid());
			p->drawPoint(x + 1, y + h - 1);
			p->drawPoint(x + w - 1, y + 1);
		}
	}
	else {
		{
			int r_wk = (cg.light().red() + cg.button().red()) / 2;
			int g_wk = (cg.light().green() + cg.button().green()) / 2;
			int b_wk = (cg.light().blue() + cg.button().blue()) / 2;
			QColor col_light(r_wk, g_wk, b_wk);
			QColor col_bg(cg.button());
			QColor col_shade(cg.dark());
			int size = 8;
			if (direction == MyWinStyle::GradationTop2Bottom) {
				if (size > (h-2)/2) {
					size = (h-2)/2 - 1;
					QColor col;
					col = calcGradation(col_light, col_bg, (8 - 1 - size), 8);
					col_light = col;
					col = calcGradation(col_bg, col_shade, size, 8);
					col_shade = col;
				}
				p->fillRect(x + 1, y + 1, w - 2, h - 2 - size + 1, cg.brush(QColorGroup::Button));
				drawGradation(p, x + 1, y + 1, w - 3, size, col_light, col_bg, direction);
				drawGradation(p, x + 1, y + h - 1 - size, w - 3, size, col_bg, col_shade, direction);
			}
			else {
				if (size > (w-2)/2) {
					size = (w-2)/2 - 1;
					QColor col;
					col = calcGradation(col_light, col_bg, (8 - 1 - size), 8);
					col_light = col;
					col = calcGradation(col_bg, col_shade, size, 8);
					col_shade = col;
				}
				p->fillRect(x + 1, y + 1, w - 2 - size + 1, h - 2, cg.brush(QColorGroup::Button));
				drawGradation(p, x + 1, y + 1, size, h - 3, col_light, col_bg, direction);
				drawGradation(p, x + w - 1 - size, y + 1, size, h - 3, col_bg, col_shade, direction);
			}
		}
		{
			// ┛
			int idx = 0;
			QPointArray pts(4);
			pts.setPoint(idx++, x + 2, y + h);
			pts.setPoint(idx++, x + w - 1, y + h);
			pts.setPoint(idx++, x + w, y + h - 1);
			pts.setPoint(idx++, x + w, y + 2);
			p->setPen(cg.shadow());
			p->drawPolyline(pts);
		}
		{
			// ┘
			int idx = 0;
			QPointArray pts(3);
			pts.setPoint(idx++, x + 1, y + h - 1);
			pts.setPoint(idx++, x + w - 1, y + h - 1);
			pts.setPoint(idx++, x + w - 1, y + 1);
			p->setPen(isEnabled ? cg.dark() : cg.light());
			p->drawPolyline(pts);
		}
		{
			// ┌
			int idx = 0;
			QPointArray pts(6);
			pts.setPoint(idx++, x + 1, y + h);
			pts.setPoint(idx++, x, y + h - 1);
			pts.setPoint(idx++, x, y + 1);
			pts.setPoint(idx++, x + 1, y);
			pts.setPoint(idx++, x + w - 1, y);
			pts.setPoint(idx++, x + w, y + 1);
			p->setPen(cg.light());
			p->drawPolyline(pts);
		}
		{
			// 左下と右上のドット
			p->setPen(cg.mid());
			p->drawPoint(x + 1, y + h - 1);
			p->drawPoint(x + w - 1, y + 1);
		}
	}
}
