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

	//ui->iSlider->setRange(0, 100);
	//ui->jSlider->setRange(0, 100);
	//ui->kSlider->setRange(0, 100);
	//ui->indexSlider->setRange(0, 100);

	//connect(ui->dataSourceButton, SIGNAL(clicked()), this, SIGNAL(dataSourceButtonClicked()));
	//connect(ui->axisButton, SIGNAL(clicked()), this, SIGNAL(axisButtonClicked()));
	//connect(ui->drawButton, SIGNAL(clicked()), this, SIGNAL(drawButtonClicked()));
	//connect(ui->markerButton, SIGNAL(clicked()), this, SIGNAL(markerButtonClicked()));
	//connect(ui->copyButton, SIGNAL(clicked()), this, SIGNAL(copyButtonClicked()));
	//connect(ui->snapshotButton, SIGNAL(clicked()), this, SIGNAL(snapshotButtonClicked()));
	//connect(ui->csvExportButton, SIGNAL(clicked()), this, SIGNAL(csvExportButtonClicked()));

	//connect(ui->iSlider, SIGNAL(valueChanged(int)), this, SIGNAL(iValueChanged(int)));
	//connect(ui->jSlider, SIGNAL(valueChanged(int)), this, SIGNAL(jValueChanged(int)));
	//connect(ui->kSlider, SIGNAL(valueChanged(int)), this, SIGNAL(kValueChanged(int)));
	//connect(ui->indexSlider, SIGNAL(valueChanged(int)), this, SIGNAL(indexValueChanged(int)));

	connect(ui->settingButton, SIGNAL(clicked()), this, SLOT(handleSettingButtonClicked()));
	connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(handleExportButtonClicked()));
}

Graph2dVerificationWindowControlWidget::~Graph2dVerificationWindowControlWidget()
{
	delete ui;
}

void Graph2dVerificationWindowControlWidget::setSetting(const Graph2dVerificationWindowResultSetting& setting)
{
#if 0 || 1
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = setting.targetDataTypeInfo();
	
	//{{
	//}}

	//ui->iSlider->setRange(1, dim[0]);
	//ui->jSlider->setRange(1, dim[1]);
	//ui->kSlider->setRange(1, dim[2]);
	//ui->indexSlider->setRange(1, dim[3]);

	//ui->iLabel->hide();
	//ui->iSlider->hide();
	//ui->jLabel->hide();
	//ui->jSlider->hide();
	//ui->kLabel->hide();
	//ui->kSlider->hide();
	//ui->indexLabel->hide();
	//ui->indexSlider->hide();
	if (info == nullptr) {return;}
#if SKIP
	switch (info->dataType) {
	case Graph2dVerificationWindowResultSetting::dtBaseIterative:
		break;
	case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
		if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			//ui->iLabel->show();
			//ui->iSlider->show();
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
		if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			//ui->iLabel->show();
			//ui->iSlider->show();
			//ui->jLabel->show();
			//ui->jSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
			//ui->jLabel->show();
			//ui->jSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
			//ui->iLabel->show();
			//ui->iSlider->show();
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
		if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			//ui->iLabel->show();
			//ui->iSlider->show();
			//ui->jLabel->show();
			//ui->jSlider->show();
			//ui->kLabel->show();
			//ui->kSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
			//ui->jLabel->show();
			//ui->jSlider->show();
			//ui->kLabel->show();
			//ui->kSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
			//ui->iLabel->show();
			//ui->iSlider->show();
			//ui->kLabel->show();
			//ui->kSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaK) {
			//ui->iLabel->show();
			//ui->iSlider->show();
			//ui->jLabel->show();
			//ui->jSlider->show();
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim1DUnstructured:
	case Graph2dVerificationWindowResultSetting::dtDim2DUnstructured:
	case Graph2dVerificationWindowResultSetting::dtDim3DUnstructured:
		if (setting.xAxisMode() != Graph2dVerificationWindowResultSetting::xaPolyline) {
			//ui->indexLabel->show();
			//ui->indexSlider->show();
		}
		break;
	}
#endif
#endif
	adjustSize();
	updateGeometry();
	fitSize();
}


void Graph2dVerificationWindowControlWidget::setComment(const QString& title)
{
	ui->commentLabel->setText(title);
}

#if SKIP
void Graph2dVerificationWindowControlWidget::setIValue(int i)
{
	//ui->iSlider->setValue(i + 1);
}

void Graph2dVerificationWindowControlWidget::setJValue(int j)
{
	//ui->jSlider->setValue(j + 1);
}

void Graph2dVerificationWindowControlWidget::setKValue(int k)
{
	//ui->kSlider->setValue(k + 1);
}

void Graph2dVerificationWindowControlWidget::setIndexValue(int index)
{
	//ui->indexSlider->setValue(index + 1);
}

int Graph2dVerificationWindowControlWidget::iValue() const
{
	//return ui->iSlider->value() - 1;
	return 0;
}

int Graph2dVerificationWindowControlWidget::jValue() const
{
	//return ui->jSlider->value() - 1;
	return 0;
}

int Graph2dVerificationWindowControlWidget::kValue() const
{
	//return ui->kSlider->value() - 1;
	return 0;
}

int Graph2dVerificationWindowControlWidget::indexValue() const
{
	//return ui->indexSlider->value() - 1;
	return 0;
}
#endif

void Graph2dVerificationWindowControlWidget::fitSize()
{
	QWidget* p = parentWidget();
	QSize s = p->size();
	p->resize(s.width(), p->minimumHeight());
}

void Graph2dVerificationWindowControlWidget::loadFromProjectMainFile(const QDomNode& node)
{
#if SKIP
	int val;
	val = iRIC::getIntAttribute(node, "i");
	setIValue(val);
	val = iRIC::getIntAttribute(node, "j");
	setJValue(val);
	val = iRIC::getIntAttribute(node, "k");
	setKValue(val);
	val = iRIC::getIntAttribute(node, "index");
	setIndexValue(val);
#endif
}

void Graph2dVerificationWindowControlWidget::saveToProjectMainFile(QXmlStreamWriter& writer) const
{
#if SKIP
	iRIC::setIntAttribute(writer, "i", iValue());
	iRIC::setIntAttribute(writer, "j", jValue());
	iRIC::setIntAttribute(writer, "k", kValue());
	iRIC::setIntAttribute(writer, "index", indexValue());
#endif
}

void Graph2dVerificationWindowControlWidget::handleSettingButtonClicked()
{
	emit settingButtonClicked();
}

void Graph2dVerificationWindowControlWidget::handleExportButtonClicked()
{
	emit exportButtonClicked();
}
