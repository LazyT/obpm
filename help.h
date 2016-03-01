#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include "ui_help.h"
#include "mainwindow.h"

class HelpBrowser : public QTextBrowser
{
	Q_OBJECT

public:

	HelpBrowser(QHelpEngine *helpEngine, QWidget *parent = NULL) : QTextBrowser(parent), helpEngine(helpEngine) {};

	QVariant loadResource(int type, const QUrl &name) { return (name.scheme() == "qthelp" ? QVariant(helpEngine->fileData(name)) : QTextBrowser::loadResource(type, name)); }

private:

	QHelpEngine *helpEngine;
};

class helpDialog : public QDialog, private Ui::Dialog_Help
{
	Q_OBJECT

public:

	helpDialog(QWidget *parent, QString, struct CONFIG*);
	void setSourceFromPage(QString);

private:

	QHelpEngine *helpEngine;
	QHelpSearchEngine *searchEngine;
	QHelpContentWidget *contentWidget;
	QHelpIndexWidget *indexWidget;
	QHelpSearchQueryWidget *queryWidget;
	QHelpSearchResultWidget *resultWidget;
	HelpBrowser *helpBrowser;
	QString lng;
	CONFIG *cfg;

private slots:

	void setSource(QUrl);
	void setSourceFromContent(QModelIndex);
	void setSourceFromIndex(QModelIndex);
	void searchQuery();
	void anchorClicked(const QUrl&);
	void reject();
};

#endif
