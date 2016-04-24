#include "login.h"

loginDialog::loginDialog(QWidget *parent, QAuthenticator *auth) : QDialog(parent)
{
	setupUi(this);

	setWindowFlags(Qt::Tool);
	layout()->setSizeConstraint(QLayout::SetFixedSize);

	authenticator = auth;

	activateWindow();
	move(parent->mapToGlobal(parent->rect().center()) - rect().center());
	exec();
}

void loginDialog::on_pushButton_login_clicked()
{
	authenticator->setUser(lineEdit_user->text());
	authenticator->setPassword(lineEdit_pass->text());

	((MainWindow*)parent())->timeout.restart();

	close();
}

void loginDialog::on_pushButton_abort_clicked()
{
	close();
}
