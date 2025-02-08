#include "maindef.h"
#include "settingdlg_moc.h"

SettingDlg::SettingDlg(QWidget *pParent)
	: QDialog(pParent, NULL, TRUE), mIconTable(NULL)
{
	setCaption(M_QSTR(Message::TrC(MG_Setting)));
	setModal(false);

	QVBoxLayout *topLayout = new QVBoxLayout(this, 10, 10);
	// マップサイズ
	QHBoxLayout *layout1 = new QHBoxLayout(topLayout, 5);
	QLabel *mapLbl = new QLabel(this);
	mapLbl->setText(M_QSTR(Message::TrC(MG_MapSize)));
	mapLbl->setFixedWidth(70);
	layout1->addWidget(mapLbl);
	QLabel *rowLbl = new QLabel(this);
	rowLbl->setText(M_QSTR(Message::TrC(MG_MapSizeRow)));
	layout1->addWidget(rowLbl);
	mMapSizeRow = new QLineEdit(this);
	mMapSizeRow->setFixedWidth(50);
	layout1->addWidget(mMapSizeRow);
	QLabel *colLbl = new QLabel(this);
	colLbl->setText("  " + M_QSTR(Message::TrC(MG_MapSizeCol)));
	layout1->addWidget(colLbl);
	mMapSizeCol = new QLineEdit(this);
	mMapSizeCol->setFixedWidth(50);
	layout1->addWidget(mMapSizeCol);
	QLabel *dmyLbl1 = new QLabel(this);
	dmyLbl1->setFixedWidth(100);
	layout1->addWidget(dmyLbl1);
	QLabel *dmyLbl2 = new QLabel(this);
	dmyLbl2->setFixedWidth(100);
	layout1->addWidget(dmyLbl2);

	QHBoxLayout *layout2 = new QHBoxLayout(topLayout, 5);
	QLabel *dirLbl = new QLabel(this);
	dirLbl->setText(M_QSTR(Message::TrC(MG_IconDir)));
	layout2->addWidget(dirLbl);
	mIconDir = new QLineEdit(this);
	layout2->addWidget(mIconDir);
	QPixmap iconDir("./lib/fileopen.png");
	QPushButton *dirBtn = new QPushButton(iconDir, "", this);
	dirBtn->setFixedWidth(30);
	connect(dirBtn, SIGNAL(clicked()), this, SLOT(OpenIconDir()));
	layout2->addWidget(dirBtn);
//	QPixmap iconReload("./lib/reload.png");
//	QPushButton *reloadBtn = new QPushButton(iconReload, "", this);
//	reloadBtn->setFixedWidth(30);
//	connect(reloadBtn, SIGNAL(clicked()), this, SLOT(ReloadIcon()));
//	layout2->addWidget(reloadBtn);

	// QHBoxLayout *layout3 = new QHBoxLayout(topLayout, 5);
	// mIconTable = new IconTable(this, NULL);
	// layout3->addWidget(mIconTable);

	QHBoxLayout *ctlLayout = new QHBoxLayout(topLayout, 10);
	QPushButton *okBtn = new QPushButton( "OK", this );
	okBtn->setFixedWidth(80);
    okBtn->setDefault( TRUE );
    ctlLayout->addWidget( okBtn );

    QPushButton *cancelBtn = new QPushButton( "Cancel", this );
	cancelBtn->setFixedWidth(80);
    ctlLayout->addWidget( cancelBtn );

    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));
	resize(400, 120);
}

SettingDlg::~SettingDlg()
{
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
	QString dirnm = QFileDialog::getExistingDirectory(curDir.c_str(), this, "IconDir",
													M_QSTR(Message::TrC(MG_OpenIconDir)), TRUE);
	if (dirnm.isEmpty()) return;

	mIconDir->setText(dirnm);
}

void SettingDlg::ReloadIcon()
{
}

QString SettingDlg::GetIconDir() const
{
	return mIconDir->text();
}

int SettingDlg::GetMapSizeRow() const
{
	return atoi(mMapSizeRow->text().latin1());
}

int SettingDlg::GetMapSizeCol() const
{
	return atoi(mMapSizeCol->text().latin1());
}
