#include "record.h"

recordDialog::recordDialog(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	setWindowFlags(Qt::Tool);

	toolButton_user->setChecked(((MainWindow*)parent)->user);
	dateTimeEdit->setDateTime(QDateTime::currentDateTime());
	spinBox_sys->setValue(((MainWindow*)parent)->cfg.sys);
	spinBox_dia->setValue(((MainWindow*)parent)->cfg.dia);
	spinBox_bpm->setValue(((MainWindow*)parent)->cfg.bpm);
	spinBox_ihb->setValue(((MainWindow*)parent)->cfg.ihb);

	show();
	activateWindow();
	move(parent->mapToGlobal(parent->rect().center()) - rect().center());
}

void recordDialog::on_toolButton_user_toggled(bool checked)
{
	QList<QAction*> actions = ((MainWindow*)parent())->groupUser->actions();

	actions.at(0)->setChecked(!checked);
	actions.at(1)->setChecked(checked);
}

void recordDialog::on_toolButton_add_clicked()
{
	HEALTHDATA set;

	set.time = dateTimeEdit->dateTime().toTime_t();
	set.time -= set.time % 60;
	set.sys = spinBox_sys->value();
	set.dia = spinBox_dia->value();
	set.bpm = spinBox_bpm->value();
	set.ihb = spinBox_ihb->value();
	set.msg = lineEdit_comment->text();

	((MainWindow*)parent())->healthdata[toolButton_user->isChecked()].append(set);

	if(((MainWindow*)parent())->validateData(((MainWindow*)parent())->groupUser->checkedAction(), true))
	{
		QMessageBox::warning(this, APPNAME, tr("Manual record for \"%1\" skipped because already exists!").arg(toolButton_user->isChecked() ? ((MainWindow*)parent())->cfg.alias2 : ((MainWindow*)parent())->cfg.alias1));
	}
	else
	{
		((MainWindow*)parent())->buildGraph(((MainWindow*)parent())->healthdata[((MainWindow*)parent())->user], ((MainWindow*)parent())->healthstat[((MainWindow*)parent())->user]);
	}
}

void recordDialog::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_F1)
	{
		((MainWindow*)parent())->showHelp("01.08");
	}

	QDialog::keyPressEvent(ke);
}
