#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QHelpEngine>
#include <QHelpContentWidget>
#include <QHelpIndexWidget>
#include <QHelpSearchQuery>
#include <QHelpSearchQueryWidget>
#include <QHelpSearchResultWidget>

#include "ui_mainwindow.h"

#include "usb.h"
#include "about.h"
#include "donation.h"
#include "update.h"
#include "setup.h"
#include "help.h"

#include "qcpdocumentobject.h"

#define APPNAME QObject::tr("Omron Blood Pressure Manager")
#define APPVERS "1.0.5"
#define APPDATE "21.02.2016"
#define APPRELS "6"

#define SYS_NORM 135
#define DIA_NORM 85
#define BPM_NORM 80

#define TABLES 3
#define TABLE_HEAD 1
#define TABLE_ROWS 20
#define TABLE_COLS 5
#ifdef Q_OS_OSX
	#define TABLE_CORR 2
#else
	#define TABLE_CORR 0
#endif

#define tdiff 3600

#define CFG QDir::homePath() + QDir::separator() + ".obpm" + QDir::separator() + "obpm.cfg"
#define DB QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + "obpm.sql"

#ifdef Q_OS_LINUX
	#define UPD '"' + QCoreApplication::applicationDirPath() + "/maintenancetool" + '"'
#elif defined Q_OS_WIN
	#define UPD '"' + QCoreApplication::applicationDirPath() + "/maintenancetool.exe" + '"'
#elif defined Q_OS_OSX
	#define UPD '"' + QCoreApplication::applicationDirPath() + "/../../../maintenancetool.app/Contents/MacOS/maintenancetool" + '"'
#endif

struct CONFIG
{
	bool hem7322u;
	bool update;
	bool legend;
	bool psd;
	bool pot;
	uint style;
	uint sys;
	uint dia;
	uint bpm;
	QString alias1, alias2;
	QString database;
	QPoint pmain, phelp;
	QSize smain, shelp;
};

struct HEALTHDATA{
	uint time;
	uint sys;
	uint dia;
	uint bpm;
};

struct HEALTHSTAT{
	uint sys_min;
	uint sys_mid;
	uint sys_max;
	uint dia_min;
	uint dia_mid;
	uint dia_max;
	uint bpm_min;
	uint bpm_mid;
	uint bpm_max;
};

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

public:

	explicit MainWindow(QWidget *parent = 0);
	QDialog *help;
	QVector <HEALTHDATA> healthdata[2], filterdata;
	void getHealthStats(QVector<HEALTHDATA>, HEALTHSTAT*);
	void showHelp(QString);
	CONFIG cfg;

private:

	QTranslator baseTranslator, helpTranslator, appTranslator;
	QActionGroup *groupUser;
	QLCDNumber *lcd;
	QDateTimeEdit *rangeStart, *rangeStop;
	QPushButton *filter;
	QCPItemStraightLine *line_sys, *line_dia, *line_bpm;
	QVector <HEALTHDATA> exportdata;
	HEALTHSTAT healthstat[2], filterstat;
	QProgressDialog *pdlg;
	QSqlDatabase db;
	uint user;
	bool update;

private slots:

	void getConfig();
	void setConfig();

	void checkUpdate();

	void createDoc(QPrinter*);
	void createDocTablePage(int, int, int, QTextCursor, QTextBlockFormat, QTextBlockFormat, QTextCharFormat, QTextCharFormat);

	void importDataFromUSB(bool);
	void importDataFromCSV(QString, bool);
	void importDataFromSQL(QString, bool, bool);
	void exportDataToCSV(QString);
	void exportDataToSQL(QString, bool);
	void exportDataToPDF(QString);

	void buildGraph(QVector<HEALTHDATA>, HEALTHSTAT);
	void dateChanged();
	void filterChanged(bool);
	void xAxisBPChanged(QCPRange);
	void xAxisHRChanged(QCPRange);

	void keyPressEvent(QKeyEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void closeEvent(QCloseEvent*);

	void on_action_importFromUSB_triggered();
	void on_action_importFromFile_triggered();
	void on_action_exportToCSV_triggered();
	void on_action_exportToSQL_triggered();
	void on_action_exportToPDF_triggered();
	void on_action_User1_toggled(bool);
	void on_action_User2_toggled(bool);
	void on_action_PrintPreview_triggered();
	void on_action_Print_triggered();
	void on_action_Help_triggered();
	void on_action_About_triggered();
	void on_action_showHideLegend_toggled(bool);
	void on_action_resetZoom_triggered();
	void on_action_Setup_triggered();
	void on_action_Exit_triggered();
};

#endif
