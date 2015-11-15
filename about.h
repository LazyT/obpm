#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "ui_about.h"
#include "mainwindow.h"

#ifdef Q_OS_LINUX
#define ICO_ABOUT "QLabel{image:url(:/png/png/update_lin_48.png)} QLabel:hover{image:url(:/png/png/update_lin_64.png)}"
#elif defined Q_OS_WIN
#define ICO_ABOUT "QLabel{image:url(:/png/png/update_win_48.png)} QLabel:hover{image:url(:/png/png/update_win_64.png)}"
#elif defined Q_OS_OSX
#define ICO_ABOUT "QLabel{image:url(:/png/png/update_mac_48.png)} QLabel:hover{image:url(:/png/png/update_mac_64.png)}"
#endif

class aboutDialog : public QDialog, private Ui::Dialog_About
{
	Q_OBJECT

public:

	aboutDialog(QWidget *parent);

private slots:

	void mouseReleaseEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);
};

#endif
