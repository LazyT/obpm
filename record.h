#ifndef RECORDDIALOG_H
#define RECORDDIALOG_H

#include "ui_record.h"
#include "mainwindow.h"

class recordDialog : public QDialog, private Ui::Dialog_Record
{
	Q_OBJECT

public:

	recordDialog(QWidget *parent);

private slots:

	void on_toolButton_user_toggled(bool);
	void on_toolButton_add_clicked();

	void keyPressEvent(QKeyEvent*);
};

#endif
