#ifndef SQLDIALOG_H
#define SQLDIALOG_H

#include "ui_sql.h"
#include "mainwindow.h"

class sqlDialog : public QDialog, private Ui::Dialog_SQL
{
	Q_OBJECT

public:

	sqlDialog(QWidget *parent);
	bool success;

private:

	QSqlDatabase ramdb;
	bool createDB();

private slots:

	void runQuery();
	void on_comboBox_activated(int);
	void on_toolButton_dt2ut_clicked();
	void on_toolButton_ut2dt_clicked();
	void on_checkBox_clicked(bool);
	void keyPressEvent(QKeyEvent*);
	void reject();
};

#endif
