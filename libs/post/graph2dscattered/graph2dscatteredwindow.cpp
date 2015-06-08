#include "graph2dscatteredwindow.h"
#include "graph2dscatteredwindowactionmanager.h"
#include "graph2dscatteredwindowcontrolwidget.h"
#include "graph2dscatteredwindowdatamodel.h"
#include "graph2dscatteredwindowobjectbrowser.h"
#include "graph2dscatteredwindowprojectdataitem.h"
#include "graph2dscatteredwindowview.h"

#include <guicore/postcontainer/postsolutioninfo.h>

#include <QDockWidget>
#include <QPainter>

Graph2dScatteredWindow::Graph2dScatteredWindow(QWidget* parent, int index, Graph2dScatteredWindowProjectDataItem* pdi)
	: PostProcessorWindow(parent, index, pdi)
{
	init();
}

Graph2dScatteredWindow::~Graph2dScatteredWindow()
{
	delete m_dataModel;
}

void Graph2dScatteredWindow::init()
{
	setWindowTitle(tr("Scattered Chart Window: %1").arg(m_index));

	m_icon = QIcon(":/images/iconVisGraphScattered.png");
	setWindowIcon(m_icon);

	m_actionManager = new Graph2dScatteredWindowActionManager(this);

	m_objectBrowser = new Graph2dScatteredWindowObjectBrowser(this);
	// object browser is created for future use, but not used now.
	addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);
	m_objectBrowser->hide();

	QDockWidget* dockWidget = new QDockWidget(tr("Controller"), this);
	dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
	dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	dockWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_controlWidget = new Graph2dScatteredWindowControlWidget(this);
	dockWidget->setWidget(m_controlWidget);

	addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
	connect(postSolutionInfo(), SIGNAL(updated()), this, SLOT(update()));
}

void Graph2dScatteredWindow::setupDefaultGeometry(int index)
{
	QWidget* parent = parentWidget();
	if (parent->isMaximized()) {return;}
	parent->move(index * 30, index * 30);
	parent->resize(700, 500);
}

QPixmap Graph2dScatteredWindow::snapshot()
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

QList<QMenu*> Graph2dScatteredWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	return menus;
}
