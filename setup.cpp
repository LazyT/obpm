#include "setup.h"

setupDialog::setupDialog(QWidget *parent, struct CONFIG *config) : QDialog(parent)
{
	setupUi(this);

	setWindowFlags(Qt::Tool);
	layout()->setSizeConstraint(QLayout::SetFixedSize);

	cfg = config;

	horizontalSlider_sys->setValue(cfg->sys);
	horizontalSlider_dia->setValue(cfg->dia);
	horizontalSlider_bpm->setValue(cfg->bpm);
	checkBox_legend->setChecked(cfg->legend);

	if(cfg->style == QCPGraph::lsLine)
	{
		radioButton_line->setChecked(true);
	}
	else if(cfg->style == QCPGraph::lsStepCenter)
	{
		radioButton_step->setChecked(true);
	}
	else if(cfg->style == QCPGraph::lsImpulse)
	{
		radioButton_impulse->setChecked(true);
	}
	else
	{
		radioButton_point->setChecked(true);
	}

	checkBox_update->setChecked(cfg->update);

	lineEdit_user1->setText(cfg->alias1);
	lineEdit_user2->setText(cfg->alias2);

	lineEdit_db->setText(cfg->database);

	show();
	activateWindow();
	move(parent->mapToGlobal(parent->rect().center()) - rect().center());
}


void setupDialog::on_toolButton_db_clicked()
{
	QString dirname = QFileDialog::getExistingDirectory(this, tr("Select Directory for Database"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

	if(!dirname.isEmpty())
	{
		lineEdit_db->setText(dirname + QDir::separator() + "obpm.sql");
	}
}

void setupDialog::on_pushButton_reset_clicked()
{
	horizontalSlider_sys->setValue(SYS_NORM);
	horizontalSlider_dia->setValue(DIA_NORM);
	horizontalSlider_bpm->setValue(BPM_NORM);

	checkBox_legend->setChecked(true);
	radioButton_line->setChecked(true);

	checkBox_update->setChecked(true);

	lineEdit_user1->setText(tr("User 1"));
	lineEdit_user2->setText(tr("User 2"));

	lineEdit_db->setText(DB);
}

void setupDialog::on_pushButton_save_clicked()
{
	cfg->sys = horizontalSlider_sys->value();
	cfg->dia = horizontalSlider_dia->value();
	cfg->bpm = horizontalSlider_bpm->value();
	cfg->legend = checkBox_legend->isChecked();

	if(radioButton_line->isChecked())
	{
		cfg->style = QCPGraph::lsLine;
	}
	else if(radioButton_step->isChecked())
	{
		cfg->style = QCPGraph::lsStepCenter;
	}
	else if(radioButton_impulse->isChecked())
	{
		cfg->style = QCPGraph::lsImpulse;
	}
	else
	{
		cfg->style = QCPGraph::lsNone;
	}

	cfg->update = checkBox_update->isChecked();

	cfg->alias1 = lineEdit_user1->text().isEmpty() ? tr("User 1") : lineEdit_user1->text();
	cfg->alias2 = lineEdit_user2->text().isEmpty() ? tr("User 2") : lineEdit_user2->text();

	cfg->database = lineEdit_db->text();

	done(QDialog::Accepted);
}

void setupDialog::on_horizontalSlider_sys_valueChanged(int value)
{
	label_sys->setText(QString("%1").arg(value));
}

void setupDialog::on_horizontalSlider_dia_valueChanged(int value)
{
	label_dia->setText(QString("%1").arg(value));
}

void setupDialog::on_horizontalSlider_bpm_valueChanged(int value)
{
	label_bpm->setText(QString("%1").arg(value));
}

void setupDialog::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_F1)
	{
		((MainWindow*)parent())->showHelp("01.02");
	}

	QDialog::keyPressEvent(ke);
}

void setupDialog::reject()
{
	if(QMessageBox::question(this, APPNAME, tr("Abort setup and discard all changes?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		done(QDialog::Rejected);
	}
}
