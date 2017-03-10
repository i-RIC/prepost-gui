#include "data/project.h"
#include "rivmakermainwindow.h"
#include "../dialogs/mousehelpdialog.h"

#include "ui_rivmakermainwindow.h"

#include <QMdiSubWindow>

RivmakerMainWindow::RivmakerMainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_verticalCrossSectionWindow {this},
	m_project {nullptr},
	ui(new Ui::RivmakerMainWindow)
{
	ui->setupUi(this);
	setupConnections();

	auto w = ui->centralwidget->addSubWindow(&m_verticalCrossSectionWindow);
	w->hide();
}

RivmakerMainWindow::~RivmakerMainWindow()
{
	delete m_project;
	delete ui;
}

void RivmakerMainWindow::newProject()
{
	deleteProject();
	m_project = new Project();
}

void RivmakerMainWindow::focusVerticalCrossSectionWindow()
{
	if (! m_project->checkIfReadyToOpenVerticalCrossSectionWindow(this)) {return;}

	auto pw = m_verticalCrossSectionWindow.parentWidget();
	pw->show();
	pw->setFocus();
}

void RivmakerMainWindow::openCrossSectionWindow()
{

}

void RivmakerMainWindow::viewToggleToolBar(bool visible)
{
	ui->toolBar->setVisible(visible);
}

void RivmakerMainWindow::viewToggleStatusBar(bool visible)
{
	ui->statusbar->setVisible(visible);
}

void RivmakerMainWindow::helpMouseHint()
{
	static MouseHelpDialog* dialog = nullptr;
	if (dialog == nullptr) {
		dialog = new MouseHelpDialog(this);
	}
	dialog->show();
}

void RivmakerMainWindow::deleteProject()
{
	delete m_project;
	m_project = nullptr;
}
