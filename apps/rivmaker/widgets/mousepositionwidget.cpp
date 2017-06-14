#include "ui_mousepositionwidget.h"
#include "mousepositionwidget.h"

#include "../data/project/project.h"

MousePositionWidget::MousePositionWidget(QWidget* parent) :
	QWidget {parent},
	ui {new Ui::MousePositionWidget},
	m_project {nullptr}
{
	ui->setupUi(this);
}

MousePositionWidget::~MousePositionWidget()
{
	delete ui;
}

void MousePositionWidget::setProject(Project* p)
{
	m_project = p;
}

void MousePositionWidget::updatePosition(const QPointF& pos)
{
	double x = pos.x();
	double y = pos.y();

	if (m_project != nullptr) {
		QPointF offset = m_project->offset();
		x += offset.x();
		y += offset.y();
	}
	ui->labelX->setText(QString("X: %1").arg(x, 0, 'f', 3));
	ui->labelY->setText(QString("Y: %1").arg(y, 0, 'f', 3));
}

void MousePositionWidget::updatePositionWithoutOffset(const QPointF& pos)
{
	double x = pos.x();
	double y = pos.y();
	ui->labelX->setText(QString("X: %1").arg(x, 0, 'f', 3));
	ui->labelY->setText(QString("Y: %1").arg(y, 0, 'f', 3));
}

void MousePositionWidget::clear()
{
	ui->labelX->setText(tr("X: ------"));
	ui->labelY->setText(tr("Y: ------"));
}
