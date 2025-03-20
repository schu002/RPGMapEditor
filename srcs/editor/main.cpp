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

// QStyledItemDelegate ���g�����ăZ�����Ƃ̉摜��`��
class PixmapDelegate : public QStyledItemDelegate {
public:
    explicit PixmapDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        painter->fillRect(option.rect, Qt::black);  // �f�o�b�O�p�ɍ��w�i��K�p

        QVariant value = index.data(Qt::DecorationRole);  // �Z���̃f�[�^���擾
        if (value.isValid() && value.canConvert<QPixmap>()) {
            QPixmap pixmap = qvariant_cast<QPixmap>(value);

            // �y�C���z�Z�����ƂɈقȂ� Pixmap ���擾���ĕ`��
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

    int rowCount = 5;  // �����p��10x10�̃T�C�Y
    int columnCount = 5;

    QTableWidget table(rowCount, columnCount);
    table.setIconSize(QSize(32, 32));
    table.setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");
    table.setShowGrid(false);

    for (int col = 0; col < columnCount; ++col) {
        table.setColumnWidth(col, 33);  // ���Ԃ��Ȃ������߂� +1
    }
    for (int row = 0; row < rowCount; ++row) {
        table.setRowHeight(row, 33);  // ���Ԃ��Ȃ������߂� +1
    }

    // �摜�̃��X�g��p��
    QPixmap pix("./icon/town/town01.png");
    pix.scaled(32, 32);

    // �e�Z���ɈقȂ�摜��ݒ�
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DecorationRole, pix);  // �摜��ݒ�
            table.setItem(row, col, item);
        }
    }

    // �J�X�^���f���Q�[�g��ݒ�
    table.setItemDelegate(new PixmapDelegate(&table));

    table.show();
    return a.exec();
}

#endif
