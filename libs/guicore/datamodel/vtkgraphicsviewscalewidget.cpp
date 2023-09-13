#include "vtkgraphicsviewscalewidget.h"
#include "ui_vtkgraphicsviewscalewidget.h"
#include "private/vtkgraphicsviewscalewidget_scaleeditdialog.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QInputDialog>

VtkGraphicsViewScaleWidget::VtkGraphicsViewScaleWidget(QWidget *parent) :
	QWidget(parent),
	m_view {nullptr},
	ui(new Ui::VtkGraphicsViewScaleWidget)
{
	ui->setupUi(this);
	connect(ui->scaleLabel, &QLabelWithClickedSignal::clicked, this, &VtkGraphicsViewScaleWidget::editScale);
	updateDisplay();
}

VtkGraphicsViewScaleWidget::~VtkGraphicsViewScaleWidget()
{
	delete ui;
}

void VtkGraphicsViewScaleWidget::setView(VTKGraphicsView* view)
{
	m_view = view;
	if (m_view != nullptr) {
		connect(m_view, &VTKGraphicsView::scaleChanged, this, &VtkGraphicsViewScaleWidget::updateDisplay);
	}

	updateDisplay();
}

void VtkGraphicsViewScaleWidget::updateDisplay()
{
	QString scaleStr = "---";

	if (m_view != nullptr) {
		scaleStr = QString("1 / %1").arg(QString::number(m_view->getScale()));
	}
	ui->scaleLabel->setText(tr("Scale: %1").arg(scaleStr));
}

void VtkGraphicsViewScaleWidget::editScale()
{
	if (m_view == nullptr) {return;}

	ScaleEditDialog dialog(this);
	dialog.setScale(m_view->getScale());

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_view->setScale(dialog.scale());
	m_view->render();
	updateDisplay();
}
