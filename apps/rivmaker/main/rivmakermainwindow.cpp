#include "data/project.h"
#include "rivmakermainwindow.h"
#include "../dialogs/mousehelpdialog.h"

#include "ui_rivmakermainwindow.h"

RivmakerMainWindow::RivmakerMainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_project {nullptr},
	ui(new Ui::RivmakerMainWindow)
{
	ui->setupUi(this);
	setupConnections();
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
