#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow mw;

	mw.move(QApplication::desktop()->screen()->rect().center() - mw.rect().center());

	mw.show();

	return app.exec();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	QApplication::setStyle("Fusion");

	if(!QLocale::system().name().startsWith("en_"))
	{
		if(appTranslator.load("obpm_" + QLocale::system().name(), QApplication::applicationDirPath() + "/lng"))
		{
			QApplication::installTranslator(&appTranslator);

			if(sysTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
			{
				QApplication::installTranslator(&sysTranslator);
			}
			else if(sysTranslator.load("qt_" + QLocale::system().name(), QApplication::applicationDirPath() + "/lng"))
			{
				QApplication::installTranslator(&sysTranslator);
			}
		}
	}

	setupUi(this);

	QMenu *menu = new QMenu(this);
	menu->addAction(action_PrintPreview);
	((QToolButton*)mainToolBar->widgetForAction(action_Print))->setMenu(menu);
	((QToolButton*)mainToolBar->widgetForAction(action_Print))->setPopupMode(QToolButton::MenuButtonPopup);

	groupUser = new QActionGroup(this);
	groupUser->addAction(action_User1);
	groupUser->addAction(action_User2);

	rangeStart = new QDateTimeEdit(QDateTime(QDate::currentDate(), QTime(0, 0, 0, 0)), this);
	rangeStop = new QDateTimeEdit(QDateTime(QDate::currentDate(), QTime(23, 59, 59, 999)), this);
	rangeStart->setToolTip(tr("Start analysis"));
	rangeStop->setToolTip(tr("Stop analysis"));
	rangeStart->setStatusTip(tr("Start analysis"));
	rangeStop->setStatusTip(tr("Stop analysis"));
	rangeStart->setCalendarPopup(true);
	rangeStop->setCalendarPopup(true);
	rangeStart->setDisplayFormat("ddd dd.MM.yyyy hh:mm");
	rangeStop->setDisplayFormat("ddd dd.MM.yyyy hh:mm");
	rangeStart->setAlignment(Qt::AlignHCenter);
	rangeStop->setAlignment(Qt::AlignHCenter);
	rangeStart->calendarWidget()->setGridVisible(true);
	rangeStop->calendarWidget()->setGridVisible(true);
	rangeStart->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
	rangeStop->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);

	mainToolBar->addWidget(rangeStart);
	mainToolBar->addWidget(rangeStop);

	widget_bp->yAxis->setLabel("mmHg");
	widget_hr->yAxis->setLabel("bpm");

	QCPItemStraightLine *line_sys = new QCPItemStraightLine(widget_bp);
	QCPItemStraightLine *line_dia = new QCPItemStraightLine(widget_bp);
	QCPItemStraightLine *line_bpm = new QCPItemStraightLine(widget_hr);
	line_sys->setPen(QPen(Qt::green));
	line_dia->setPen(QPen(Qt::green));
	line_bpm->setPen(QPen(Qt::green));
	line_sys->point1->setTypeX(QCPItemPosition::ptAbsolute);
	line_sys->point2->setTypeX(QCPItemPosition::ptAbsolute);
	line_dia->point1->setTypeX(QCPItemPosition::ptAbsolute);
	line_dia->point2->setTypeX(QCPItemPosition::ptAbsolute);
	line_bpm->point1->setTypeX(QCPItemPosition::ptAbsolute);
	line_bpm->point2->setTypeX(QCPItemPosition::ptAbsolute);
	line_sys->point1->setCoords(0, SYS_NORM);
	line_sys->point2->setCoords(1, SYS_NORM);
	line_dia->point1->setCoords(0, DIA_NORM);
	line_dia->point2->setCoords(1, DIA_NORM);
	line_bpm->point1->setCoords(0, BPM_NORM);
	line_bpm->point2->setCoords(1, BPM_NORM);
	widget_bp->addItem(line_sys);
	widget_bp->addItem(line_dia);
	widget_hr->addItem(line_bpm);

	widget_bp->addGraph();
	widget_bp->addGraph();
	widget_bp->graph(0)->setPen(QPen(Qt::magenta));
	widget_bp->graph(1)->setPen(QPen(Qt::blue));
	widget_bp->graph(0)->setScatterStyle(QCPScatterStyle(QPixmap(":/png/png/sys.png")));
	widget_bp->graph(1)->setScatterStyle(QCPScatterStyle(QPixmap(":/png/png/dia.png")));
	widget_bp->graph(0)->setLineStyle(QCPGraph::lsLine);
	widget_bp->graph(1)->setLineStyle(QCPGraph::lsLine);
	widget_bp->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	widget_bp->xAxis->setDateTimeFormat("hh:mm\ndd.MM.yy");
	widget_bp->yAxis->setRange(DIA_NORM - 15, SYS_NORM + 15);
	widget_bp->yAxis->setAutoTickStep(false);
	widget_bp->yAxis->setTickStep(10);
	widget_bp->plottable(0)->setName("SYS");
	widget_bp->plottable(1)->setName("DIA");
	widget_bp->legend->setVisible(true);
	widget_bp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	widget_bp->axisRect(0)->setRangeDrag(Qt::Horizontal);
	widget_bp->axisRect(0)->setRangeZoom(Qt::Horizontal);

	widget_hr->addGraph();
	widget_hr->graph(0)->setPen(QPen(Qt::red));
	widget_hr->graph(0)->setScatterStyle(QCPScatterStyle(QPixmap(":/png/png/bpm.png")));
	widget_hr->graph(0)->setLineStyle(QCPGraph::lsLine);
	widget_hr->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	widget_hr->xAxis->setDateTimeFormat("hh:mm\ndd.MM.yy");
	widget_hr->yAxis->setRange(60, 80);
	widget_hr->yAxis->setAutoTickStep(false);
	widget_hr->yAxis->setTickStep(5);
	widget_hr->plottable(0)->setName(tr("Pulse"));
	widget_hr->legend->setVisible(true);
	widget_hr->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	widget_hr->axisRect(0)->setRangeDrag(Qt::Horizontal);
	widget_hr->axisRect(0)->setRangeZoom(Qt::Horizontal);

	connect(rangeStart, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(dateChanged()));
	connect(rangeStop, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(dateChanged()));
	connect(widget_bp, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
	connect(widget_hr, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
	connect(widget_bp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisBPChanged(QCPRange)));
	connect(widget_hr->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisHRChanged(QCPRange)));

	dateChanged();
}

void MainWindow::getHealthStats(bool user)
{
	QVector <HEALTHDATA> cpy(healthdata[user]);

	qSort(cpy.begin(), cpy.end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.sys < b.sys; });

	healthstat[user].sys_min = cpy.first().sys;
	healthstat[user].sys_mid = cpy.at(cpy.count() / 2).sys;
	healthstat[user].sys_max = cpy.last().sys;

	qSort(cpy.begin(), cpy.end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.dia < b.dia; });

	healthstat[user].dia_min = cpy.first().dia;
	healthstat[user].dia_mid = cpy.at(cpy.count() / 2).dia;
	healthstat[user].dia_max = cpy.last().dia;

	qSort(cpy.begin(), cpy.end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.bpm < b.bpm; });

	healthstat[user].bpm_min = cpy.first().bpm;
	healthstat[user].bpm_mid = cpy.at(cpy.count() / 2).bpm;
	healthstat[user].bpm_max = cpy.last().bpm;
}

void MainWindow::createDocTablePage(int tablecount, int tablerows, int index, QTextCursor cursor, QTextBlockFormat bfmt_pbrk, QTextBlockFormat bfmt_cntr, QTextCharFormat cfmt_bold, QTextCharFormat cfmt_norm)
{
	QTextTable *table, *tables[tablecount];
	QTextTableFormat tfmt;
	QTextCharFormat cfmt_table, cfmt_red, cfmt_blk;
	QTextBlockFormat bfmt_table;
	QVector<QTextLength> constraints;
	int page = index / (tablecount * tablerows);

	tfmt.setHeaderRowCount(0);
	tfmt.setAlignment(Qt::AlignHCenter);
	tfmt.setCellPadding(15 - 2*TABLE_CORR);
	tfmt.setCellSpacing(0);
	tfmt.setBorderStyle(QTextFrameFormat::BorderStyle_None);
	cfmt_table.setFontWeight(QFont::Bold);
	cfmt_table.setFontPointSize(11);
	cfmt_red.setForeground(QColor("red"));
	cfmt_red.setFontWeight(QFont::Bold);
	cfmt_red.setFontPointSize(11);
	cfmt_blk.setFontPointSize(11);
	bfmt_table.setAlignment(Qt::AlignHCenter);
	cursor.insertBlock(bfmt_pbrk);
	bfmt_cntr.setBackground(QColor("lightGray"));
	cursor.insertBlock(bfmt_cntr);
	cursor.setCharFormat(cfmt_bold);
	cursor.insertText(tr("Overview - %1/%2").arg(page + 1).arg(ceil((double)exportdata.count() / tablecount / tablerows)) + "\n");
	cursor.setCharFormat(cfmt_norm);
	cursor.insertText(QString("%1 - %2").arg(rangeStart->dateTime().toString("dd.MM.yyyy")).arg(rangeStop->dateTime().toString("dd.MM.yyyy")));
	bfmt_cntr.setBackground(QColor("transparent"));

	table = cursor.insertTable(1, tablecount, tfmt);

	tfmt.setHeaderRowCount(1);
	tfmt.setAlignment(Qt::AlignHCenter);
	tfmt.setCellPadding(5 - TABLE_CORR);
	tfmt.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);

	constraints.append(QTextLength(QTextLength::FixedLength, 125 - 15*TABLE_CORR));								// fixme: calculate real width
	constraints.append(QTextLength(QTextLength::FixedLength, 60 - 7*TABLE_CORR));
	constraints.append(QTextLength(QTextLength::FixedLength, 40 - 5*TABLE_CORR));
	constraints.append(QTextLength(QTextLength::FixedLength, 40 - 5*TABLE_CORR));
	constraints.append(QTextLength(QTextLength::FixedLength, 40 - 5*TABLE_CORR));
	tfmt.setColumnWidthConstraints(constraints);

	for(int i = 0; i < tablecount; i++)
	{
		tables[i] = table->cellAt(0, i).firstCursorPosition().insertTable(TABLE_HEAD + tablerows, TABLE_COLS, tfmt);

		tables[i]->cellAt(0, 0).firstCursorPosition().setBlockFormat(bfmt_table);
		tables[i]->cellAt(0, 0).firstCursorPosition().insertText(tr("Date"), cfmt_table);
		tables[i]->cellAt(0, 1).firstCursorPosition().setBlockFormat(bfmt_table);
		tables[i]->cellAt(0, 1).firstCursorPosition().insertText(tr("Time"), cfmt_table);
		tables[i]->cellAt(0, 2).firstCursorPosition().setBlockFormat(bfmt_table);
		tables[i]->cellAt(0, 2).firstCursorPosition().insertText("SYS", cfmt_table);
		tables[i]->cellAt(0, 3).firstCursorPosition().setBlockFormat(bfmt_table);
		tables[i]->cellAt(0, 3).firstCursorPosition().insertText("DIA", cfmt_table);
		tables[i]->cellAt(0, 4).firstCursorPosition().setBlockFormat(bfmt_table);
		tables[i]->cellAt(0, 4).firstCursorPosition().insertText("BPM", cfmt_table);
	}

	for(int i = 0; i < tablecount * tablerows; i++)
	{
		if(pdlg->wasCanceled())
		{
			break;
		}

		if(i + page * tablecount * tablerows >= exportdata.count())
		{
			pdlg->setMaximum(tablecount * tablerows - 1);
			pdlg->setLabelText(tr("Adjustment %1/%2").arg(1 + i).arg(tablecount * tablerows));
			pdlg->setValue(i);

			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 0).firstCursorPosition().insertText(" ", cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 1).firstCursorPosition().insertText(" ", cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 2).firstCursorPosition().insertText(" ", cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 3).firstCursorPosition().insertText(" ", cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 4).firstCursorPosition().insertText(" ", cfmt_blk);
		}
		else
		{
			pdlg->setLabelText(tr("Record %1 of %2").arg(1 + i + page * tablecount * tablerows).arg(exportdata.count()));
			pdlg->setValue(i + page * tablecount * tablerows);

			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 0).firstCursorPosition().setBlockFormat(bfmt_table);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 0).firstCursorPosition().insertText(QDateTime::fromTime_t(exportdata.at(i + page * tablecount * tablerows).time).toString("ddd dd.MM.yy"), cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 1).firstCursorPosition().setBlockFormat(bfmt_table);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 1).firstCursorPosition().insertText(QDateTime::fromTime_t(exportdata.at(i + page * tablecount * tablerows).time).toString("hh:mm"), cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 2).firstCursorPosition().setBlockFormat(bfmt_table);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 2).firstCursorPosition().insertText(QString("%1").arg(exportdata.at(i + page * tablecount * tablerows).sys, 3, 10, QChar('0')), exportdata.at(i + page * tablecount * tablerows).sys > SYS_NORM ? cfmt_red : cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 3).firstCursorPosition().setBlockFormat(bfmt_table);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 3).firstCursorPosition().insertText(QString("%1").arg(exportdata.at(i + page * tablecount * tablerows).dia, 3, 10, QChar('0')), exportdata.at(i + page * tablecount * tablerows).dia > DIA_NORM ? cfmt_red : cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 4).firstCursorPosition().setBlockFormat(bfmt_table);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 4).firstCursorPosition().insertText(QString("%1").arg(exportdata.at(i + page * tablecount * tablerows).bpm, 3, 10, QChar('0')), exportdata.at(i + page * tablecount * tablerows).bpm > BPM_NORM ? cfmt_red : cfmt_blk);
		}
	}
}

void MainWindow::createDoc(QPrinter *printer)
{
	doc = new QTextDocument();
	QTextCursor cursor(doc);
	QTextBlockFormat bfmt_cntr, bfmt_pbrk;
	QTextCharFormat cfmt_bold, cfmt_norm;
	int fontid = QFontDatabase::addApplicationFont(":/ttf/ttf/larabie.ttf");
	int width = QApplication::desktop()->logicalDpiX() * printer->pageRect(QPrinter::Inch).width();
	int height = QApplication::desktop()->logicalDpiY() * printer->pageRect(QPrinter::Inch).height() - 100;		// fixme: subtract real header height
	int tablecount = printer->pageLayout().orientation() == QPageLayout::Landscape ? TABLES : TABLES - 1;		// fixme: calculate based on paper format
	int tablerows = printer->pageLayout().orientation() == QPageLayout::Landscape ? TABLE_ROWS : TABLE_ROWS + 11;

	pdlg = new QProgressDialog(tr("Diagram %1/2").arg(1), tr("Cancel"), 0, 2, this, Qt::Tool);
	pdlg->setWindowTitle(tr("Creating Document"));
	pdlg->setWindowModality(Qt::WindowModal);
	pdlg->setAutoClose(false);
	pdlg->setAutoReset(false);
	pdlg->setMinimumDuration(0);
	pdlg->setValue(1);
	pdlg->activateWindow();
	pdlg->move(mapToGlobal(rect().center()) - pdlg->rect().center());

	doc->setDefaultFont(QFont(QFontDatabase::applicationFontFamilies(fontid).at(0)));

	bfmt_cntr.setAlignment(Qt::AlignHCenter);
	bfmt_pbrk.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
	cfmt_bold.setFontWeight(QFont::Bold);
	cfmt_bold.setFontPointSize(18);
	cfmt_norm.setFontWeight(QFont::Normal);
	cfmt_norm.setFontPointSize(12);

	doc->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, new QCPDocumentObject(this));

	bfmt_cntr.setBackground(QColor("lightGray"));
	cursor.insertBlock(bfmt_cntr);
	cursor.setCharFormat(cfmt_bold);
	cursor.insertText(tr("Blood Pressure") + "\n");
	cursor.setCharFormat(cfmt_norm);
	cursor.insertText(QString("%1 - %2").arg(rangeStart->dateTime().toString("dd.MM.yyyy")).arg(rangeStop->dateTime().toString("dd.MM.yyyy")));
	bfmt_cntr.setBackground(QColor("transparent"));
	cursor.insertBlock(bfmt_cntr);
	cursor.insertText("\n");

	cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(widget_bp, width, height));

	cursor.insertBlock(bfmt_pbrk);

	pdlg->setLabelText(tr("Diagram %1/2").arg(2));
	pdlg->setValue(2);

	bfmt_cntr.setBackground(QColor("lightGray"));
	cursor.insertBlock(bfmt_cntr);
	cursor.setCharFormat(cfmt_bold);
	cursor.insertText(tr("Heart Rate") + "\n");
	cursor.setCharFormat(cfmt_norm);
	cursor.insertText(QString("%1 - %2").arg(rangeStart->dateTime().toString("dd.MM.yyyy")).arg(rangeStop->dateTime().toString("dd.MM.yyyy")));
	bfmt_cntr.setBackground(QColor("transparent"));
	cursor.insertBlock(bfmt_cntr);
	cursor.insertText("\n");

	cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(widget_hr, width, height));

	exportdata.clear();

	for(int i = 0; i < healthdata[user].count(); i++)
	{
		if(healthdata[user].at(i).time >= rangeStart->dateTime().toTime_t() && healthdata[user].at(i).time <= rangeStop->dateTime().toTime_t())
		{
			exportdata.append(healthdata[user].at(i));
		}
	}

	pdlg->setMaximum(exportdata.count());

	for(int i = 0; i < exportdata.count(); i += tablecount * tablerows)
	{
		if(pdlg->wasCanceled())
		{
			printer->setPrintRange(QPrinter::PageRange);
			printer->setFromTo(1, 2);

			break;
		}

		createDocTablePage(tablecount, tablerows, i, cursor, bfmt_pbrk, bfmt_cntr, cfmt_bold, cfmt_norm);
	}

	pdlg->close();

	printer->setDocName(tr("OBPM data export (%1 - %2)").arg(rangeStart->dateTime().toString("dd.MM.yyyy")).arg(rangeStop->dateTime().toString("dd.MM.yyyy")));

	doc->setPageSize(printer->pageRect().size());
	doc->print(printer);

	QFontDatabase::removeApplicationFont(fontid);
}

void MainWindow::importDataFromUSB(bool append)
{
	QString msg(tr("Successfully imported %1 records from USB:\n\n     User 1 = %2\n     User 2 = %3"));
	usbDialog *dlg = new usbDialog(this);
	QAction *active = NULL;
	int duplicate = 0;

	if(dlg->hids_found)
	{
		if(append)
		{
			active = groupUser->checkedAction();
		}
		else
		{
			healthdata[0].clear();
			healthdata[1].clear();

			buildGraph(0);
		}

		action_User1->setChecked(false);
		action_User2->setChecked(false);

		if(dlg->exec() == QDialog::Accepted)
		{
			if(healthdata[0].count())
			{
				qSort(healthdata[0].begin(), healthdata[0].end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.time < b.time; });

				for(int i = 0; i < healthdata[0].count() - 1; i++)
				{
					if(healthdata[0].at(i).time == healthdata[0].at(i + 1).time)
					{
						duplicate++;
						healthdata[0].remove(i--);
					}
				}

				getHealthStats(0);
			}

			if(healthdata[1].count())
			{
				qSort(healthdata[1].begin(), healthdata[1].end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.time < b.time; });

				for(int i = 0; i < healthdata[1].count() - 1; i++)
				{
					if(healthdata[1].at(i).time == healthdata[1].at(i + 1).time)
					{
						duplicate++;
						healthdata[1].remove(i--);
					}
				}

				getHealthStats(1);
			}

			if(append)
			{
				active->setChecked(true);
			}
			else
			{
				if(healthdata[0].count())
				{
					action_User1->setChecked(true);
				}
				else if(healthdata[1].count())
				{
					action_User2->setChecked(true);
				}
			}

			if(duplicate)
			{
				msg.append("\n\n" + tr("Skipped %1 duplicate entries!").arg(duplicate));
			}

			QMessageBox::information(this, APPNAME, msg.arg(dlg->user1 + dlg->user2).arg(dlg->user1).arg(dlg->user2));
		}
		else
		{
			QMessageBox::warning(this, APPNAME, tr("Import from usb device canceled!"));
		}
	}
	else
	{
		QMessageBox::critical(this, APPNAME, tr("No supported device (M500IT) found!\n\nCheck usb connection and try again..."));
	}
}

void MainWindow::importDataFromFile(QString filename, bool append)
{
	QString msg(tr("Successfully imported %1 records from CSV:\n\n     User 1 = %2\n     User 2 = %3"));
	QFile file(filename);
	QTextStream in(&file);
	HEALTHDATA set;
	int invalid = 0, duplicate = 0;
	QAction *active = NULL;

	if(append)
	{
		active = groupUser->checkedAction();
	}
	else
	{
		healthdata[0].clear();
		healthdata[1].clear();

		buildGraph(0);
	}

	action_User1->setChecked(false);
	action_User2->setChecked(false);

	if(file.open(QIODevice::ReadOnly))
	{
		QString line = in.readLine();

		while(!line.isNull())
		{
			QStringList values;

			line = in.readLine();

			if(!line.isEmpty())
			{
				values.append(line.split(';'));

				if(values.count() == 6)
				{
					user = values.at(0).toUInt() - 1;

					if(user > 1)
					{
						invalid++;
						continue;
					}

					set.time = QDateTime::fromString(values.at(1) + values.at(2), "dd.MM.yyhh:mm").addYears(100).toTime_t();
					set.sys = values.at(3).toUInt();
					set.dia = values.at(4).toUInt();
					set.bpm = values.at(5).toUInt();

					healthdata[user].append(set);
				}
				else
				{
					invalid++;
					continue;
				}
			}
		}

		file.close();

		if(healthdata[0].count())
		{
			qSort(healthdata[0].begin(), healthdata[0].end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.time < b.time; });

			for(int i = 0; i < healthdata[0].count() - 1; i++)
			{
				if(healthdata[0].at(i).time == healthdata[0].at(i + 1).time)
				{
					duplicate++;
					healthdata[0].remove(i--);
				}
			}

			getHealthStats(0);
		}

		if(healthdata[1].count())
		{
			qSort(healthdata[1].begin(), healthdata[1].end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.time < b.time; });

			for(int i = 0; i < healthdata[1].count() - 1; i++)
			{
				if(healthdata[1].at(i).time == healthdata[1].at(i + 1).time)
				{
					duplicate++;
					healthdata[1].remove(i--);
				}
			}

			getHealthStats(1);
		}

		if(append)
		{
			active->setChecked(true);
		}
		else
		{
			if(healthdata[0].count())
			{
				action_User1->setChecked(true);
			}
			else if(healthdata[1].count())
			{
				action_User2->setChecked(true);
			}
		}

		if(!healthdata[0].count() && !healthdata[1].count())
		{
			QMessageBox::warning(this, APPNAME, tr("No valid records in imported CSV found!\n\nPlease check line format of file: %1").arg("\n\n<user>;<date>;<time>;<sys>;<dia>;<pulse>"));

			return;
		}
		else
		{
			if(invalid)
			{
				msg.append("\n\n" + tr("Skipped %1 invalid entries!").arg(invalid));

			}

			if(duplicate)
			{
				msg.append("\n\n" + tr("Skipped %1 duplicate entries!").arg(duplicate));
			}

			QMessageBox::information(this, APPNAME, msg.arg(healthdata[0].count() + healthdata[1].count()).arg(healthdata[0].count()).arg(healthdata[1].count()));
		}
	}
	else
	{
		QMessageBox::critical(this, APPNAME, tr("Could not open \"%1\"!\n\nReason: %2").arg(filename).arg(file.errorString()));
	}
}

void MainWindow::exportDataToCSV(QString filename)
{
	QFile file(filename);
	QString header("user;date;time;sys;dia;pulse\n");

	if(file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		file.write(header.toUtf8(), header.length());

		for(int i = 0; i < healthdata[0].count(); i++)
		{
			file.write(QString("1;%1;%2;%3;%4\n").arg(QDateTime::fromTime_t(healthdata[0].at(i).time).toString("dd.MM.yy;hh:mm")).arg(healthdata[0].at(i).sys).arg(healthdata[0].at(i).dia, 3, 10, QChar('0')).arg(healthdata[0].at(i).bpm, 3, 10, QChar('0')).toUtf8(), 28 + 1);
		}

		for(int i = 0; i < healthdata[1].count(); i++)
		{
			file.write(QString("2;%1;%2;%3;%4\n").arg(QDateTime::fromTime_t(healthdata[1].at(i).time).toString("dd.MM.yy;hh:mm")).arg(healthdata[1].at(i).sys).arg(healthdata[1].at(i).dia, 3, 10, QChar('0')).arg(healthdata[1].at(i).bpm, 3, 10, QChar('0')).toUtf8(), 28 + 1);
		}

		file.close();

		if(QMessageBox::question(this, APPNAME, tr("Successfully exported %1 records to CSV:\n\n     User 1 = %2\n     User 2 = %3\n\nOpen now with default app?").arg(healthdata[0].count() + healthdata[1].count()).arg(healthdata[0].count()).arg(healthdata[1].count()), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
		{
			QDesktopServices::openUrl(QUrl("file:///" + filename));
		}
	}
	else
	{
		QMessageBox::critical(this, APPNAME, tr("Could not create \"%1\"!\n\nReason: %2").arg(filename).arg(file.errorString()));
	}
}

void MainWindow::exportDataToPDF(QString filename)
{
	QPrinter *printer = new QPrinter(QPrinter::ScreenResolution);

	printer->setOutputFileName(filename);
	printer->setOutputFormat(QPrinter::PdfFormat);
	printer->setPaperSize(QPrinter::A4);
	printer->setPageSize(QPrinter::A4);
	printer->setPageOrientation(QPageLayout::Landscape);
//	printer->setFullPage(true);

	createDoc(printer);

	if(QMessageBox::question(this, APPNAME, tr("Successfully exported current view to PDF.\n\nOpen now with default app?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		QDesktopServices::openUrl(QUrl("file:///" + filename));
	}
}

void MainWindow::on_action_Exit_triggered()
{
	close();
}

void MainWindow::on_action_importFromUSB_triggered()
{
	if((healthdata[0].count() || healthdata[1].count()) && QMessageBox::question(this, APPNAME, tr("Append new data to existing records?")) == QMessageBox::Yes)
	{
		importDataFromUSB(true);
	}
	else
	{
		importDataFromUSB(false);
	}
}

void MainWindow::on_action_importFromFile_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Import from CSV"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + "export.csv", tr("CSV Document (*.csv)"), 0, QFileDialog::DontUseNativeDialog);

	if(!filename.isEmpty())
	{
		if((healthdata[0].count() || healthdata[1].count()) && QMessageBox::question(this, APPNAME, tr("Append new data to existing records?")) == QMessageBox::Yes)
		{
			importDataFromFile(filename, true);
		}
		else
		{
			importDataFromFile(filename, false);
		}
	}
}

void MainWindow::on_action_exportToCSV_triggered()
{
	if(healthdata[0].count() || healthdata[1].count())
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + QString("export-%1.csv").arg(QDateTime::currentDateTime().date().toString("yyyyMMdd")), tr("CSV Document (*.csv)"), 0, QFileDialog::DontUseNativeDialog);

		if(!filename.isEmpty())
		{
			exportDataToCSV(filename);
		}
	}
	else
	{
		QMessageBox::warning(this, APPNAME, tr("No records to export!"));
	}
}

void MainWindow::on_action_exportToPDF_triggered()
{
	if(healthdata[user].count())
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Export to PDF"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + QString("export-%1-%2-%3.pdf").arg(1 + user).arg(rangeStart->dateTime().toString("yyyyMMdd"), rangeStop->dateTime().toString("yyyyMMdd")), tr("PDF Document (*.pdf)"), 0, QFileDialog::DontUseNativeDialog);

		if(!filename.isEmpty())
		{
			exportDataToPDF(filename);
		}
	}
	else
	{
		QMessageBox::warning(this, APPNAME, tr("No records to export!"));
	}
}

void MainWindow::on_action_PrintPreview_triggered()
{
	if(healthdata[user].count())
	{
		QPrintPreviewDialog printPreviewDialog(this, Qt::Tool);

		printPreviewDialog.printer()->setPaperSize(QPrinter::A4);
		printPreviewDialog.printer()->setOrientation(QPrinter::Landscape);

		connect(&printPreviewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(createDoc(QPrinter*)));

		printPreviewDialog.exec();
	}
	else
	{
		QMessageBox::warning(this, APPNAME, tr("No records to print!"));
	}
}

void MainWindow::on_action_Print_triggered()
{
	if(healthdata[user].count())
	{
		QPrintDialog printDialog(this);

		printDialog.printer()->setPaperSize(QPrinter::A4);
		printDialog.printer()->setOrientation(QPrinter::Landscape);

		if(printDialog.exec() == QDialog::Accepted)
		{
			createDoc(printDialog.printer());
		}
	}
	else
	{
		QMessageBox::warning(this, APPNAME, tr("No records to print!"));
	}
}

void MainWindow::on_action_Help_triggered()
{
	QMessageBox::information(this, APPNAME, tr("Help is not yet implemented..."));
}

void MainWindow::on_action_About_triggered()
{
	new aboutDialog(this);
}

void MainWindow::on_action_showHideLegend_toggled(bool state)
{
	widget_bp->legend->setVisible(state);
	widget_hr->legend->setVisible(state);

	widget_bp->replot();
	widget_hr->replot();
}

void MainWindow::on_action_resetZoom_triggered()
{
	if(healthdata[user].count())
	{
		rangeStart->setDate(QDateTime::fromTime_t(healthdata[user].first().time - tdiff).date());
		rangeStop->setDate(QDateTime::fromTime_t(healthdata[user].last().time + tdiff).date());

		widget_bp->xAxis->setRange(healthdata[user].first().time - tdiff, healthdata[user].last().time + tdiff);
		widget_bp->replot();
	}
	else
	{
		rangeStart->setDate(QDateTime::currentDateTime().date());
		rangeStop->setDate(QDateTime::currentDateTime().date());

		widget_bp->xAxis->setRange(rangeStart->dateTime().toTime_t(), rangeStop->dateTime().toTime_t());
	}
}

void MainWindow::on_action_User1_toggled(bool enabled)
{
	if(enabled)
	{
		user = 0;

		buildGraph(user);
	}
}

void MainWindow::on_action_User2_toggled(bool enabled)
{
	if(enabled)
	{
		user = 1;

		buildGraph(user);
	}
}

void MainWindow::buildGraph(bool user)
{
	widget_bp->graph(0)->clearData();
	widget_bp->graph(1)->clearData();
	widget_hr->graph(0)->clearData();

	if(healthdata[user].count())
	{
		rangeStart->setDateTimeRange(QDateTime(QDateTime::fromTime_t(healthdata[user].first().time).date(), QTime(0, 0, 0, 0)), QDateTime(QDateTime::fromTime_t(healthdata[user].last().time).date(), QTime(23, 59, 59, 999)));
		rangeStop->setDateTimeRange(QDateTime(QDateTime::fromTime_t(healthdata[user].first().time).date(), QTime(0, 0, 0, 0)), QDateTime(QDateTime::fromTime_t(healthdata[user].last().time).date(), QTime(23, 59, 59, 999)));
		rangeStart->setDateTime(QDateTime(QDateTime::fromTime_t(healthdata[user].first().time).date(), QTime(0, 0, 0, 0)));
		rangeStop->setDateTime(QDateTime(QDateTime::fromTime_t(healthdata[user].last().time).date(), QTime(23, 59, 59, 999)));

		widget_bp->xAxis->setRange(healthdata[user].first().time - tdiff, healthdata[user].last().time + tdiff);
		widget_bp->yAxis->setRange(healthstat[user].dia_min - 10, healthstat[user].sys_max + 10);
		widget_hr->xAxis->setRange(healthdata[user].first().time - tdiff, healthdata[user].last().time + tdiff);
		widget_hr->yAxis->setRange(healthstat[user].bpm_min - 5, healthstat[user].bpm_max + 5);

		widget_bp->plottable(0)->setName(QString("SYS: %1 [%2] %3").arg(healthstat[user].sys_min).arg(healthstat[user].sys_mid).arg(healthstat[user].sys_max));
		widget_bp->plottable(1)->setName(QString("DIA: %1 [%2] %3").arg(healthstat[user].dia_min).arg(healthstat[user].dia_mid).arg(healthstat[user].dia_max));
		widget_hr->plottable(0)->setName(QString(tr("Pulse: %1 [%2] %3")).arg(healthstat[user].bpm_min).arg(healthstat[user].bpm_mid).arg(healthstat[user].bpm_max));

		for(int i = 0; i < healthdata[user].count(); i++)
		{
			widget_bp->graph(0)->addData(healthdata[user].at(i).time, healthdata[user].at(i).sys);
			widget_bp->graph(1)->addData(healthdata[user].at(i).time, healthdata[user].at(i).dia);
			widget_hr->graph(0)->addData(healthdata[user].at(i).time, healthdata[user].at(i).bpm);
		}
	}
	else
	{
		widget_bp->plottable(0)->setName("SYS");
		widget_bp->plottable(1)->setName("DIA");
		widget_hr->plottable(0)->setName(tr("Pulse"));

		rangeStart->clearMinimumDateTime();
		rangeStart->clearMaximumDateTime();
		rangeStop->clearMinimumDateTime();
		rangeStop->clearMaximumDateTime();
		rangeStart->setDateTime(QDateTime(QDateTime::currentDateTime().date(), QTime(0, 0, 0, 0)));
		rangeStop->setDateTime(QDateTime(QDateTime::currentDateTime().date(), QTime(23, 59, 59, 999)));

		widget_bp->xAxis->setRange(rangeStart->dateTime().toTime_t(), rangeStop->dateTime().toTime_t());
	}

	widget_bp->replot();
	widget_hr->replot();
}

void MainWindow::dateChanged()
{
	widget_bp->xAxis->setRange(rangeStart->dateTime().toTime_t(), rangeStop->dateTime().toTime_t());
	widget_bp->replot();
}

void MainWindow::xAxisBPChanged(QCPRange range)
{
	widget_hr->xAxis->setRange(range);
	widget_hr->replot();
}

void MainWindow::xAxisHRChanged(QCPRange range)
{
	widget_bp->xAxis->setRange(range);
	widget_bp->replot();
}

void MainWindow::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_F1)
	{
			QMessageBox::information(this, APPNAME, tr("Help is not yet implemented..."));
	}
	else if(ke->key() == Qt::Key_Escape)
	{
		close();
	}

	QMainWindow::keyPressEvent(ke);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *me)
{
	if(me->button() == Qt::LeftButton && sender())
	{
		int x = ((QCustomPlot*)sender())->xAxis->pixelToCoord(me->pos().x() - 5);

		if(((QCustomPlot*)sender())->plottableAt(me->pos()))
		{
			QCPData sys = widget_bp->graph(0)->data()->lowerBound(x).value();
			QCPData dia = widget_bp->graph(1)->data()->lowerBound(x).value();
			QCPData bpm = widget_hr->graph(0)->data()->lowerBound(x).value();

			if(QDateTime::fromTime_t(bpm.key).date().year() == 1970)
			{
				sys = widget_bp->graph(0)->data()->last();
				dia = widget_bp->graph(1)->data()->last();
				bpm = widget_hr->graph(0)->data()->last();
			}

			QToolTip::showText(me->globalPos(), QString("%1\nSYS %2 / DIA %3 / BPM %4").arg(QDateTime::fromTime_t(bpm.key).toString("dddd, dd.MM.yyyy hh:mm")).arg(sys.value).arg(dia.value).arg(bpm.value));
		}
	}
	else if(me->button() == Qt::MiddleButton)
	{
		on_action_resetZoom_triggered();
	}
	else if(me->button() == Qt::RightButton)
	{
		QMenu menu;
		QActionGroup *group = new QActionGroup(this);

		menu.addAction(action_showHideLegend);
		menu.addSeparator();
		menu.addAction(action_stylePoint);
		menu.addAction(action_styleLine);
		menu.addAction(action_styleStep);
		menu.addAction(action_styleImpulse);
		menu.addSeparator();
		menu.addAction(action_resetZoom);

		group->addAction(action_stylePoint);
		group->addAction(action_styleLine);
		group->addAction(action_styleStep);
		group->addAction(action_styleImpulse);

		QAction* selectedItem = menu.exec(me->globalPos());

		if(selectedItem)
		{
			enum QCPGraph::LineStyle lineStyle;

			if(selectedItem->objectName() == "action_stylePoint")
			{
				lineStyle = QCPGraph::lsNone;
			}
			else if(selectedItem->objectName() == "action_styleLine")
			{
				lineStyle = QCPGraph::lsLine;
			}
			else if(selectedItem->objectName() == "action_styleStep")
			{
				lineStyle = QCPGraph::lsStepCenter;
			}
			else if(selectedItem->objectName() == "action_styleImpulse")
			{
				lineStyle = QCPGraph::lsImpulse;
			}
			else
			{
				return;
			}

			widget_bp->graph(0)->setLineStyle(lineStyle);
			widget_bp->graph(1)->setLineStyle(lineStyle);
			widget_hr->graph(0)->setLineStyle(lineStyle);

			widget_bp->replot();
			widget_hr->replot();
		}
	}
}

void MainWindow::closeEvent(QCloseEvent *ce)
{
	if(QMessageBox::question(this, APPNAME, tr("Really exit program?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		ce->accept();
	}
	else
	{
		ce->ignore();
	}
}
