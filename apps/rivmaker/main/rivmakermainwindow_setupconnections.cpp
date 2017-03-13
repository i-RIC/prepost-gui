#include "rivmakermainwindow.h"
#include "ui_rivmakermainwindow.h"

void RivmakerMainWindow::setupConnections()
{
	connect(ui->exitAction, SIGNAL(triggered()), this, SLOT(close()));

	connect(ui->focusPreWindowAction, SIGNAL(triggered()), this, SLOT(focusPreProcessorWindow()));
	connect(ui->focusVerticalCrosssectionWindowAction, SIGNAL(triggered()), this, SLOT(focusVerticalCrossSectionWindow()));

	connect(ui->viewToolbarAction, SIGNAL(toggled(bool)), this, SLOT(viewToggleToolBar(bool)));
	connect(ui->viewStatusBarAction, SIGNAL(toggled(bool)), this, SLOT(viewToggleStatusBar(bool)));

	connect(ui->mouseHintAction, SIGNAL(triggered()), this, SLOT(helpMouseHint()));
}
