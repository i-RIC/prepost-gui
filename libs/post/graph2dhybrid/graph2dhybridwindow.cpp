#include "graph2dhybridwindow.h"
#include "graph2dhybridwindowactionmanager.h"
#include "graph2dhybridwindowcontrolwidget.h"
#include "graph2dhybridwindowdatamodel.h"
#include "graph2dhybridwindowobjectbrowser.h"
#include "graph2dhybridwindowprojectdataitem.h"
#include "graph2dhybridwindowview.h"

#include <guicore/postcontainer/postsolutioninfo.h>

#include <QDockWidget>
#include <QPainter>

Graph2dHybridWindow::Graph2dHybridWindow(QWidget* parent, int index, Graph2dHybridWindowProjectDataItem* pdi) :
	PostProcessorWindow(parent, index, pdi)
{
	init();
}

Graph2dHybridWindow::~Graph2dHybridWindow()
{
	delete m_dataModel;
}

void Graph2dHybridWindow::init()
{
	setWindowTitle(tr("Graph Window: %1").arg(m_index));

	m_icon = QIcon(":/images/iconVisGraphHybrid.svg");
	setWindowIcon(m_icon);

	m_actionManager = new Graph2dHybridWindowActionManager(this);

	m_objectBrowser = new Graph2dHybridWindowObjectBrowser(this);
	// object browser is created for future use, but not used now.
	// addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);
	m_objectBrowser->hide();

	QDockWidget* dockWidget = new QDockWidget(tr("Controller"), this);
	dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
	dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	dockWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_controlWidget = new Graph2dHybridWindowControlWidget(this);
	dockWidget->setWidget(m_controlWidget);

	addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
	connect(postSolutionInfo(), SIGNAL(updated()), this, SLOT(update()));
}

void Graph2dHybridWindow::setupDefaultGeometry(int index)
{
	QWidget* parent = parentWidget();
	if (parent->isMaximized()) {return;}
	parent->move(index * 30, index * 30);
	parent->resize(700, 500);
}

QPixmap Graph2dHybridWindow::snapshot()
{
	QWidget* w = centralWidget();

	QPixmap pixmap(w->size() * devicePixelRatioF());
	pixmap.setDevicePixelRatio(devicePixelRatioF());

	QBrush brush = QBrush(Qt::white);
	QPainter painter;
	painter.begin(&pixmap);
	QRect rect = pixmap.rect();
	painter.fillRect(rect, brush);
	QRegion region(0, 0, pixmap.width(), pixmap.height());
	w->render(&painter, QPoint(), region, QWidget::DrawChildren);
	painter.end();
	return pixmap;
}

QList<QMenu*> Graph2dHybridWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	return menus;
}

const std::shared_ptr<QToolBar>& Graph2dHybridWindow::getAdditionalToolBar() const
{
	return m_toolBar;
}

Graph2dHybridWindowControlWidget* Graph2dHybridWindow::controlWidget() const
{
	return m_controlWidget;
}
