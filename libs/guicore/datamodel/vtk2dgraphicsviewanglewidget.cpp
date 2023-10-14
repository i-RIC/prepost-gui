#include "vtk2dgraphicsview.h"
#include "vtk2dgraphicsviewanglewidget.h"
#include "private/vtk2dgraphicsviewanglewidget_angleeditdialog.h"
#include "ui_vtk2dgraphicsviewanglewidget.h"

Vtk2dGraphicsViewAngleWidget::Vtk2dGraphicsViewAngleWidget(QWidget *parent) :
	QWidget(parent),
	m_view {nullptr},
	ui(new Ui::Vtk2dGraphicsViewAngleWidget)
{
	ui->setupUi(this);
	connect(ui->angleLabel, &QLabelWithClickedSignal::clicked, this, &Vtk2dGraphicsViewAngleWidget::editAngle);
	updateDisplay();
}

Vtk2dGraphicsViewAngleWidget::~Vtk2dGraphicsViewAngleWidget()
{
	delete ui;
}

void Vtk2dGraphicsViewAngleWidget::setView(VTK2DGraphicsView* view)
{
	m_view = view;
	if (m_view != nullptr) {
		connect(m_view, &VTK2DGraphicsView::angleChanged, this, &Vtk2dGraphicsViewAngleWidget::updateDisplay);
	}

	updateDisplay();
}

void Vtk2dGraphicsViewAngleWidget::updateDisplay()
{
	QString angleStr = "---";

	if (m_view != nullptr) {
		double angle = m_view->getAngle();
		while (angle > 360) {
			angle -= 360;
		}
		while (angle < 0) {
			angle += 360;
		}
		angleStr = QString("%1").arg(QString::number(angle));
	}
	ui->angleLabel->setText(tr("Angle: %1").arg(angleStr));
}

void Vtk2dGraphicsViewAngleWidget::editAngle()
{
	if (m_view == nullptr) {return;}

	AngleEditDialog dialog(this);
	dialog.setAngle(m_view->getAngle());

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_view->setAngle(dialog.angle());
	m_view->render();
	updateDisplay();
}
