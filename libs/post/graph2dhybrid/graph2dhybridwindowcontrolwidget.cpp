#include "ui_graph2dhybridwindowcontrolwidget.h"

#include "graph2dhybridwindowcontrolwidget.h"

#include <geodata/polydatagroup/geodatapolydatagroup.h>
#include <geodata/polylinegroup/geodatapolylinegrouppolyline.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dHybridWindowControlWidget::Graph2dHybridWindowControlWidget(QWidget* parent) :
	QWidget(parent),
	m_setupFinished {false},
	ui(new Ui::Graph2dHybridWindowControlWidget)
{
	ui->setupUi(this);

	ui->iSlider->setRange(0, 100);
	ui->jSlider->setRange(0, 100);
	ui->kSlider->setRange(0, 100);
	ui->indexSlider->setRange(0, 100);

	connect(ui->dataSourceButton, &QPushButton::clicked, this, &Graph2dHybridWindowControlWidget::dataSourceButtonClicked);
	connect(ui->axisButton, &QPushButton::clicked, this, &Graph2dHybridWindowControlWidget::axisButtonClicked);
	connect(ui->drawButton, &QPushButton::clicked, this, &Graph2dHybridWindowControlWidget::drawButtonClicked);
	connect(ui->markerButton, &QPushButton::clicked, this, &Graph2dHybridWindowControlWidget::markerButtonClicked);
	connect(ui->fontButton, &QPushButton::clicked, this, &Graph2dHybridWindowControlWidget::fontButtonClicked);
	connect(ui->copyButton, &QPushButton::clicked, this, &Graph2dHybridWindowControlWidget::copyButtonClicked);
	connect(ui->snapshotButton, &QPushButton::clicked, this, &Graph2dHybridWindowControlWidget::snapshotButtonClicked);
	connect(ui->csvExportButton, &QPushButton::clicked, this, &Graph2dHybridWindowControlWidget::csvExportButtonClicked);

	connect(ui->iSlider, &SliderWithValue::valueChanged, this, &Graph2dHybridWindowControlWidget::iValueChanged);
	connect(ui->jSlider, &SliderWithValue::valueChanged, this, &Graph2dHybridWindowControlWidget::jValueChanged);
	connect(ui->kSlider, &SliderWithValue::valueChanged, this, &Graph2dHybridWindowControlWidget::kValueChanged);
	connect(ui->indexSlider, &SliderWithValue::valueChanged, this, &Graph2dHybridWindowControlWidget::indexValueChanged);
	connect<void (QComboBox::*)(int)>(ui->lineComboBox, &QComboBox::currentIndexChanged, this, &Graph2dHybridWindowControlWidget::polyLineChanged);
}

Graph2dHybridWindowControlWidget::~Graph2dHybridWindowControlWidget()
{
	delete ui;
}

void Graph2dHybridWindowControlWidget::setSetting(const Graph2dHybridWindowResultSetting& setting, int dim[4])
{
	static Graph2dHybridWindowResultSetting previousSetting;

	Graph2dHybridWindowResultSetting::DataTypeInfo* info = setting.targetDataTypeInfo();
	ui->iSlider->setRange(1, dim[0]);
	ui->jSlider->setRange(1, dim[1]);
	ui->kSlider->setRange(1, dim[2]);
	ui->indexSlider->setRange(1, dim[3]);

	if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
		ui->lineComboBox->clear();
		m_lines.clear();

		auto line = setting.targetPolyLineGroupPolyLine();
		auto group = line->group();
		for (auto d : group->allData()) {
			auto l = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (d);
			m_lines.push_back(l);
			ui->lineComboBox->addItem(l->name());
		}
	}

	bool sametoPrev = true;
	if (m_setupFinished) {
		auto prevTI = previousSetting.targetDataTypeInfo();
		auto ti = setting.targetDataTypeInfo();
		sametoPrev = (prevTI->dataType == ti->dataType && previousSetting.xAxisMode() == setting.xAxisMode());
	}
	if (m_setupFinished && sametoPrev) {return;}

	ui->iLabel->hide();
	ui->iSlider->hide();
	ui->jLabel->hide();
	ui->jSlider->hide();
	ui->kLabel->hide();
	ui->kSlider->hide();
	ui->indexLabel->hide();
	ui->indexSlider->hide();
	ui->lineLabel->hide();
	ui->lineComboBox->hide();
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
		} else if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
			ui->lineLabel->show();
			ui->lineComboBox->show();
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
		if (setting.xAxisMode() != Graph2dHybridWindowResultSetting::xaPolyLine && setting.xAxisMode() != Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
			ui->indexLabel->show();
			ui->indexSlider->show();
		} else if (setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
			ui->lineLabel->show();
			ui->lineComboBox->show();
		}
		break;
	}
	adjustSize();
	updateGeometry();
	fitSize();

	previousSetting = setting;
	m_setupFinished = true;
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

void Graph2dHybridWindowControlWidget::setPolyline(GeoDataPolyLineGroupPolyLine* line)
{
	auto it = std::find(m_lines.begin(), m_lines.end(), line);
	if (it != m_lines.end()) {
		ui->lineComboBox->setCurrentIndex(it - m_lines.begin());
	}
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

GeoDataPolyLineGroupPolyLine* Graph2dHybridWindowControlWidget::polyline() const
{
	if (ui->lineComboBox->count() > static_cast<int> (m_lines.size())) {
		return nullptr;
	}
	int current = ui->lineComboBox->currentIndex();
	if (current < 0) {return nullptr;}
	if (current >= m_lines.size()) {return nullptr;}

	return m_lines.at(current);
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
