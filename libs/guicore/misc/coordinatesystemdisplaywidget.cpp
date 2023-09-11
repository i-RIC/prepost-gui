#include "coordinatesystemdisplaywidget.h"
#include "ui_coordinatesystemdisplaywidget.h"

#include <cs/coordinatesystem.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

CoordinateSystemDisplayWidget::CoordinateSystemDisplayWidget(QWidget *parent) :
	QWidget(parent),
	m_projectData {nullptr},
	ui(new Ui::CoordinateSystemDisplayWidget)
{
	ui->setupUi(this);
	updateDisplay();

	connect(ui->labelCs, &QLabelWithClickedSignal::clicked, this, &CoordinateSystemDisplayWidget::editCoordinateSystem);
}

CoordinateSystemDisplayWidget::~CoordinateSystemDisplayWidget()
{
	delete ui;
}

void CoordinateSystemDisplayWidget::setProjectData(ProjectData* data)
{
	m_projectData = data;
	updateDisplay();
}

void CoordinateSystemDisplayWidget::updateDisplay()
{
	QString csStr = tr("(Not Specified)");

	if (m_projectData != nullptr) {
		auto mf = m_projectData->mainfile();
		auto cs = mf->coordinateSystem();
		if (cs != nullptr) {
			csStr = cs->caption();
		}
	}
	ui->labelCs->setText(tr("CS: %1").arg(csStr));
}

void CoordinateSystemDisplayWidget::editCoordinateSystem()
{
	if (m_projectData == nullptr) {return;}

	auto ret = m_projectData->mainfile()->showCoordinateSystemDialog();
	if (ret == QDialog::Rejected) {return;}

	updateDisplay();
}
