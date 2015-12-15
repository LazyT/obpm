#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include "ui_setup.h"
#include "mainwindow.h"

class setupDialog : public QDialog, private Ui::Dialog_Setup
{
	Q_OBJECT

public:

	setupDialog(QWidget *parent, struct CONFIG*);

private:

	CONFIG *cfg;

private slots:

	void on_horizontalSlider_sys_valueChanged(int);
	void on_horizontalSlider_dia_valueChanged(int);
	void on_horizontalSlider_bpm_valueChanged(int);

	void on_pushButton_reset_clicked();
	void on_pushButton_save_clicked();

	void keyPressEvent(QKeyEvent*);
	void reject();
};

#endif
