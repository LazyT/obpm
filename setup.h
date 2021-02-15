#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

class mainwindow;

#include "ui_setup.h"
#include "mainwindow.h"

class setupDialog : public QDialog, private Ui::Dialog_Setup
{
	Q_OBJECT

public:

	setupDialog(QWidget *parent, struct CONFIG*);
    QSqlDatabase db, ramdb;

private:

	CONFIG *cfg;

public slots:
    void on_deluser1_clicked();
    void on_deluser2_clicked();

private slots:

	void on_horizontalSlider_sys_valueChanged(int);
	void on_horizontalSlider_dia_valueChanged(int);
	void on_horizontalSlider_bpm_valueChanged(int);

	void on_toolButton_db_clicked();
	void on_pushButton_reset_clicked();
	void on_pushButton_save_clicked();

	void keyPressEvent(QKeyEvent*);
	void reject();
};

#endif
