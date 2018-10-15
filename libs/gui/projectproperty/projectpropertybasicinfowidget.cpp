#include "ui_projectpropertybasicinfowidget.h"

#include "../main/iricmainwindow.h"
#include "projectpropertybasicinfowidget.h"

#include <cs/coordinatesystem.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <pre/preprocessorwindow.h>

#include <QDateTime>
#include <QFileInfo>

ProjectPropertyBasicInfoWidget::ProjectPropertyBasicInfoWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ProjectPropertyBasicInfoWidget)
{
	m_projectData = nullptr;
	ui->setupUi(this);
	connect(ui->csEditButton, SIGNAL(clicked()), this, SLOT(showSelectCoordinateSystemDialog()));
	connect(ui->coEditButton, SIGNAL(clicked()), this, SLOT(showSetOffsetDialog()));
}

ProjectPropertyBasicInfoWidget::~ProjectPropertyBasicInfoWidget()
{
	delete ui;
}

void ProjectPropertyBasicInfoWidget::setProjectData(ProjectData* data)
{
	m_projectData = data;
	if (data->filename().isNull()) {
		ui->filenameWidget->setText(tr("(No data)"));
	} else {
		ui->filenameWidget->setText(data->filename());
	}
	QString projFile = data->absoluteFileName("project.xml");
	QFileInfo p(projFile);
	if (! p.exists()) {
		ui->updateTimeWidget->setText(tr("(Not saved yet)"));
	} else {
		QDateTime lm = p.lastModified();
		ui->updateTimeWidget->setText(lm.toString());
	}
	QString solverInfo = QString("%1 version %2").arg(data->solverDefinition()->caption(), data->solverDefinition()->version().toString());
	ui->solverWidget->setText(solverInfo);

	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(data->mainWindow()->preProcessorWindow());
	PreProcessorWindow::GridState gridState = pre->checkGridState();
	switch (gridState) {
	case PreProcessorWindow::grFinished:
		ui->gridsWidget->setText(tr("Setup finished"));
		break;
	case PreProcessorWindow::grUnfinished:
		ui->gridsWidget->setText(tr("Setup unfinished"));
		break;
	case PreProcessorWindow::grPartiallyUnfinished:
		ui->gridsWidget->setText(tr("Setup partially unfinished"));
		break;
	}

	if (pre->isInputConditionSet()) {
		ui->calcCondWidget->setText(tr("Already set"));
	} else {
		ui->calcCondWidget->setText(tr("Not set yet"));
	}
	if (data->mainfile()->postSolutionInfo()->isDataAvailable()) {
		ui->resultWidget->setText(tr("Data exists"));
	} else {
		ui->resultWidget->setText(tr("No data"));
	}
	updateCoordinateSystem();
	updateCoordinateOffset();
}

void ProjectPropertyBasicInfoWidget::showSelectCoordinateSystemDialog()
{
	int ret = m_projectData->mainfile()->showCoordinateSystemDialog();
	if (ret == QDialog::Rejected) {return;}
	updateCoordinateSystem();
}

void ProjectPropertyBasicInfoWidget::updateCoordinateSystem()
{
	if (m_projectData->mainfile()->coordinateSystem() != nullptr) {
		ui->csWidget->setText(m_projectData->mainfile()->coordinateSystem()->caption());
	} else {
		ui->csWidget->setText(tr("(Not specified)"));
	}
}

void ProjectPropertyBasicInfoWidget::showSetOffsetDialog()
{
	m_projectData->mainfile()->setupOffset();
	updateCoordinateOffset();
}

void ProjectPropertyBasicInfoWidget::updateCoordinateOffset()
{
	auto offset = m_projectData->mainfile()->offset();
	ui->coValue->setText(QString("(%1, %2)").arg(offset.x()).arg(offset.y()));
}
