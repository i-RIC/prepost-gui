#include "startpagelaunchtabwidget.h"
#include "ui_startpagelaunchtabwidget.h"

#include <QFileDialog>

StartPageLaunchTabWidget::StartPageLaunchTabWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::StartPageLaunchTabWidget)
{
	ui->setupUi(this);
	connect(ui->newProjectButton, SIGNAL(clicked()), this, SIGNAL(newProjectButtonClicked()));
	connect(ui->openProjectButton, SIGNAL(clicked()), this, SIGNAL(openProjectButtonClicked()));
	connect(ui->recentProjectsList, SIGNAL(projectSelected(QString)), this, SIGNAL(projectFileSelected(QString)));
	connect(ui->recentSolversList, SIGNAL(solverSelected(SolverDefinitionAbstract*)), this, SIGNAL(solverSelected(SolverDefinitionAbstract*)));
}

StartPageLaunchTabWidget::~StartPageLaunchTabWidget()
{
	delete ui;
}

void StartPageLaunchTabWidget::openProject()
{}

void StartPageLaunchTabWidget::setSolverList(SolverDefinitionList* solverList)
{
	ui->recentSolversList->setSolverList(solverList);
}
