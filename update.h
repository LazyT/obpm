#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

class mainwindow;

#include "ui_update.h"
#include "mainwindow.h"

#ifdef Q_OS_LINUX
	#define ICO_UPDATE ":/png/png/update_lin_64.png"
#elif defined Q_OS_WIN
	#define ICO_UPDATE ":/png/png/update_win_64.png"
#elif defined Q_OS_OSX
	#define ICO_UPDATE ":/png/png/update_mac_64.png"
#endif

class updateDialog : public QDialog, private Ui::Dialog_Update
{
	Q_OBJECT

public:

	updateDialog(QWidget *parent, QString version, QString date, QString changelog);

private slots:

	void keyPressEvent(QKeyEvent*);

	void on_pushButton_Update_clicked();
	void on_pushButton_Ignore_clicked();
};

#endif
