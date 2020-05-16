#include "ui_graph2dverificationwindowcontrolwidget.h"

#include "graph2dverificationwindowcontrolwidget.h"

#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dVerificationWindowControlWidget::Graph2dVerificationWindowControlWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Graph2dVerificationWindowControlWidget)
{
	ui->setupUi(this);

	connect(ui->settingButton, SIGNAL(clicked()), this, SLOT(handleSettingButtonClicked()));
	connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(handleExportButtonClicked()));
}

Graph2dVerificationWindowControlWidget::~Graph2dVerificationWindowControlWidget()
{
	delete ui;
}

void Graph2dVerificationWindowControlWidget::setSetting(const Graph2dVerificationWindowResultSetting& /* setting */)
{
	adjustSize();
	updateGeometry();
	fitSize();
}


void Graph2dVerificationWindowControlWidget::setComment(const QString& title)
{
	ui->commentLabel->setText(title);
}

void Graph2dVerificationWindowControlWidget::fitSize()
{
	QWidget* p = parentWidget();
	QSize s = p->size();
	p->resize(s.width(), p->minimumHeight());
}

void Graph2dVerificationWindowControlWidget::handleSettingButtonClicked()
{
	emit settingButtonClicked();
}

void Graph2dVerificationWindowControlWidget::handleExportButtonClicked()
{
	emit exportButtonClicked();
}
