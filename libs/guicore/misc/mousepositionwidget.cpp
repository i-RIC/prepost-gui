#include "ui_mousepositionwidget.h"

#include "mousepositionwidget.h"

#include <QPointF>

MousePositionWidget::MousePositionWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::MousePositionWidget),
	m_projectData {nullptr},
	m_xLabel {"X"},
	m_yLabel {"Y"}
{
	ui->setupUi(this);
}

MousePositionWidget::~MousePositionWidget()
{
	delete ui;
}

void MousePositionWidget::setProjectData(ProjectData* data)
{
	m_projectData = data;
}

void MousePositionWidget::setAxisLabels(const QString& xLabel, const QString& yLabel)
{
	m_xLabel = xLabel;
	m_yLabel = yLabel;
}

void MousePositionWidget::updatePosition(const QPointF& position)
{
	m_xLabel = "X";
	m_yLabel = "Y";

	QSettings settings;
	auto decimals = settings.value("textformat/statusbarcoordsdecimal", 6).toInt();

	double x = position.x();
	double y = position.y();
	if (m_projectData != nullptr) {
		auto offset = m_projectData->mainfile()->offset();
		x += offset.x();
		y += offset.y();
	}

	ui->labelX->setText(QString("%1: %2").arg(m_xLabel).arg(x, 0, 'f', decimals));
	ui->labelY->setText(QString("%1: %2").arg(m_yLabel).arg(y, 0, 'f', decimals));
}

void MousePositionWidget::updateCrossSectionPosition(const QPointF& position)
{
	QSettings settings;
	auto decimals = settings.value("textformat/statusbarcoordsdecimal", 6).toInt();

	ui->labelX->setText(QString("%1: %2").arg(m_xLabel).arg(position.x(), 0, 'f', decimals));
	ui->labelY->setText(QString("%1: %2").arg(m_yLabel).arg(position.y(), 0, 'f', decimals));
}

void MousePositionWidget::clear()
{
	ui->labelX->setText(QString("%1: ------").arg(m_xLabel));
	ui->labelY->setText(QString("%1: ------").arg(m_yLabel));
}
