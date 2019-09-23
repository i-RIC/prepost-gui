#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowactionmanager.h"
#include "graph2dverificationwindowtopwidget.h"
#include "graph2dverificationwindowcontrolwidget.h"
#include "graph2dverificationwindowdatamodel.h"
#include "graph2dverificationwindowobjectbrowser.h"
#include "graph2dverificationwindowprojectdataitem.h"
#include "graph2dverificationwindowview.h"

#include <guicore/postcontainer/postsolutioninfo.h>

#include <QDockWidget>
#include <QPainter>

Graph2dVerificationWindow::Graph2dVerificationWindow(QWidget* parent, int index, Graph2dVerificationWindowProjectDataItem* pdi)
	: PostProcessorWindow(parent, index, pdi)
{
	init();
}

Graph2dVerificationWindow::~Graph2dVerificationWindow()
{
	// m_dataModel is allocated in Graph2dVerificationWindowProjectDataItem
	delete m_dataModel;
}

void Graph2dVerificationWindow::init()
{
	setWindowTitle(tr("Verification Window: %1").arg(m_index));

	m_icon = QIcon(":/images/iconGraphVerification.png");
	setWindowIcon(m_icon);

	m_actionManager = new Graph2dVerificationWindowActionManager(this);

	m_objectBrowser = new Graph2dVerificationWindowObjectBrowser(this);
	// object browser is created for future use, but not used now.
	// addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);
	m_objectBrowser->hide();


	////{{
	QDockWidget* dockWidgetTop = new QDockWidget(tr("Top"), this);
	dockWidgetTop->setTitleBarWidget(new QWidget());  // hide title bar
	dockWidgetTop->setFeatures(QDockWidget::DockWidgetMovable);
	dockWidgetTop->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	dockWidgetTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_topWidget = new Graph2dVerificationWindowTopWidget(this);
	dockWidgetTop->setWidget(m_topWidget);

	addDockWidget(Qt::TopDockWidgetArea, dockWidgetTop);
	////}}

	QDockWidget* dockWidget = new QDockWidget(tr("Controller"), this);
	dockWidget->setTitleBarWidget(new QWidget());  // hide title bar
	dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
	dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	dockWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_controlWidget = new Graph2dVerificationWindowControlWidget(this);
	dockWidget->setWidget(m_controlWidget);

	addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

	connect(postSolutionInfo(), SIGNAL(updated()), this, SLOT(update()));
	//{{
	//connect(m_topWidget, SIGNAL(graphTypeChanged(int)), m_dataModel, SLOT(setType(int)));
	//}}
}

void Graph2dVerificationWindow::setupDefaultGeometry(int index)
{
	QWidget* parent = parentWidget();
	if (parent->isMaximized()) {return;}
	parent->move(index * 30, index * 30);
	parent->resize(700, 500);
}

QPixmap Graph2dVerificationWindow::snapshot()
{
	QWidget* w = centralWidget();
	QPixmap pixmap(w->size());
	QBrush brush = QBrush(Qt::white);
	QPainter painter;
	painter.begin(&pixmap);
	QRect rect = pixmap.rect();
	painter.fillRect(rect, brush);
	w->render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
	painter.end();
	return pixmap;
}

QList<QMenu*> Graph2dVerificationWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	return menus;
}

QToolBar* Graph2dVerificationWindow::getAdditionalToolBar() const
{
	return nullptr;
}

Graph2dVerificationWindowControlWidget* Graph2dVerificationWindow::controlWidget() const
{
	return m_controlWidget;
}

Graph2dVerificationWindowTopWidget* Graph2dVerificationWindow::topWidget() const
{
	return m_topWidget;
}

void Graph2dVerificationWindow::setComment(const QString& comment)
{
	controlWidget()->setComment(comment);
}
