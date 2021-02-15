#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpEngineCore>
#include <QtHelp/QHelpSearchEngine>
#include <QtHelp/QHelpContentWidget>
#include <QtHelp/QHelpIndexWidget>
#include <QtHelp/QHelpSearchQuery>
#include <QtHelp/QHelpSearchQueryWidget>
#include <QtHelp/QHelpSearchResultWidget>
#include <QtWidgets>

#include "ui_mainwindow.h"

class usb;

#include "usb.h"
#include "about.h"
#include "donation.h"
#include "update.h"
#include "setup.h"
#include "help.h"
#include "sql.h"
#include "record.h"
#include "login.h"

#include "qcpdocumentobject.h"

#define APPNAME QObject::tr("Omron Blood Pressure Manager")
#define APPTRNS QObject::tr("LazyT")
#define APPLANG QObject::tr("English")
#define APPVERS "2.0.2"
#define APPDATE "21.09.2020"
#define APPRELS "1"

#define SYS_NORM 135
#define DIA_NORM 85
#define BPM_NORM 80
#define IHB_NORM 0

#define TABLES 3
#define TABLE_HEAD 1
#define TABLE_ROWS 20
#define TABLE_COLS 6
#ifdef Q_OS_OSX
	#define TABLE_CORR 2
#else
	#define TABLE_CORR 0
#endif

#define TDIFF 0.015

#define CFG QDir::homePath() + "/.obpm/obpm.cfg"
#define BL QDir::homePath() + "/.obpm/blacklist"
#define DB QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/obpm.sql"
#define EXP QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)

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
    bool ihbp;
	uint style;
	uint sys;
	uint dia;
	uint bpm;
    uint ihb;
	QString alias1, alias2;
	QString database;
	QString exports;
	QPoint pmain, phelp;
	QSize smain, shelp;
};

struct HEALTHDATA{
	uint time;
	uint sys;
	uint dia;
	uint bpm;
    uint ihb;
	QString msg;
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
    uint ihb;
};

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

public:

	explicit MainWindow(QWidget *parent = 0);
	void getHealthStats(QVector<HEALTHDATA>, HEALTHSTAT*);
	void showHelp(QString);
	int validateData(QAction*, bool);
	void buildGraph(QVector<HEALTHDATA>, HEALTHSTAT);
	QDialog *help;
	QVector <HEALTHDATA> healthdata[2], filterdata;
	HEALTHSTAT healthstat[2], filterstat;
	CONFIG cfg;
	uint user;
	QDateTimeEdit *rangeStart, *rangeStop;
	QPushButton *filter;
	QNetworkAccessManager *mgr;
	QElapsedTimer timeout;
	QActionGroup *groupUser;

private:

	QTranslator baseTranslator, helpTranslator, appTranslator;
	QLCDNumber *lcd;
	QCPItemStraightLine *line_sys, *line_dia, *line_bpm, *line_ihb;
//	QCPItemStraightLine *line_sys, *line_dia, *line_bpm;
	QVector <HEALTHDATA> exportdata;
	QList <uint> blacklist;
	QProgressDialog *pdlg;
	QSqlDatabase db;
	bool update;
	int offsetUTC;
	int tdiff;

private slots:

	void proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);

	void getConfig();
	void setConfig();

	void getBlacklist();
	void setBlacklist();

	void checkUpdate();

	void createDoc(QPrinter*);
	void createDocTablePage(int, int, int, QTextCursor, QTextBlockFormat, QTextBlockFormat, QTextCharFormat, QTextCharFormat, int*);

	void importDataFromUSB(bool);
	void importDataFromCSV(QString, bool);
	void importDataFromSQL(QString, bool, bool);
	void exportDataToCSV(QString);
	void exportDataToSQL(QString, bool);
	void exportDataToPDF(QString);

	void dateChanged();
	void filterChanged(bool);
	void xAxisBPChanged(QCPRange);
	void xAxisHRChanged(QCPRange);
	void plotAxisTicks();

	bool eventFilter(QObject*, QEvent*);

	int indexFromTime(QObject*, QPoint);

	void keyPressEvent(QKeyEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void closeEvent(QCloseEvent*);

	void on_action_importFromUSB_triggered();
	void on_action_importFromFile_triggered();
	void on_action_exportToCSV_triggered();
	void on_action_exportToSQL_triggered();
	void on_action_exportToPDF_triggered();
	void on_action_addRecord_triggered();
	void on_action_querySQL_triggered();
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
