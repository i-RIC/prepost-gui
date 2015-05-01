#include "mousepositionwidget.h"
#include "ui_mousepositionwidget.h"

MousePositionWidget::MousePositionWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::MousePositionWidget)
{
	ui->setupUi(this);
	m_projectData = nullptr;
}

MousePositionWidget::~MousePositionWidget()
{
	delete ui;
}


void MousePositionWidget::updatePosition(const QVector2D& position)
{
	double x = position.x();
	double y = position.y();
	if (m_projectData != nullptr) {
		QVector2D offset = m_projectData->mainfile()->offset();
		x += offset.x();
		y += offset.y();
	}

	ui->labelX->setText(QString("X: %1").arg(x, 0, 'g', 20));
	ui->labelY->setText(QString("Y: %1").arg(y, 0, 'g', 20));
}

void MousePositionWidget::clear()
{
	ui->labelX->setText(tr("X: ------"));
	ui->labelY->setText(tr("Y: ------"));
}
