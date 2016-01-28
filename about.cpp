#include "about.h"

aboutDialog::aboutDialog(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	setWindowFlags(Qt::Tool);
	layout()->setSizeConstraint(QLayout::SetFixedSize);

	label_Version->setText(tr("Version %1 - %2").arg(APPVERS, APPDATE));
	label_Update->setStyleSheet(ICO_ABOUT);

	textEdit->setAlignment(Qt::AlignCenter);
	textEdit->textCursor().insertText(tr("This program is Freeware and may be installed and used free of charge for non-commercial use on as many computers as you like without limitations.\n\nA liability for any damages resulting from the use is excluded. Use at your own risk!"));
	textEdit->moveCursor(QTextCursor::Start);
	textEdit->setFixedHeight(5 * textEdit->fontMetrics().height() + textEdit->document()->documentMargin() + 2);

	show();
	activateWindow();
	move(parent->mapToGlobal(parent->rect().center()) - rect().center());
}

void aboutDialog::mouseReleaseEvent(QMouseEvent *me)
{
	QWidget *child = QWidget::childAt(me->pos());

	if(child)
	{
		QString name = child->objectName();

		if(name == "label_Mail")
		{
			QDesktopServices::openUrl(QUrl(QByteArray::fromBase64("bWFpbHRvOkxhenlUQG1haWxib3gub3JnP3N1YmplY3Q9T0JQTSZib2R5PVdyaXRlIGluIEVuZ2xpc2ggb3IgR2VybWFuIHBsZWFzZS4uLg==")));
		}
		else if(name == "label_Forum")
		{
			QDesktopServices::openUrl(QUrl(QByteArray::fromBase64("aHR0cDovL2JvYXJkLm5ldGRva3Rvci5kZS9iZWl0cmFnL2JsdXRkcnVja21lc3NnZXJhZXQtb21yb24tbTUwMGl0LWJlc2l0emVyLXRlc3Rlci1nZXN1Y2h0LjI1MjU1MQ==")));
		}
		else if(name == "label_Donation")
		{
			if(QMessageBox::warning(this, APPNAME, tr("Please note the following points:\n\n* The payment is made voluntarily without the acquisition of claims.\n* You receive no rights to the offered software.\n* Because this is not a donation in juridical sense no certificate can be issued.\n\nWould you like to support the further development of this project nevertheless?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
			{
				new donationDialog((QWidget*)parent());
			}
		}
		else if(name == "label_Update")
		{
			QDesktopServices::openUrl(QUrl(QByteArray::fromBase64("aHR0cHM6Ly9naXRodWIuY29tL0xhenlUL29icG0vcmVsZWFzZXM=")));
		}
		else if(name == "label_Language")
		{
			QDesktopServices::openUrl(QUrl(QByteArray::fromBase64("aHR0cHM6Ly9naXRodWIuY29tL0xhenlUL29icG0vdHJlZS9tYXN0ZXIvbG5n")));
		}
	}
}

void aboutDialog::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_F1)
	{
		((MainWindow*)parent())->showHelp("01.04");
	}

	QDialog::keyPressEvent(ke);
}
