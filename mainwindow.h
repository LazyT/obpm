#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "ui_mainwindow.h"

#include "usb.h"
#include "about.h"
#include "donation.h"
#include "update.h"
#include "setup.h"

#include "qcpdocumentobject.h"

#define APPNAME QObject::tr("Omron Blood Pressure Manager")
#define APPVERS "1.0.4"
#define APPDATE "31.12.2015"
#define APPRELS "5"

#define SYS_NORM 135
#define DIA_NORM 85
#define BPM_NORM 80

#define TABLES 3
#define TABLE_HEAD 1
#define TABLE_ROWS 20
#define TABLE_COLS 5
#if defined Q_OS_OSX
#define TABLE_CORR 2
#else
#define TABLE_CORR 0
#endif

#define tdiff 3600

#define CFG QDir::homePath() + QDir::separator() + ".obpm" + QDir::separator() + "obpm.cfg"
#define BAK QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + "obpm-db.sql"

struct CONFIG
{
	bool m500it;
	bool update;
	bool legend;
	uint style;
	uint sys;
	uint dia;
	uint bpm;
	QString alias1, alias2;
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
	QVector <HEALTHDATA> healthdata[2];
	void getHealthStats(bool);

	CONFIG cfg;

private:

	QTranslator sysTranslator, appTranslator;
	QActionGroup *groupUser;
	QDateTimeEdit *rangeStart, *rangeStop;
	QCPItemStraightLine *line_sys, *line_dia, *line_bpm;
	QVector <HEALTHDATA> exportdata;
	HEALTHSTAT healthstat[2];
	QTextDocument *doc;
	QProgressDialog *pdlg;
	QSqlDatabase db;
	uint user;

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

	void buildGraph(bool user);
	void dateChanged();
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
