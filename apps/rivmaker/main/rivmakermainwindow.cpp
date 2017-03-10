#include "rivmakermainwindow.h"
#include "ui_rivmakermainwindow.h"

RivmakerMainWindow::RivmakerMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::RivmakerMainWindow)
{
	ui->setupUi(this);
}

RivmakerMainWindow::~RivmakerMainWindow()
{
	delete ui;
}
