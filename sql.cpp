#include "sql.h"

sqlDialog::sqlDialog(QWidget *parent) : QDialog(parent)
{
	if(!(success = createDB()))
	{
		return;
	}

	setupUi(this);

	setWindowFlags(Qt::Tool);

	tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	tableWidget->setMinimumHeight(5 * tableWidget->rowHeight(0) + tableWidget->horizontalHeader()->height() + 4);

	lineEdit_unixtime->setValidator(new QRegExpValidator(QRegExp("[0-9]*")));
	lineEdit_datetime->setValidator(new QRegExpValidator(QRegExp("(0[1-9]|[12][0-9]|3[01])\\.(0[1-9]|1[012])\\.(19[7-9][0-9]|20[0-9][0-9]) ([0-1][0-9]|2[0-3]):([0-5][0-9])")));

	comboBox->addItem(QString("select * from U%1 where SYS > %2").arg(1 + ((MainWindow*)parent)->user).arg(((MainWindow*)parent)->cfg.sys));
	comboBox->addItem(QString("select * from U%1 where DIA > %2").arg(1 + ((MainWindow*)parent)->user).arg(((MainWindow*)parent)->cfg.dia));
	comboBox->addItem(QString("select * from U%1 where BPM > %2").arg(1 + ((MainWindow*)parent)->user).arg(((MainWindow*)parent)->cfg.bpm));

	if(((MainWindow*)parent)->filter->isChecked())
	{
		comboBox->addItem(QString("select * from U%1 where UTS >= %2 and UTS <= %3").arg(1 + ((MainWindow*)parent)->user).arg(((MainWindow*)parent)->rangeStart->dateTime().toTime_t()).arg(((MainWindow*)parent)->rangeStop->dateTime().toTime_t()));
	}
	else
	{
		comboBox->addItem(QString("select * from U%1 where UTS > %2").arg(1 + ((MainWindow*)parent)->user).arg(QDateTime::currentDateTime().addDays(-7).toTime_t()));
	}

	connect(lineEdit_unixtime, SIGNAL(returnPressed()), this, SLOT(on_toolButton_ut2dt_clicked()));
	connect(lineEdit_datetime, SIGNAL(returnPressed()), this, SLOT(on_toolButton_dt2ut_clicked()));

	move(parent->mapToGlobal(parent->rect().center()) - rect().center());
}

bool sqlDialog::createDB()
{
	ramdb = QSqlDatabase::addDatabase("QSQLITE", "RAMDB");

	ramdb.setDatabaseName(":memory:");

	if(ramdb.open())
	{
		QSqlQuery query(ramdb);

		query.exec("CREATE TABLE 'U1' ('uts' INTEGER, 'sys' INTEGER, 'dia' INTEGER, 'bpm' INTEGER, 'msg' TEXT)");
		query.exec("CREATE TABLE 'U2' ('uts' INTEGER, 'sys' INTEGER, 'dia' INTEGER, 'bpm' INTEGER, 'msg' TEXT)");

		for(int i = 0; i < ((MainWindow*)parent())->healthdata[0].count(); i++)
		{
			query.exec(QString("INSERT INTO 'U1' VALUES (%1, %2, %3, %4, '%5')").arg(((MainWindow*)parent())->healthdata[0].at(i).time).arg(((MainWindow*)parent())->healthdata[0].at(i).sys).arg(((MainWindow*)parent())->healthdata[0].at(i).dia).arg(((MainWindow*)parent())->healthdata[0].at(i).bpm).arg(((MainWindow*)parent())->healthdata[0].at(i).msg));
		}

		for(int i = 0; i < ((MainWindow*)parent())->healthdata[1].count(); i++)
		{
			query.exec(QString("INSERT INTO 'U2' VALUES (%1, %2, %3, %4, '%5')").arg(((MainWindow*)parent())->healthdata[1].at(i).time).arg(((MainWindow*)parent())->healthdata[1].at(i).sys).arg(((MainWindow*)parent())->healthdata[1].at(i).dia).arg(((MainWindow*)parent())->healthdata[1].at(i).bpm).arg(((MainWindow*)parent())->healthdata[1].at(i).msg));
		}
	}
	else
	{
		QMessageBox::warning((QWidget*)parent(), APPNAME, tr("Could not create database in memory!\n\n%1").arg(ramdb.lastError().driverText()));

		return false;
	}

	return true;
}

void sqlDialog::runQuery()
{
	QTableWidgetItem *twi;
	QSqlQuery query(ramdb);
	int row = 0;

	tableWidget->setRowCount(0);

	query.exec(comboBox->currentText());

	while(query.next())
	{
		tableWidget->insertRow(row);

		if(query.record().contains("uts"))
		{
			twi = new QTableWidgetItem();

			if(checkBox->isChecked())
			{
				twi->setData(Qt::DisplayRole, QDateTime::fromTime_t(query.value("uts").toInt()).toString("dd.MM.yyyy hh:mm"));
			}
			else
			{
				twi->setData(Qt::DisplayRole, query.value("uts").toInt());
			}

			twi->setTextAlignment(Qt::AlignCenter);
			tableWidget->setItem(row, 0, twi);
		}

		if(query.record().contains("sys"))
		{
			twi = new QTableWidgetItem();
			twi->setData(Qt::DisplayRole, query.value("sys").toInt());
			twi->setTextAlignment(Qt::AlignCenter);
			tableWidget->setItem(row, 1, twi);
		}

		if(query.record().contains("dia"))
		{
			twi = new QTableWidgetItem();
			twi->setData(Qt::DisplayRole, query.value("dia").toInt());
			twi->setTextAlignment(Qt::AlignCenter);
			tableWidget->setItem(row, 2, twi);
		}

		if(query.record().contains("bpm"))
		{
			twi = new QTableWidgetItem();
			twi->setData(Qt::DisplayRole, query.value("bpm").toInt());
			twi->setTextAlignment(Qt::AlignCenter);
			tableWidget->setItem(row, 3, twi);
		}

		if(query.record().contains("msg"))
		{
			twi = new QTableWidgetItem();
			twi->setData(Qt::DisplayRole, query.value("msg").toString());
			twi->setTextAlignment(Qt::AlignCenter);
			tableWidget->setItem(row, 4, twi);
		}

		row++;
	}

	if(!row)
	{
		groupBox_results->setTitle(tr("Results"));

		QMessageBox::information(this, APPNAME, tr("No results for this query found!"));
	}
	else
	{
		groupBox_results->setTitle(tr("Results [ %1 ]").arg(row));
	}
}

void sqlDialog::on_comboBox_activated(__attribute__((unused)) int index)
{
	runQuery();
}

void sqlDialog::on_toolButton_dt2ut_clicked()
{
	if(lineEdit_datetime->text().length() < 16)
	{
		QMessageBox::warning(this, APPNAME, tr("Please enter valid date and time!"));
	}
	else
	{
		lineEdit_unixtime->setText(QString("%1").arg(QDateTime::fromString(lineEdit_datetime->text(), "dd.MM.yyyy hh:mm").toTime_t()));
	}
}

void sqlDialog::on_toolButton_ut2dt_clicked()
{
	if(lineEdit_unixtime->text().isEmpty())
	{
		QMessageBox::warning(this, APPNAME, tr("Please enter unix timestamp!"));
	}
	else
	{
		lineEdit_datetime->setText(QDateTime::fromTime_t(lineEdit_unixtime->text().toInt()).toString("dd.MM.yyyy hh:mm"));
	}
}

void sqlDialog::on_checkBox_clicked(bool enabled)
{
	QTableWidgetItem *twi = tableWidget->horizontalHeaderItem(0);

	twi->setText(enabled ? tr("D&T") : "UTS");

	tableWidget->setHorizontalHeaderItem(0, twi);

	runQuery();
}

void sqlDialog::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_F1)
	{
		((MainWindow*)parent())->showHelp("01.07");
	}

	QDialog::keyPressEvent(ke);
}

void sqlDialog::reject()
{
	ramdb = QSqlDatabase();

	QSqlDatabase::removeDatabase("RAMDB");

	accept();
}
