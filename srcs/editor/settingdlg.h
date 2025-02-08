#ifndef settingdlg_h
#define settingdlg_h 1

class SettingDlg : public QDialog 
{
	Q_OBJECT

public:
	SettingDlg(QWidget *pParent);
	~SettingDlg();
	void Init(const string &pIconDir, int pMapSizeRow, int pMapSizeCol);
	QString GetIconDir() const;
	int GetMapSizeRow() const;
	int GetMapSizeCol() const;

public slots:
	void OpenIconDir();
	void ReloadIcon();

private:
	QLineEdit *mMapSizeRow, *mMapSizeCol;
	QLineEdit *mIconDir;
	IconTable *mIconTable;
};

#endif
