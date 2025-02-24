#include "maindef.h"
#include "settingdlg_moc.h"

SettingDlg::SettingDlg(QWidget *pParent)
	: QDialog(pParent), mIconTable(NULL)
{
	setWindowTitle(M_QSTR(Message::TrC(MG_Setting)));
	setModal(false);

	// マップサイズ
	QGroupBox *mapGrp = new QGroupBox(M_QSTR(Message::TrC(MG_MapSize)));
	// QLabel *mapLbl = new QLabel(M_QSTR(Message::TrC(MG_MapSize)));
	// mapLbl->setFixedWidth(70);
	// hLayout1->addWidget(mapLbl);
	QLabel *rowLbl = new QLabel(M_QSTR(Message::TrC(MG_MapSizeRow)));
	mMapSizeRow = new QLineEdit();
	mMapSizeRow->setFixedWidth(50);
	QLabel *colLbl = new QLabel("  " + M_QSTR(Message::TrC(MG_MapSizeCol)));
	mMapSizeCol = new QLineEdit();
	mMapSizeCol->setFixedWidth(50);
	QLabel *dmyLbl1 = new QLabel("");
	dmyLbl1->setFixedWidth(100);
	QLabel *dmyLbl2 = new QLabel("");
	dmyLbl2->setFixedWidth(100);
	QHBoxLayout *hLayout1 = new QHBoxLayout;
	hLayout1->addWidget(rowLbl);
	hLayout1->addWidget(mMapSizeRow);
	hLayout1->addWidget(colLbl);
	hLayout1->addWidget(mMapSizeCol);
	hLayout1->addWidget(dmyLbl1);
	hLayout1->addWidget(dmyLbl2);
	mapGrp->setLayout(hLayout1);

	// アイコンフォルダ
	QGroupBox *dirGrp = new QGroupBox(M_QSTR(Message::TrC(MG_IconDir)));
	// QLabel *dirLbl = new QLabel(M_QSTR(Message::TrC(MG_IconDir)));
	// layout2->addWidget(dirLbl);
	mIconDir = new QLineEdit();
	QIcon iconDir("./lib/open.png");
	QPushButton *dirBtn = new QPushButton(iconDir, "");
	dirBtn->setFixedWidth(30);
	QHBoxLayout *hLayout2 = new QHBoxLayout;
	hLayout2->addWidget(mIconDir);
	hLayout2->addWidget(dirBtn);
	dirGrp->setLayout(hLayout2);
	connect(dirBtn, SIGNAL(clicked()), this, SLOT(OpenIconDir()));

	QHBoxLayout *ctlLayout = new QHBoxLayout;
	QPushButton *okBtn = new QPushButton("OK");
	okBtn->setFixedWidth(80);
    okBtn->setDefault( true );
    ctlLayout->addWidget( okBtn );

    QPushButton *cancelBtn = new QPushButton("Cancel");
	cancelBtn->setFixedWidth(80);
    ctlLayout->addWidget( cancelBtn );
    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));

	QVBoxLayout *topLayout = new QVBoxLayout;
    topLayout->addWidget(mapGrp);
    topLayout->addWidget(dirGrp);
    topLayout->addLayout(ctlLayout);
    setLayout(topLayout);

	resize(400, 120);
}

void SettingDlg::Init(const string &pIconDir, int pMapSizeRow, int pMapSizeCol)
{
	mIconDir->setText(pIconDir.c_str());
	char str[64];
	sprintf(str, "%d", pMapSizeRow);
	mMapSizeRow->setText(str);
	sprintf(str, "%d", pMapSizeCol);
	mMapSizeCol->setText(str);
}

void SettingDlg::OpenIconDir()
{
	string curDir = FileUtil::GetCurDir();
	QString dirnm = QFileDialog::getExistingDirectory(this, M_QSTR(Message::TrC(MG_OpenIconDir)), curDir.c_str());
	if (dirnm.isEmpty()) return;

	mIconDir->setText(dirnm);
}

QString SettingDlg::GetIconDir() const
{
	return mIconDir->text();
}

int SettingDlg::GetMapSizeRow() const
{
	return mMapSizeRow->text().toInt();
}

int SettingDlg::GetMapSizeCol() const
{
	return mMapSizeCol->text().toInt();
}
