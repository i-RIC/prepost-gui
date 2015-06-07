#include "ui_solverdefinitionabstractwidget.h"

#include "solverdefinitionabstractwidget.h"

#include <guicore/solverdef/solverdefinitionabstract.h>

#include <QUrl>

SolverDefinitionAbstractWidget::SolverDefinitionAbstractWidget(const SolverDefinitionAbstract* solver, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::SolverDefinitionAbstractWidget)
{
	m_solverDefinition = solver;
	ui->setupUi(this);
	setup();
}

SolverDefinitionAbstractWidget::~SolverDefinitionAbstractWidget()
{
	delete ui;
}

void SolverDefinitionAbstractWidget::setup()
{
	QGridLayout* layout = reinterpret_cast<QGridLayout*>(ui->basicGroup->layout());
	layout->setColumnStretch(0, 0);
	layout->setColumnStretch(1, 1);
	layout->update();
	ui->nameValue->setText(m_solverDefinition->caption());
	ui->versionValue->setText(m_solverDefinition->version().toString());
	ui->copyrightValue->setText(m_solverDefinition->copyright());
	ui->releaseValue->setText(m_solverDefinition->release().toString("yyyy/MM/dd"));
	QString url = m_solverDefinition->url();
	QString link;
	if (url != "") {
		QUrl u(url);
		link.append("<a href=\"").append(u.toString()).append("\">").append(url).append("</a>");
	}
	ui->homepageValue->setText(link);
	ui->tabWidget->setCurrentWidget(ui->readmeTab);
	QString tmptxt;
	tmptxt = m_solverDefinition->description();
	ui->readmeBrowser->setText(tmptxt);
	tmptxt = m_solverDefinition->license();
	ui->licenseBrowser->setText(tmptxt);
}
