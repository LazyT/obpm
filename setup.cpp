#include "setup.h"

setupDialog::setupDialog(QWidget *parent, struct CONFIG *config) : QDialog(parent)
{
	setupUi(this);

	setWindowFlags(Qt::Tool);
	layout()->setSizeConstraint(QLayout::SetFixedSize);

	cfg = config;

	lineEdit_user1->setText(cfg->alias1);
	lineEdit_user2->setText(cfg->alias2);

	lineEdit_db->setText(cfg->database);

	checkBox_update->setChecked(cfg->update);

	horizontalSlider_sys->setValue(cfg->sys);
	horizontalSlider_dia->setValue(cfg->dia);
	horizontalSlider_bpm->setValue(cfg->bpm);

	checkBox_psd->setChecked(cfg->psd);
	checkBox_pot->setChecked(cfg->pot);

	show();
	activateWindow();
	move(parent->mapToGlobal(parent->rect().center()) - rect().center());
}


void setupDialog::on_toolButton_db_clicked()
{
	QString dirname = QFileDialog::getExistingDirectory(this, tr("Select Directory for Database"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

	if(!dirname.isEmpty())
	{
		lineEdit_db->setText(dirname + "/obpm.sql");
	}
}

void setupDialog::on_pushButton_reset_clicked()
{
	lineEdit_user1->setText(tr("User 1"));
	lineEdit_user2->setText(tr("User 2"));

	lineEdit_db->setText(DB);

	checkBox_update->setChecked(true);

	horizontalSlider_sys->setValue(SYS_NORM);
	horizontalSlider_dia->setValue(DIA_NORM);
	horizontalSlider_bpm->setValue(BPM_NORM);

	checkBox_psd->setChecked(true);
	checkBox_pot->setChecked(false);
}

void setupDialog::on_pushButton_save_clicked()
{
	cfg->alias1 = lineEdit_user1->text().isEmpty() ? tr("User 1") : lineEdit_user1->text();
	cfg->alias2 = lineEdit_user2->text().isEmpty() ? tr("User 2") : lineEdit_user2->text();

	cfg->database = lineEdit_db->text();

	cfg->update = checkBox_update->isChecked();

	cfg->sys = horizontalSlider_sys->value();
	cfg->dia = horizontalSlider_dia->value();
	cfg->bpm = horizontalSlider_bpm->value();

	cfg->psd = checkBox_psd->isChecked();
	cfg->pot = checkBox_pot->isChecked();

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
