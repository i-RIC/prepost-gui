#include "ui_graph2dscatteredwindowcontrolwidget.h"

#include "graph2dscatteredwindowcontrolwidget.h"

#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dScatteredWindowControlWidget::Graph2dScatteredWindowControlWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Graph2dScatteredWindowControlWidget)
{
	ui->setupUi(this);

	connect(ui->dataSourceButton, SIGNAL(clicked()), this, SIGNAL(dataSourceButtonClicked()));
	connect(ui->axisButton, SIGNAL(clicked()), this, SIGNAL(axisButtonClicked()));
	connect(ui->drawButton, SIGNAL(clicked()), this, SIGNAL(drawButtonClicked()));
}

Graph2dScatteredWindowControlWidget::~Graph2dScatteredWindowControlWidget()
{
	delete ui;
}

void Graph2dScatteredWindowControlWidget::fitSize()
{
	QWidget* p = parentWidget();
	QSize s = p->size();
	p->resize(s.width(), p->minimumHeight());
}
