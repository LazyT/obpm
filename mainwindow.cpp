#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow mw;

	mw.show();

	return app.exec();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	QApplication::setStyle("Fusion");

	QNetworkProxyFactory::setUseSystemConfiguration(true);

	mgr = new QNetworkAccessManager(this);

	connect(mgr, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), SLOT(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));

	if(!QLocale::system().name().startsWith("en_"))
	{
		if(appTranslator.load("obpm_" + QLocale::system().name(), QApplication::applicationDirPath() + "/lng"))
		{
			QApplication::installTranslator(&appTranslator);

			if(baseTranslator.load("qtbase_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
			{
				QApplication::installTranslator(&baseTranslator);
			}
			else if(baseTranslator.load("qtbase_" + QLocale::system().name(), QApplication::applicationDirPath() + "/lng"))
			{
				QApplication::installTranslator(&baseTranslator);
			}

			if(helpTranslator.load("qt_help_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
			{
				QApplication::installTranslator(&helpTranslator);
			}
			else if(helpTranslator.load("qt_help_" + QLocale::system().name(), QApplication::applicationDirPath() + "/lng"))
			{
				QApplication::installTranslator(&helpTranslator);
			}
		}
	}

	setupUi(this);

	getConfig();

	help = NULL;
	user = 0;
	update = false;

	QMenu *menu = new QMenu(this);
	menu->addAction(action_PrintPreview);
	((QToolButton*)mainToolBar->widgetForAction(action_Print))->setMenu(menu);
	((QToolButton*)mainToolBar->widgetForAction(action_Print))->setPopupMode(QToolButton::MenuButtonPopup);

	groupUser = new QActionGroup(this);
	groupUser->addAction(action_User1);
	groupUser->addAction(action_User2);

	action_User1->setToolTip(tr("show %1").arg(cfg.alias1));
	action_User2->setToolTip(tr("show %1").arg(cfg.alias2));
	action_User1->setStatusTip(tr("show %1").arg(cfg.alias1));
	action_User2->setStatusTip(tr("show %1").arg(cfg.alias2));

	lcd = new QLCDNumber(1);
	lcd->setSegmentStyle(QLCDNumber::Flat);
	lcd->setToolTip(tr("record count"));
	lcd->setStatusTip(tr("record count"));

	rangeStart = new QDateTimeEdit(QDateTime(QDate::currentDate(), QTime(0, 0, 0, 0)), this);
	rangeStop = new QDateTimeEdit(QDateTime(QDate::currentDate(), QTime(23, 59, 59, 999)), this);
	rangeStart->setToolTip(tr("start analysis"));
	rangeStop->setToolTip(tr("stop analysis"));
	rangeStart->setStatusTip(tr("start analysis"));
	rangeStop->setStatusTip(tr("stop analysis"));
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

	filter = new QPushButton(this);
	filter->setToolTip(tr("filter analysis"));
	filter->setStatusTip(tr("filter analysis"));
	filter->setCheckable(true);
	filter->setIcon(QIcon(":/png/png/filter.png"));

	mainToolBar->insertWidget(action_User2, lcd);
	mainToolBar->addWidget(rangeStart);
	mainToolBar->addWidget(filter);
	mainToolBar->addWidget(rangeStop);

	widget_bp->yAxis->setLabel("mmHg");
	widget_hr->yAxis->setLabel("bpm");

	line_sys = new QCPItemStraightLine(widget_bp);
	line_dia = new QCPItemStraightLine(widget_bp);
	line_bpm = new QCPItemStraightLine(widget_hr);
	line_sys->setPen(QPen(Qt::green));
	line_dia->setPen(QPen(Qt::green));
	line_bpm->setPen(QPen(Qt::green));
	line_sys->point1->setTypeX(QCPItemPosition::ptAbsolute);
	line_sys->point2->setTypeX(QCPItemPosition::ptAbsolute);
	line_dia->point1->setTypeX(QCPItemPosition::ptAbsolute);
	line_dia->point2->setTypeX(QCPItemPosition::ptAbsolute);
	line_bpm->point1->setTypeX(QCPItemPosition::ptAbsolute);
	line_bpm->point2->setTypeX(QCPItemPosition::ptAbsolute);
	line_sys->point1->setCoords(0, cfg.sys);
	line_sys->point2->setCoords(1, cfg.sys);
	line_dia->point1->setCoords(0, cfg.dia);
	line_dia->point2->setCoords(1, cfg.dia);
	line_bpm->point1->setCoords(0, cfg.bpm);
	line_bpm->point2->setCoords(1, cfg.bpm);
	widget_bp->addItem(line_sys);
	widget_bp->addItem(line_dia);
	widget_hr->addItem(line_bpm);

	widget_bp->addGraph();
	widget_bp->addGraph();
	widget_bp->graph(0)->setPen(QPen(Qt::magenta));
	widget_bp->graph(1)->setPen(QPen(Qt::blue));
	widget_bp->graph(0)->setScatterStyle(QCPScatterStyle(QPixmap(":/png/png/sys.png")));
	widget_bp->graph(1)->setScatterStyle(QCPScatterStyle(QPixmap(":/png/png/dia.png")));
	widget_bp->graph(0)->setLineStyle((QCPGraph::LineStyle)cfg.style);
	widget_bp->graph(1)->setLineStyle((QCPGraph::LineStyle)cfg.style);
	widget_bp->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	widget_bp->xAxis->setDateTimeFormat("hh:mm\ndd.MM.yy");
	widget_bp->yAxis->setRange(cfg.dia - 15, cfg.sys + 15);
	widget_bp->yAxis->setAutoTickStep(false);
	widget_bp->yAxis->setTickStep(10);
	widget_bp->plottable(0)->setName("SYS");
	widget_bp->plottable(1)->setName("DIA");
	widget_bp->legend->setVisible(cfg.legend);
	widget_bp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	widget_bp->axisRect(0)->setRangeDrag(Qt::Horizontal);
	widget_bp->axisRect(0)->setRangeZoom(Qt::Horizontal);

	widget_hr->addGraph();
	widget_hr->graph(0)->setPen(QPen(Qt::red));
	widget_hr->graph(0)->setScatterStyle(QCPScatterStyle(QPixmap(":/png/png/bpm.png")));
	widget_hr->graph(0)->setLineStyle((QCPGraph::LineStyle)cfg.style);
	widget_hr->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	widget_hr->xAxis->setDateTimeFormat("hh:mm\ndd.MM.yy");
	widget_hr->yAxis->setRange(cfg.bpm - 20, cfg.bpm);
	widget_hr->yAxis->setAutoTickStep(false);
	widget_hr->yAxis->setTickStep(5);
	widget_hr->plottable(0)->setName(tr("Pulse"));
	widget_hr->legend->setVisible(cfg.legend);
	widget_hr->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	widget_hr->axisRect(0)->setRangeDrag(Qt::Horizontal);
	widget_hr->axisRect(0)->setRangeZoom(Qt::Horizontal);

	connect(rangeStart, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(dateChanged()));
	connect(rangeStop, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(dateChanged()));
	connect(filter, SIGNAL(clicked(bool)), this, SLOT(filterChanged(bool)));
	connect(widget_bp, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
	connect(widget_hr, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
	connect(widget_bp->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisBPChanged(QCPRange)));
	connect(widget_hr->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisHRChanged(QCPRange)));

	widget_bp->installEventFilter(this);
	widget_hr->installEventFilter(this);

	dateChanged();

	if(QFile::exists(cfg.database))
	{
		importDataFromSQL(cfg.database, false, false);
	}

	if(cfg.update)
	{
		QTimer::singleShot(250, this, SLOT(checkUpdate()));
	}

	if(cfg.smain.width() == -1 || cfg.smain.height() == -1)
	{
		move(QApplication::desktop()->screen()->rect().center() - rect().center());
	}
	else
	{
		move(cfg.pmain);
		resize(cfg.smain);
	}
}

void MainWindow::proxyAuthenticationRequired(__attribute__ ((unused)) const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
	loginDialog(this, authenticator);
}

void MainWindow::getConfig()
{
	QSettings ini(CFG, QSettings::IniFormat);

	cfg.hem7322u = ini.value("HEM7322U", false).toBool();
	cfg.update = ini.value("Update", true).toBool();
	cfg.legend = ini.value("Legend", true).toBool();
	cfg.psd = ini.value("PrintSingleDiagram", true).toBool();
	cfg.pot = ini.value("PrintOverviewTable", false).toBool();
	cfg.style = ini.value("Style", QCPGraph::lsLine).toInt();
	cfg.sys = ini.value("SYS", SYS_NORM).toInt();
	cfg.dia = ini.value("DIA", DIA_NORM).toInt();
	cfg.bpm = ini.value("BPM", BPM_NORM).toInt();
	cfg.alias1 = ini.value("Alias1", tr("User 1")).toString();
	cfg.alias2 = ini.value("Alias2", tr("User 2")).toString();
	cfg.database = ini.value("Database", DB).toString();

	ini.beginGroup("MainWindow");
	cfg.pmain = ini.value("Position", QPoint(0, 0)).toPoint();
	cfg.smain = ini.value("Size", QSize(-1, -1)).toSize();
	ini.endGroup();

	ini.beginGroup("HelpWindow");
	cfg.phelp = ini.value("Position", QPoint(0, 0)).toPoint();
	cfg.shelp = ini.value("Size", QSize(-1, -1)).toSize();
	ini.endGroup();
}

void MainWindow::setConfig()
{
	QSettings ini(CFG, QSettings::IniFormat);

	ini.setValue("HEM7322U", cfg.hem7322u);
	ini.setValue("Update", cfg.update);
	ini.setValue("Legend", cfg.legend);
	ini.setValue("PrintSingleDiagram", cfg.psd);
	ini.setValue("PrintOverviewTable", cfg.pot);
	ini.setValue("Style", cfg.style);
	ini.setValue("SYS", cfg.sys);
	ini.setValue("DIA", cfg.dia);
	ini.setValue("BPM", cfg.bpm);
	ini.setValue("Alias1", cfg.alias1);
	ini.setValue("Alias2", cfg.alias2);
	ini.setValue("Database", cfg.database);

	ini.beginGroup("MainWindow");
	ini.setValue("Position", cfg.pmain);
	ini.setValue("Size", cfg.smain);
	ini.endGroup();

	ini.beginGroup("HelpWindow");
	ini.setValue("Position", cfg.phelp);
	ini.setValue("Size", cfg.shelp);
	ini.endGroup();

	ini.sync();
}

void MainWindow::checkUpdate()
{
	QNetworkReply *rep = mgr->get(QNetworkRequest(QUrl("https://lazyt.github.io/obpm/updater/update.xml")));
	QByteArray raw;
	QXmlStreamReader xml;
	QString release, version, date, changelog;

	rep->ignoreSslErrors();

	timeout.start();

	while(rep->isRunning())
	{
		QCoreApplication::processEvents();

		if(timeout.hasExpired(3000))
		{
			rep->abort();
		}
	}

	if(rep->error())
	{
		QMessageBox::warning(this, APPNAME, tr("Failed to contact online updater!\n\n%1").arg(rep->errorString()));

		return;
	}

	raw = rep->readAll();

	rep->deleteLater();

	xml.addData(raw);

	while(!xml.atEnd())
	{
		xml.readNext();

		if(xml.isStartElement() && xml.name() == "online-updater")
		{
			while(xml.readNext() != QXmlStreamReader::Invalid && xml.name() != "online-updater")
			{
				if(xml.name() == "release")
				{
					release = xml.readElementText();
				}
				else if(xml.name() == "version")
				{
					version = xml.readElementText();
				}
				else if(xml.name() == "date")
				{
					date = xml.readElementText();
				}
				else if(xml.isStartElement() && xml.name() == "changes")
				{
					while(xml.readNext() != QXmlStreamReader::Invalid && xml.name() != "changes")
					{
						if(xml.name() == "en")
						{
							changelog = xml.readElementText();
						}
						else if(xml.name() == QLocale::system().name().mid(0, 2))
						{
							changelog = xml.readElementText();
						}
					}
				}
			}
		}
	}

	if(xml.hasError())
	{
		QMessageBox::warning(this, APPNAME, tr("Failed to analyze online updater!\n\n%1").arg(xml.errorString()));

		return;
	}

	if(APPRELS < release)
	{
		updateDialog dlg(this, version, date, changelog);

		dlg.exec();

		if(dlg.result() == QDialog::Accepted)
		{
			qputenv("OBPM_INSTALLER_MODE", "SILENT");

			if(!QProcess::startDetached(UPD + " --updater"))
			{
				QMessageBox::warning(this, APPNAME, tr("Failed to start the installer!\n\nPlease update manually."));
			}
			else
			{
				update = true;

				close();
			}
		}
	}
}

void MainWindow::getHealthStats(QVector <HEALTHDATA> data, HEALTHSTAT *stat)
{
	QVector <HEALTHDATA> cpy(data);

	qSort(cpy.begin(), cpy.end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.sys < b.sys; });

	stat->sys_min = cpy.first().sys;
	stat->sys_mid = cpy.at(cpy.count() / 2).sys;
	stat->sys_max = cpy.last().sys;

	qSort(cpy.begin(), cpy.end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.dia < b.dia; });

	stat->dia_min = cpy.first().dia;
	stat->dia_mid = cpy.at(cpy.count() / 2).dia;
	stat->dia_max = cpy.last().dia;

	qSort(cpy.begin(), cpy.end(), [](const HEALTHDATA &a, const HEALTHDATA &b) { return a.bpm < b.bpm; });

	stat->bpm_min = cpy.first().bpm;
	stat->bpm_mid = cpy.at(cpy.count() / 2).bpm;
	stat->bpm_max = cpy.last().bpm;
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
	cursor.insertText(tr("Overview - %1 %2/%3").arg(user ? cfg.alias2 : cfg.alias1).arg(page + 1).arg(ceil((double)exportdata.count() / tablecount / tablerows)) + "\n");
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
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 2).firstCursorPosition().insertText(QString("%1").arg(exportdata.at(i + page * tablecount * tablerows).sys, 3, 10, QChar('0')), exportdata.at(i + page * tablecount * tablerows).sys > cfg.sys ? cfmt_red : cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 3).firstCursorPosition().setBlockFormat(bfmt_table);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 3).firstCursorPosition().insertText(QString("%1").arg(exportdata.at(i + page * tablecount * tablerows).dia, 3, 10, QChar('0')), exportdata.at(i + page * tablecount * tablerows).dia > cfg.dia ? cfmt_red : cfmt_blk);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 4).firstCursorPosition().setBlockFormat(bfmt_table);
			tables[i / tablerows]->cellAt(TABLE_HEAD + i % tablerows, 4).firstCursorPosition().insertText(QString("%1").arg(exportdata.at(i + page * tablecount * tablerows).bpm, 3, 10, QChar('0')), exportdata.at(i + page * tablecount * tablerows).bpm > cfg.bpm ? cfmt_red : cfmt_blk);
		}
	}
}

void MainWindow::createDoc(QPrinter *printer)
{
	QTextDocument *doc = new QTextDocument();
	QTextCursor cursor(doc);
	QTextBlockFormat bfmt_cntr, bfmt_pbrk;
	QTextCharFormat cfmt_bold, cfmt_norm;
	int fontid = QFontDatabase::addApplicationFont(":/ttf/ttf/larabie.ttf");
	int width = QApplication::desktop()->logicalDpiX() * printer->pageRect(QPrinter::Inch).width();
	int height = QApplication::desktop()->logicalDpiY() * printer->pageRect(QPrinter::Inch).height() - 100;		// fixme: subtract real header height
	int tablecount = printer->pageLayout().orientation() == QPageLayout::Landscape ? TABLES : TABLES - 1;		// fixme: calculate based on paper format
	int tablerows = printer->pageLayout().orientation() == QPageLayout::Landscape ? TABLE_ROWS : TABLE_ROWS + 11;

	pdlg = cfg.psd ? new QProgressDialog(tr("Diagram"), tr("Cancel"), 0, 1, this, Qt::Tool) : new QProgressDialog(tr("Diagram %1/2").arg(1), tr("Cancel"), 0, 2, this, Qt::Tool);
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

	if(cfg.psd)
	{
		bfmt_cntr.setBackground(QColor("lightGray"));
		cursor.insertBlock(bfmt_cntr);
		cursor.setCharFormat(cfmt_bold);
		cursor.insertText(tr("Blood Pressure & Heart Rate - %1").arg(user ? cfg.alias2 : cfg.alias1) + "\n");
		cursor.setCharFormat(cfmt_norm);
		cursor.insertText(QString("%1 - %2").arg(rangeStart->dateTime().toString("dd.MM.yyyy")).arg(rangeStop->dateTime().toString("dd.MM.yyyy")));
		bfmt_cntr.setBackground(QColor("transparent"));
		cursor.insertBlock(bfmt_cntr);
		cursor.insertText("\n");

		cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(widget_bp, width, height/2));
		cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(widget_hr, width, height/2));
	}
	else
	{
		bfmt_cntr.setBackground(QColor("lightGray"));
		cursor.insertBlock(bfmt_cntr);
		cursor.setCharFormat(cfmt_bold);
		cursor.insertText(tr("Blood Pressure - %1").arg(user ? cfg.alias2 : cfg.alias1) + "\n");
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
		cursor.insertText(tr("Heart Rate - %1").arg(user ? cfg.alias2 : cfg.alias1)  + "\n");
		cursor.setCharFormat(cfmt_norm);
		cursor.insertText(QString("%1 - %2").arg(rangeStart->dateTime().toString("dd.MM.yyyy")).arg(rangeStop->dateTime().toString("dd.MM.yyyy")));
		bfmt_cntr.setBackground(QColor("transparent"));
		cursor.insertBlock(bfmt_cntr);
		cursor.insertText("\n");

		cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(widget_hr, width, height));
	}

	if(cfg.pot)
	{
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
			createDocTablePage(tablecount, tablerows, i, cursor, bfmt_pbrk, bfmt_cntr, cfmt_bold, cfmt_norm);

			if(pdlg->wasCanceled())
			{
				printer->setPrintRange(QPrinter::PageRange);
				printer->setFromTo(1, cfg.psd ? 1 : 2);

				break;
			}
		}
	}

	pdlg->close();
	delete pdlg;

	printer->setDocName(tr("OBPM data export (%1 - %2)").arg(rangeStart->dateTime().toString("dd.MM.yyyy")).arg(rangeStop->dateTime().toString("dd.MM.yyyy")));

	doc->setPageSize(printer->pageRect().size());
	doc->print(printer);

	QFontDatabase::removeApplicationFont(fontid);
}

int MainWindow::validateData(QAction *active, bool append)
{
	int duplicate = 0;

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

		getHealthStats(healthdata[0], &healthstat[0]);
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

		getHealthStats(healthdata[1], &healthstat[1]);
	}

	if(append)
	{
		active->setChecked(true);
	}
	else
	{
		if(healthdata[0].count() || !healthdata[1].count())
		{
			action_User1->setChecked(true);
		}
		else if(healthdata[1].count())
		{
			action_User2->setChecked(true);
		}
	}

	return duplicate;
}

void MainWindow::importDataFromUSB(bool append)
{
	QString msg(tr("Successfully imported %1 records from USB:\n\n     %2 = %3\n     %4 = %5"));
	usbDialog *dlg = new usbDialog(this, cfg.hem7322u);
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

			buildGraph(healthdata[user], healthstat[user]);
		}

		action_User1->setChecked(false);
		action_User2->setChecked(false);

		if(dlg->exec() == QDialog::Accepted)
		{
			duplicate = validateData(active, append);

			if(duplicate)
			{
				msg.append("\n\n" + tr("Skipped %1 duplicate entries!").arg(duplicate));
			}

			QMessageBox::information(this, APPNAME, msg.arg(dlg->user1 + dlg->user2).arg(cfg.alias1).arg(dlg->user1).arg(cfg.alias2).arg(dlg->user2));
		}
		else
		{
			QMessageBox::warning(this, APPNAME, tr("Import from usb device canceled!"));
		}
	}
	else
	{
		QMessageBox::critical(this, APPNAME, tr("No supported device found!\n\nCheck usb connection and try again..."));
	}
}

void MainWindow::importDataFromCSV(QString filename, bool append)
{
	QString msg(tr("Successfully imported %1 records from CSV:\n\n     %2 = %3\n     %4 = %5"));
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

		buildGraph(healthdata[user], healthstat[user]);
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

				if(values.count() >= 6)
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

					if(values.count() > 6)
					{
						set.msg = values.at(6);
					}

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

		duplicate = validateData(active, append);

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

			QMessageBox::information(this, APPNAME, msg.arg(healthdata[0].count() + healthdata[1].count()).arg(cfg.alias1).arg(healthdata[0].count()).arg(cfg.alias2).arg(healthdata[1].count()));
		}
	}
	else
	{
		QMessageBox::critical(this, APPNAME, tr("Could not open \"%1\"!\n\nReason: %2").arg(filename).arg(file.errorString()));
	}
}

void MainWindow::importDataFromSQL(QString filename, bool append, bool showmsg)
{
	QString msg(tr("Successfully imported %1 records from SQL:\n\n     %2 = %3\n     %4 = %5"));
	HEALTHDATA set;
	int duplicate = 0;
	QAction *active = NULL;

	if(append)
	{
		active = groupUser->checkedAction();
	}
	else
	{
		healthdata[0].clear();
		healthdata[1].clear();

		buildGraph(healthdata[user], healthstat[user]);
	}

	action_User1->setChecked(false);
	action_User2->setChecked(false);

	if(!db.isValid())
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "OBPM");
	}

	db.setDatabaseName(filename);

	if(db.open())
	{
		QSqlQuery query(db);

		if(query.exec("SELECT * FROM U1") && query.record().count() >= 4)
		{
			while(query.next())
			{
				set.time = query.value("uts").toInt();
				set.sys = query.value("sys").toInt();
				set.dia = query.value("dia").toInt();
				set.bpm = query.value("bpm").toInt();

				if(query.record().count() > 4)
				{
					set.msg = query.value("msg").toString();
				}

				healthdata[0].append(set);
			}
		}

		if(query.exec("SELECT * FROM U2") && query.record().count() >= 4)
		{
			while(query.next())
			{
				set.time = query.value("uts").toInt();
				set.sys = query.value("sys").toInt();
				set.dia = query.value("dia").toInt();
				set.bpm = query.value("bpm").toInt();

				if(query.record().count() > 4)
				{
					set.msg = query.value("msg").toString();
				}

				healthdata[1].append(set);
			}
		}

		db.close();

		duplicate = validateData(active, append);

		if(!healthdata[0].count() && !healthdata[1].count())
		{
			if(showmsg)
			{
				QMessageBox::warning(this, APPNAME, tr("No valid records in imported SQL found!\n\nPlease check table format of file: %1").arg("\n\n'U1'|'U2' ('uts' INTEGER, 'sys' INTEGER, 'dia' INTEGER, 'bpm' INTEGER)"));
			}

			return;
		}
		else
		{
			if(duplicate)
			{
				msg.append("\n\n" + tr("Skipped %1 duplicate entries!").arg(duplicate));
			}

			if(showmsg)
			{
				QMessageBox::information(this, APPNAME, msg.arg(healthdata[0].count() + healthdata[1].count()).arg(cfg.alias1).arg(healthdata[0].count()).arg(cfg.alias2).arg(healthdata[1].count()));
			}
		}
	}
	else
	{
		if(showmsg)
		{
			QMessageBox::critical(this, APPNAME, tr("Could not open \"%1\"!\n\nReason: %2").arg(filename).arg(db.lastError().text()));
		}
	}
}

void MainWindow::exportDataToCSV(QString filename)
{
	QFile file(filename);
	QString header("user;date;time;sys;dia;bpm;comment\n");
	QString record;

	if(file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		file.write(header.toUtf8(), header.length());

		for(int i = 0; i < healthdata[0].count(); i++)
		{
			record = QString("1;%1;%2;%3;%4;%5\n").arg(QDateTime::fromTime_t(healthdata[0].at(i).time).toString("dd.MM.yy;hh:mm")).arg(healthdata[0].at(i).sys).arg(healthdata[0].at(i).dia, 3, 10, QChar('0')).arg(healthdata[0].at(i).bpm, 3, 10, QChar('0')).arg(healthdata[0].at(i).msg);

			file.write(record.toUtf8(), record.toUtf8().length());
		}

		for(int i = 0; i < healthdata[1].count(); i++)
		{
			record = QString("2;%1;%2;%3;%4;%5\n").arg(QDateTime::fromTime_t(healthdata[1].at(i).time).toString("dd.MM.yy;hh:mm")).arg(healthdata[1].at(i).sys).arg(healthdata[1].at(i).dia, 3, 10, QChar('0')).arg(healthdata[1].at(i).bpm, 3, 10, QChar('0')).arg(healthdata[1].at(i).msg);

			file.write(record.toUtf8(), record.toUtf8().length());
		}

		file.close();

		if(QMessageBox::question(this, APPNAME, tr("Successfully exported %1 records to CSV:\n\n     %2 = %3\n     %4 = %5\n\nOpen now with default app?").arg(healthdata[0].count() + healthdata[1].count()).arg(cfg.alias1).arg(healthdata[0].count()).arg(cfg.alias2).arg(healthdata[1].count()), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
		{
			QDesktopServices::openUrl(QUrl("file:///" + filename));
		}
	}
	else
	{
		QMessageBox::critical(this, APPNAME, tr("Could not create \"%1\"!\n\nReason: %2").arg(filename).arg(file.errorString()));
	}
}

void MainWindow::exportDataToSQL(QString filename, bool showmsg)
{
	if(!db.isValid())
	{
		db = QSqlDatabase::addDatabase("QSQLITE", "OBPM");
	}

	db.setDatabaseName(filename);

	if(db.open())
	{
		QSqlQuery query(db);

		db.transaction();

		if(db.tables().count())
		{
			query.exec("DROP TABLE 'U1'");
			query.exec("DROP TABLE 'U2'");
		}

		query.exec("CREATE TABLE 'U1' ('uts' INTEGER, 'sys' INTEGER, 'dia' INTEGER, 'bpm' INTEGER, 'msg' TEXT)");
		query.exec("CREATE TABLE 'U2' ('uts' INTEGER, 'sys' INTEGER, 'dia' INTEGER, 'bpm' INTEGER, 'msg' TEXT)");

		for(int i = 0; i < healthdata[0].count(); i++)
		{
			query.exec(QString("INSERT INTO 'U1' VALUES (%1, %2, %3, %4, '%5')").arg(healthdata[0].at(i).time).arg(healthdata[0].at(i).sys).arg(healthdata[0].at(i).dia).arg(healthdata[0].at(i).bpm).arg(healthdata[0].at(i).msg));
		}

		for(int i = 0; i < healthdata[1].count(); i++)
		{
			query.exec(QString("INSERT INTO 'U2' VALUES (%1, %2, %3, %4, '%5')").arg(healthdata[1].at(i).time).arg(healthdata[1].at(i).sys).arg(healthdata[1].at(i).dia).arg(healthdata[1].at(i).bpm).arg(healthdata[1].at(i).msg));
		}

		db.commit();

		db.close();

		if(showmsg)
		{
			if(QMessageBox::question(this, APPNAME, tr("Successfully exported %1 records to SQL:\n\n     %2 = %3\n     %4 = %5\n\nOpen now with default app?").arg(healthdata[0].count() + healthdata[1].count()).arg(cfg.alias1).arg(healthdata[0].count()).arg(cfg.alias2).arg(healthdata[1].count()), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
			{
				QDesktopServices::openUrl(QUrl("file:///" + filename));
			}
		}
	}
	else
	{
		QMessageBox::critical(this, APPNAME, tr("Could not create \"%1\"!\n\nReason: %2").arg(filename).arg(db.lastError().driverText()));
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

void MainWindow::on_action_Setup_triggered()
{
	setupDialog *dlg = new setupDialog(this, &cfg);

	if(dlg->exec() == QDialog::Accepted)
	{
		line_sys->point1->setCoords(0, cfg.sys);
		line_sys->point2->setCoords(1, cfg.sys);
		line_dia->point1->setCoords(0, cfg.dia);
		line_dia->point2->setCoords(1, cfg.dia);
		line_bpm->point1->setCoords(0, cfg.bpm);
		line_bpm->point2->setCoords(1, cfg.bpm);

		widget_bp->graph(0)->setLineStyle((QCPGraph::LineStyle)cfg.style);
		widget_bp->graph(1)->setLineStyle((QCPGraph::LineStyle)cfg.style);
		widget_hr->graph(0)->setLineStyle((QCPGraph::LineStyle)cfg.style);

		widget_bp->legend->setVisible(cfg.legend);
		widget_hr->legend->setVisible(cfg.legend);

		widget_bp->replot();
		widget_hr->replot();

		action_User1->setToolTip(tr("show %1").arg(cfg.alias1));
		action_User2->setToolTip(tr("show %1").arg(cfg.alias2));
		action_User1->setStatusTip(tr("show %1").arg(cfg.alias1));
		action_User2->setStatusTip(tr("show %1").arg(cfg.alias2));
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
	QString filename = QFileDialog::getOpenFileName(this, tr("Import from CSV or SQL File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator(), tr("CSV File (*.csv);;SQL File (*.sql)"), 0, QFileDialog::DontUseNativeDialog);

	if(!filename.isEmpty())
	{
		if((healthdata[0].count() || healthdata[1].count()) && QMessageBox::question(this, APPNAME, tr("Append new data to existing records?")) == QMessageBox::Yes)
		{
			filename.endsWith(".csv", Qt::CaseInsensitive) ? importDataFromCSV(filename, true) : importDataFromSQL(filename, true, true);
		}
		else
		{
			filename.endsWith(".csv", Qt::CaseInsensitive) ? importDataFromCSV(filename, false) : importDataFromSQL(filename, false, true);
		}
	}
}

void MainWindow::on_action_exportToCSV_triggered()
{
	if(healthdata[0].count() || healthdata[1].count())
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + QString("obpm-%1.csv").arg(QDateTime::currentDateTime().date().toString("yyyyMMdd")), tr("CSV File (*.csv)"), 0, QFileDialog::DontUseNativeDialog);

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

void MainWindow::on_action_exportToSQL_triggered()
{
	if(healthdata[0].count() || healthdata[1].count())
	{
		QString filename = QFileDialog::getSaveFileName(this, tr("Export to SQL"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + QString("obpm-%1.sql").arg(QDateTime::currentDateTime().date().toString("yyyyMMdd")), tr("SQL File (*.sql)"), 0, QFileDialog::DontUseNativeDialog);

		if(!filename.isEmpty())
		{
			exportDataToSQL(filename, true);
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
		QString filename = QFileDialog::getSaveFileName(this, tr("Export to PDF"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + QString("obpm-%1-%2-%3.pdf").arg(user ? cfg.alias2 : cfg.alias1).arg(rangeStart->dateTime().toString("yyyyMMdd"), rangeStop->dateTime().toString("yyyyMMdd")), tr("PDF File (*.pdf)"), 0, QFileDialog::DontUseNativeDialog);

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
	showHelp("00");
}

void MainWindow::on_action_About_triggered()
{
	new aboutDialog(this);
}

void MainWindow::on_action_showHideLegend_toggled(bool state)
{
	cfg.legend = state;

	widget_bp->legend->setVisible(state);
	widget_hr->legend->setVisible(state);

	widget_bp->replot();
	widget_hr->replot();
}

void MainWindow::on_action_resetZoom_triggered()
{
	if(filter->isChecked())
	{
		rangeStart->setDate(QDateTime::fromTime_t(filterdata.first().time - tdiff).date());
		rangeStop->setDate(QDateTime::fromTime_t(filterdata.last().time + tdiff).date());

		widget_bp->xAxis->setRange(filterdata.first().time - tdiff, filterdata.last().time + tdiff);
		widget_bp->replot();
	}
	else
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
}


void MainWindow::on_action_addRecord_triggered()
{
	new recordDialog(this);
}

void MainWindow::on_action_querySQL_triggered()
{
	if(healthdata[0].count() || healthdata[1].count())
	{
		sqlDialog *dlg = new sqlDialog(this);

		if(dlg->success)
		{
			dlg->show();
			dlg->activateWindow();
		}
	}
	else
	{
		QMessageBox::warning(this, APPNAME, tr("No records to analyze!"));
	}
}

void MainWindow::on_action_User1_toggled(bool enabled)
{
	if(enabled)
	{
		user = 0;

		filter->setChecked(false);

		buildGraph(healthdata[user], healthstat[user]);
	}
}

void MainWindow::on_action_User2_toggled(bool enabled)
{
	if(enabled)
	{
		user = 1;

		filter->setChecked(false);

		buildGraph(healthdata[user], healthstat[user]);
	}
}

void MainWindow::buildGraph(QVector <HEALTHDATA> data, HEALTHSTAT stat)
{
	widget_bp->graph(0)->clearData();
	widget_bp->graph(1)->clearData();
	widget_hr->graph(0)->clearData();

	if(data.count())
	{
		rangeStart->setDateTimeRange(QDateTime(QDateTime::fromTime_t(data.first().time).date(), QTime(0, 0, 0, 0)), QDateTime(QDateTime::fromTime_t(data.last().time).date(), QTime(23, 59, 59, 999)));
		rangeStop->setDateTimeRange(QDateTime(QDateTime::fromTime_t(data.first().time).date(), QTime(0, 0, 0, 0)), QDateTime(QDateTime::fromTime_t(data.last().time).date(), QTime(23, 59, 59, 999)));
		rangeStart->setDateTime(QDateTime(QDateTime::fromTime_t(data.first().time).date(), QTime(0, 0, 0, 0)));
		rangeStop->setDateTime(QDateTime(QDateTime::fromTime_t(data.last().time).date(), QTime(23, 59, 59, 999)));

		widget_bp->xAxis->setRange(data.first().time - tdiff, data.last().time + tdiff);
		widget_bp->yAxis->setRange(stat.dia_min - 10, stat.sys_max + 10);
		widget_hr->xAxis->setRange(data.first().time - tdiff, data.last().time + tdiff);
		widget_hr->yAxis->setRange(stat.bpm_min - 5, stat.bpm_max < 100 ? 100 : stat.bpm_max + 5);

		widget_bp->plottable(0)->setName(QString("SYS: %1 [%2] %3").arg(stat.sys_min).arg(stat.sys_mid).arg(stat.sys_max));
		widget_bp->plottable(1)->setName(QString("DIA: %1 [%2] %3").arg(stat.dia_min).arg(stat.dia_mid).arg(stat.dia_max));
		widget_hr->plottable(0)->setName(QString(tr("Pulse: %1 [%2] %3")).arg(stat.bpm_min).arg(stat.bpm_mid).arg(stat.bpm_max));

		for(int i = 0; i < data.count(); i++)
		{
			widget_bp->graph(0)->addData(data.at(i).time, data.at(i).sys);
			widget_bp->graph(1)->addData(data.at(i).time, data.at(i).dia);
			widget_hr->graph(0)->addData(data.at(i).time, data.at(i).bpm);
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

	lcd->setDigitCount(QString::number(data.count()).length());
	lcd->display(data.count());
}

void MainWindow::dateChanged()
{
	if(filter->isChecked())
	{
		filterChanged(true);
	}
	else
	{
		widget_bp->xAxis->setRange(rangeStart->dateTime().toTime_t(), rangeStop->dateTime().toTime_t());
		widget_bp->replot();
	}
}

void MainWindow::filterChanged(bool checked)
{
	if(checked)
	{
		filterdata.clear();

		for(int i = 0; i < healthdata[user].count(); i++)
		{
			if(healthdata[user].at(i).time >= rangeStart->dateTime().toTime_t() && healthdata[user].at(i).time <= rangeStop->dateTime().toTime_t())
			{
				filterdata.append(healthdata[user].at(i));
			}
		}

		if(filterdata.count())
		{
				getHealthStats(filterdata, &filterstat);

				buildGraph(filterdata, filterstat);
		}
		else
		{
			filter->setChecked(false);

			QMessageBox::warning(this, APPNAME, tr("No records found for selected period!"));
		}
	}
	else
	{
		getHealthStats(healthdata[user], &healthstat[user]);

		buildGraph(healthdata[user], healthstat[user]);
	}
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

int MainWindow::indexFromTime(QObject *src, QPoint pos)
{
	int index = -1;

	if(((QCustomPlot*)src)->plottableAt(pos))
	{
		QCPData data = widget_hr->graph(0)->data()->lowerBound(((QCustomPlot*)src)->xAxis->pixelToCoord(pos.x() - 8)).value();

		for(int scan = 0; scan < healthdata[user].count(); scan++)
		{
			if((uint)data.key == healthdata[user].at(scan).time)
			{
				index = scan;

				break;
			}
		}
	}

	return index;
}

void MainWindow::showHelp(QString page)
{
	if(help)
	{
		((helpDialog*)help)->setSourceFromPage(page);
	}
	else
	{
		help = new helpDialog(this, page, &cfg);
	}
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
	if(ev->type() == QEvent::ToolTip)
	{
		int record = indexFromTime(obj, ((QHelpEvent*)ev)->pos());

		if(record >= 0)
		{
			QToolTip::showText(((QHelpEvent*)ev)->globalPos(), healthdata[user].at(record).msg.isEmpty() ? QString("%1\n\nSYS %2 / DIA %3 / BPM %4").arg(QDateTime::fromTime_t(healthdata[user].at(record).time).toString("dddd, dd.MM.yyyy hh:mm")).arg(healthdata[user].at(record).sys).arg(healthdata[user].at(record).dia).arg(healthdata[user].at(record).bpm) : QString("%1\n\nSYS %2 / DIA %3 / BPM %4\n\n%5").arg(QDateTime::fromTime_t(healthdata[user].at(record).time).toString("dddd, dd.MM.yyyy hh:mm")).arg(healthdata[user].at(record).sys).arg(healthdata[user].at(record).dia).arg(healthdata[user].at(record).bpm).arg(healthdata[user].at(record).msg));
		}
		else
		{
			QToolTip::hideText();
		}

		return true;
	}

	return QMainWindow::eventFilter(obj, ev);
}

void MainWindow::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_F1)
	{
		showHelp("01.01");
	}
	else if(ke->key() == Qt::Key_Escape)
	{
		close();
	}

	QMainWindow::keyPressEvent(ke);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *me)
{
	if(me->button() == Qt::MiddleButton)
	{
		on_action_resetZoom_triggered();
	}
	else if(me->button() == Qt::RightButton && sender())
	{
		QMenu menu;
		QAction* selectedItem;

		int record = indexFromTime(sender(), me->pos());

		if(record >= 0)
		{
			menu.addAction(action_deleteRecord);
			menu.addAction(action_commentRecord);

			selectedItem = menu.exec(me->globalPos());

			if(selectedItem)
			{
				if(selectedItem->objectName() == "action_deleteRecord")
				{
					if(QMessageBox::question(this, APPNAME, tr("Really delete this record?\n\n%1\n\nSYS %2 / DIA %3 / BPM %4").arg(QDateTime::fromTime_t(healthdata[user].at(record).time).toString("dddd, dd.MM.yyyy hh:mm")).arg(healthdata[user].at(record).sys).arg(healthdata[user].at(record).dia).arg(healthdata[user].at(record).bpm), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
					{
						healthdata[user].remove(record);

						if(healthdata[user].count())
						{
							getHealthStats(healthdata[user], &healthstat[user]);
						}

						buildGraph(healthdata[user], healthstat[user]);
					}
				}
				else if(selectedItem->objectName() == "action_commentRecord")
				{
					bool rc;

					QString msg = QInputDialog::getText(this, APPNAME, tr("Please enter comment for this record:"), QLineEdit::Normal, healthdata[user].at(record).msg, &rc);

					if(rc)
					{
						healthdata[user][record].msg = msg;
					}
				}
			}
		}
		else
		{
			QActionGroup *group = new QActionGroup(this);

			menu.addAction(action_showHideLegend);
			menu.addSeparator();
			menu.addAction(action_stylePoint);
			menu.addAction(action_styleLine);
			menu.addAction(action_styleStep);
			menu.addAction(action_styleImpulse);
			menu.addSeparator();
			menu.addAction(action_resetZoom);

			action_showHideLegend->setChecked(cfg.legend);

			group->addAction(action_stylePoint)->setChecked(cfg.style == QCPGraph::lsNone ? true : false);
			group->addAction(action_styleLine)->setChecked(cfg.style == QCPGraph::lsLine ? true : false);
			group->addAction(action_styleStep)->setChecked(cfg.style == QCPGraph::lsStepCenter ? true : false);
			group->addAction(action_styleImpulse)->setChecked(cfg.style == QCPGraph::lsImpulse ? true : false);

			selectedItem = menu.exec(me->globalPos());

			if(selectedItem)
			{
				if(selectedItem->objectName() == "action_stylePoint")
				{
					cfg.style = QCPGraph::lsNone;
				}
				else if(selectedItem->objectName() == "action_styleLine")
				{
					cfg.style = QCPGraph::lsLine;
				}
				else if(selectedItem->objectName() == "action_styleStep")
				{
					cfg.style = QCPGraph::lsStepCenter;
				}
				else if(selectedItem->objectName() == "action_styleImpulse")
				{
					cfg.style = QCPGraph::lsImpulse;
				}
				else
				{
					return;
				}

				widget_bp->graph(0)->setLineStyle((QCPGraph::LineStyle)cfg.style);
				widget_bp->graph(1)->setLineStyle((QCPGraph::LineStyle)cfg.style);
				widget_hr->graph(0)->setLineStyle((QCPGraph::LineStyle)cfg.style);

				widget_bp->replot();
				widget_hr->replot();
			}
		}
	}
}

void MainWindow::closeEvent(QCloseEvent *ce)
{
	if(update || QMessageBox::question(this, APPNAME, tr("Really exit program?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		if(!isMaximized())
		{
			cfg.pmain = pos();
			cfg.smain = size();
		}

		setConfig();

		if(healthdata[0].count() || healthdata[1].count())
		{
			exportDataToSQL(cfg.database, false);
		}
		else if(db.isValid())
		{
			QFile::remove(cfg.database);
		}

		ce->accept();
	}
	else
	{
		ce->ignore();
	}
}
