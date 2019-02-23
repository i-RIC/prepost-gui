#include "ui_graph2dhybridwindowcontrolwidget.h"

#include "graph2dhybridwindowcontrolwidget.h"

#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dHybridWindowControlWidget::Graph2dHybridWindowControlWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Graph2dHybridWindowControlWidget)
{
	ui->setupUi(this);

	ui->iSlider->setRange(0, 100);
	ui->jSlider->setRange(0, 100);
	ui->kSlider->setRange(0, 100);
	ui->indexSlider->setRange(0, 100);

	connect(ui->dataSourceButton, SIGNAL(clicked()), this, SIGNAL(dataSourceButtonClicked()));
	connect(ui->axisButton, SIGNAL(clicked()), this, SIGNAL(axisButtonClicked()));
	connect(ui->drawButton, SIGNAL(clicked()), this, SIGNAL(drawButtonClicked()));
	connect(ui->markerButton, SIGNAL(clicked()), this, SIGNAL(markerButtonClicked()));
	connect(ui->copyButton, SIGNAL(clicked()), this, SIGNAL(copyButtonClicked()));
	connect(ui->snapshotButton, SIGNAL(clicked()), this, SIGNAL(snapshotButtonClicked()));
	connect(ui->csvExportButton, SIGNAL(clicked()), this, SIGNAL(csvExportButtonClicked()));

	connect(ui->iSlider, SIGNAL(valueChanged(int)), this, SIGNAL(iValueChanged(int)));
	connect(ui->jSlider, SIGNAL(valueChanged(int)), this, SIGNAL(jValueChanged(int)));
	connect(ui->kSlider, SIGNAL(valueChanged(int)), this, SIGNAL(kValueChanged(int)));
	connect(ui->indexSlider, SIGNAL(valueChanged(int)), this, SIGNAL(indexValueChanged(int)));
}

Graph2dHybridWindowControlWidget::~Graph2dHybridWindowControlWidget()
{
	delete ui;
}

void Graph2dHybridWindowControlWidget::setSetting(const Graph2dHybridWindowResultSetting& setting, int dim[4])
{
	Graph2dHybridWindowResultSetting::DataTypeInfo* info = setting.targetDataTypeInfo();
	ui->iSlider->setRange(1, dim[0]);
	ui->jSlider->setRange(1, dim[1]);
	ui->kSlider->setRange(1, dim[2]);
	ui->indexSlider->setRange(1, dim[3]);

	ui->iLabel->hide();
	ui->iSlider->hide();
	ui->jLabel->hide();
	ui->jSlider->hide();
	ui->kLabel->hide();
	ui->kSlider->hide();
	ui->indexLabel->hide();
	ui->indexSlider->hide();
	if (info == nullptr) {return;}
	switch (info->dataType) {
	case Graph2dHybridWindowResultSetting::dtBaseIterative:
		break;
	case Graph2dHybridWindowResultSetting::dtDim1DStructured:
		if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			ui->iLabel->show();
			ui->iSlider->show();
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim2DStructured:
		if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			ui->iLabel->show();
			ui->iSlider->show();
			ui->jLabel->show();
			ui->jSlider->show();
		} else if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			ui->jLabel->show();
			ui->jSlider->show();
		} else if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			ui->iLabel->show();
			ui->iSlider->show();
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim3DStructured:
		if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaTime) {
			ui->iLabel->show();
			ui->iSlider->show();
			ui->jLabel->show();
			ui->jSlider->show();
			ui->kLabel->show();
			ui->kSlider->show();
		} else if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaI) {
			ui->jLabel->show();
			ui->jSlider->show();
			ui->kLabel->show();
			ui->kSlider->show();
		} else if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaJ) {
			ui->iLabel->show();
			ui->iSlider->show();
			ui->kLabel->show();
			ui->kSlider->show();
		} else if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaK) {
			ui->iLabel->show();
			ui->iSlider->show();
			ui->jLabel->show();
			ui->jSlider->show();
		}
		break;
	case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
	case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
	case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
		if (setting.xAxisMode() != Graph2dHybridWindowResultSetting::xaPolyline) {
			ui->indexLabel->show();
			ui->indexSlider->show();
		}
		break;
	}
	adjustSize();
	updateGeometry();
	fitSize();
}


void Graph2dHybridWindowControlWidget::setIValue(int i)
{
	ui->iSlider->setValue(i + 1);
}

void Graph2dHybridWindowControlWidget::setJValue(int j)
{
	ui->jSlider->setValue(j + 1);
}

void Graph2dHybridWindowControlWidget::setKValue(int k)
{
	ui->kSlider->setValue(k + 1);
}

void Graph2dHybridWindowControlWidget::setIndexValue(int index)
{
	ui->indexSlider->setValue(index + 1);
}

int Graph2dHybridWindowControlWidget::iValue() const
{
	return ui->iSlider->value() - 1;
}

int Graph2dHybridWindowControlWidget::jValue() const
{
	return ui->jSlider->value() - 1;
}

int Graph2dHybridWindowControlWidget::kValue() const
{
	return ui->kSlider->value() - 1;
}

int Graph2dHybridWindowControlWidget::indexValue() const
{
	return ui->indexSlider->value() - 1;
}

void Graph2dHybridWindowControlWidget::fitSize()
{
	QWidget* p = parentWidget();
	QSize s = p->size();
	p->resize(s.width(), p->minimumHeight());
}

void Graph2dHybridWindowControlWidget::loadFromProjectMainFile(const QDomNode& node)
{
	int val;
	val = iRIC::getIntAttribute(node, "i");
	setIValue(val);
	val = iRIC::getIntAttribute(node, "j");
	setJValue(val);
	val = iRIC::getIntAttribute(node, "k");
	setKValue(val);
	val = iRIC::getIntAttribute(node, "index");
	setIndexValue(val);
}

void Graph2dHybridWindowControlWidget::saveToProjectMainFile(QXmlStreamWriter& writer) const
{
	iRIC::setIntAttribute(writer, "i", iValue());
	iRIC::setIntAttribute(writer, "j", jValue());
	iRIC::setIntAttribute(writer, "k", kValue());
	iRIC::setIntAttribute(writer, "index", indexValue());
}
