#include "maindef.h"
#include "windows.h"

#define L_MAPSIZE_ROW	30
#define L_MAPSIZE_COL	30
#define L_PIXSIZE	32

#if 1
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
#else

// QStyledItemDelegate を拡張してセルごとの画像を描画
class PixmapDelegate : public QStyledItemDelegate {
public:
    explicit PixmapDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        painter->fillRect(option.rect, Qt::black);  // デバッグ用に黒背景を適用

        QVariant value = index.data(Qt::DecorationRole);  // セルのデータを取得
        if (value.isValid() && value.canConvert<QPixmap>()) {
            QPixmap pixmap = qvariant_cast<QPixmap>(value);

            // 【修正】セルごとに異なる Pixmap を取得して描画
            QRect targetRect = option.rect.adjusted(-1, -1, 1, 1);
            painter->drawPixmap(targetRect, pixmap);
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override {
        return QSize(32, 32);
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    int rowCount = 5;  // 実験用に10x10のサイズ
    int columnCount = 5;

    QTableWidget table(rowCount, columnCount);
    table.setIconSize(QSize(32, 32));
    table.setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");
    table.setShowGrid(false);

    for (int col = 0; col < columnCount; ++col) {
        table.setColumnWidth(col, 33);  // 隙間をなくすために +1
    }
    for (int row = 0; row < rowCount; ++row) {
        table.setRowHeight(row, 33);  // 隙間をなくすために +1
    }

    // 画像のリストを用意
    QPixmap pix("./icon/town/town01.png");
    pix.scaled(32, 32);

    // 各セルに異なる画像を設定
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DecorationRole, pix);  // 画像を設定
            table.setItem(row, col, item);
        }
    }

    // カスタムデリゲートを設定
    table.setItemDelegate(new PixmapDelegate(&table));

    table.show();
    return a.exec();
}

#endif
