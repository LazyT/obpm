#include "help.h"

helpDialog::helpDialog(QWidget *parent, QString page) : QDialog(parent)
{
	setupUi(this);

	setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

	lng = QFile::exists(QString("%1/hlp/obpm_%2.qhc").arg(QApplication::applicationDirPath()).arg(QLocale::system().name().mid(0, 2))) ? QLocale::system().name().mid(0, 2) : "en";

	helpEngine = new QHelpEngine(QString("%1/hlp/obpm_%2.qhc").arg(QApplication::applicationDirPath()).arg(lng));
	helpEngine->setupData();

	searchEngine = helpEngine->searchEngine();
	searchEngine->reindexDocumentation();

	contentWidget = helpEngine->contentWidget();
	indexWidget = helpEngine->indexWidget();
	queryWidget = searchEngine->queryWidget();
	resultWidget = searchEngine->resultWidget();

	verticalLayout_content->addWidget(contentWidget);
	verticalLayout_index->addWidget(indexWidget);
	verticalLayout_search->addWidget(queryWidget);
	verticalLayout_search->addWidget(resultWidget);

	helpBrowser = new HelpBrowser(helpEngine);

	splitter->addWidget(helpBrowser);
	splitter->setStretchFactor(0, 0);
	splitter->setStretchFactor(1, 1);
	splitter->widget(0)->setMinimumWidth(250);

	connect(contentWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(setSourceFromContent(QModelIndex)));
	connect(contentWidget, SIGNAL(activated(QModelIndex)), this, SLOT(setSourceFromContent(QModelIndex)));
	connect(indexWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(setSourceFromIndex(QModelIndex)));
	connect(indexWidget, SIGNAL(activated(QModelIndex)), this, SLOT(setSourceFromIndex(QModelIndex)));
	connect(resultWidget, SIGNAL(requestShowLink(const QUrl&)), this, SLOT(setSource(const QUrl&)));
	connect(queryWidget, SIGNAL(search()),this, SLOT(searchQuery()));
	connect(helpBrowser, SIGNAL(anchorClicked(const QUrl&)),this, SLOT(anchorClicked(const QUrl&)));

	show();
	activateWindow();
	move(parent->mapToGlobal(parent->rect().center()) - rect().center());

	QApplication::processEvents();

	setSourceFromPage(page);

	contentWidget->expandAll();
//	contentWidget->setStyleSheet("QTreeView::item::!has-children { color: #505050 }");

	QMessageBox::information(this, APPNAME, tr("Help is unfortunately still under construction..."));
}

void helpDialog::setSource(QUrl url)
{
	helpBrowser->setSource(url);

	contentWidget->expand(contentWidget->indexOf(helpBrowser->source()));
	contentWidget->setCurrentIndex(contentWidget->indexOf(helpBrowser->source()));
}

void helpDialog::setSourceFromPage(QString page)
{
	setSource(QString("qthelp://obpm/%1/help-%2.html").arg(lng).arg(page));
}

void helpDialog::setSourceFromContent(QModelIndex index)
{
	setSource(helpEngine->contentModel()->contentItemAt(index)->url());
}

void helpDialog::setSourceFromIndex(QModelIndex index)
{
	setSource(helpEngine->indexModel()->linksForKeyword(index.data().toString()).first());
}

void helpDialog::searchQuery()
{
	searchEngine->search(queryWidget->query());
}

void helpDialog::anchorClicked(const QUrl &link)
{
	setSource(link);
}

void helpDialog::reject()
{
	((MainWindow*)parent())->help = NULL;

	accept();
}
