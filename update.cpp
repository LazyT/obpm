#include "update.h"

updateDialog::updateDialog(QWidget *parent, QString version, QString date) : QDialog(parent)
{
	setupUi(this);

	setWindowFlags(Qt::Tool);
	layout()->setSizeConstraint(QLayout::SetFixedSize);

	label_icon->setPixmap(QPixmap(ICO_UPDATE));
	label_installedVersion->setText(APPVERS);
	label_installedDate->setText(QString("[ %1 ]").arg(APPDATE));
	label_availableVersion->setText(version);
	label_availableDate->setText(QString("[ %1 ]").arg(date));

	show();
	activateWindow();
	move(parent->mapToGlobal(parent->rect().center()) - rect().center());
}

void updateDialog::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_F1)
	{
		QMessageBox::information(this, APPNAME, tr("Help is not yet implemented..."));
	}

	QDialog::keyPressEvent(ke);
}

void updateDialog::on_pushButton_Update_clicked()
{
	QDesktopServices::openUrl(QUrl("https://github.com/LazyT/obpm/releases/latest"));

	done(Accepted);
}

void updateDialog::on_pushButton_Ignore_clicked()
{
	done(Rejected);
}
