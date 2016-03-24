#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "ui_login.h"
#include "mainwindow.h"

class loginDialog : public QDialog, private Ui::Dialog_Login
{
	Q_OBJECT

public:

	loginDialog(QWidget *parent, QAuthenticator*);

private:

	QAuthenticator *authenticator;

private slots:

	void on_pushButton_login_clicked();
	void on_pushButton_abort_clicked();
};

#endif
