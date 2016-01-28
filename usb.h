#ifndef USBDIALOG_H
#define USBDIALOG_H

#include "ui_usb.h"
#include "mainwindow.h"
#include "hidapi.h"

#define VID 0x0590	// Omron
#define PID 0x0090	// M400IT & M500IT

struct devinfo {
	uint vid;
	uint pid;
	QString man;
	QString prd;
	QString ser;
};

Q_DECLARE_METATYPE(devinfo)

class usbDialog : public QDialog, private Ui::Dialog_USB
{
	Q_OBJECT

public:

	usbDialog(QWidget *parent, bool);
	int hids_found;
	int user1, user2;

private:

	devinfo di;
	hid_device *dev;
	bool import, abort;
	QFile dumpfile;
	int mem;

private slots:

	int buildCRC(unsigned char*);
	bool readRawData();
	void dumpRawData(bool, int, unsigned char*);
	int sendCMD(unsigned char*);
	void scanHID();
	void on_comboBox_currentIndexChanged(int);
	void on_pushButton_import_clicked();
	void keyPressEvent(QKeyEvent*);
	void reject();
};

#endif
