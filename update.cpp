#include "update.h"

updateDialog::updateDialog(QWidget *parent, QString version, QString date, QString changelog) : QDialog(parent)
{
	QIcon icons[3] = { QIcon(":/png/png/chg-new.png"), QIcon(":/png/png/chg-fix.png"), QIcon(":/png/png/chg-del.png") };
	QStringList changes = changelog.split(";");

	setupUi(this);

	setWindowFlags(Qt::Tool);

	label_icon->setPixmap(QPixmap(ICO_UPDATE));
	label_installedVersion->setText(APPVERS);
	label_installedDate->setText(QString("[ %1 ]").arg(APPDATE));
	label_availableVersion->setText(version);
	label_availableDate->setText(QString("[ %1 ]").arg(date));

	for(int i = 0; i < changes.size(); i++)
	{
		listWidget_changelog->addItem(new QListWidgetItem(icons[changes.at(i).at(0).digitValue() - 1], changes.at(i).mid(2), 0, changes.at(i).at(0).digitValue()));
	}

	listWidget_changelog->setMinimumHeight(3 * listWidget_changelog->visualItemRect(listWidget_changelog->item(0)).height() + 2);

	show();
	activateWindow();
	move(parent->mapToGlobal(parent->rect().center()) - rect().center());
}

void updateDialog::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_F1)
	{
		((MainWindow*)parent())->showHelp("01.06");
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
