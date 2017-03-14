#include "rivmakermainwindow.h"
#include "ui_rivmakermainwindow.h"

void RivmakerMainWindow::setupConnections()
{
	connect(ui->exitAction, SIGNAL(triggered()), this, SLOT(close()));

	connect(ui->importElevationAction, SIGNAL(triggered()), this, SLOT(importElevation()));
	connect(ui->fileImportElevationAction, SIGNAL(triggered()), this, SLOT(importElevation()));

	connect(ui->importWaterSurfaceElevationAction, SIGNAL(triggered()), this, SLOT(importWaterSurfaceElevation()));
	connect(ui->fileImportWaterSurfaceElevationAction, SIGNAL(triggered()), this, SLOT(importWaterSurfaceElevation()));

	connect(ui->importSACGUIImportAction, SIGNAL(triggered()), this, SLOT(importSACGUIFile()));
	connect(ui->fileImportSACGUIImportAction, SIGNAL(triggered()), this, SLOT(importSACGUIFile()));

	connect(ui->importBaseLineAction, SIGNAL(triggered()), this, SLOT(importBaseLine()));
	connect(ui->fileImportBaseLineAction, SIGNAL(triggered()), this, SLOT(importBaseLine()));
	connect(ui->baseLineImportAction, SIGNAL(triggered()), this, SLOT(importBaseLine()));

	connect(ui->importCrosssectionLineAction, SIGNAL(triggered()), this, SLOT(importCrossSectionLines()));
	connect(ui->fileImportCrosssectionLineAction, SIGNAL(triggered()), this, SLOT(importCrossSectionLines()));
	connect(ui->crosssectionLineImportAction, SIGNAL(triggered()), this, SLOT(importCrossSectionLines()));

	connect(ui->fitAction, SIGNAL(triggered()), this, SLOT(fit()));
	connect(ui->resetRotationAction, SIGNAL(triggered()), this, SLOT(resetRotation()));
	connect(ui->rotate90Action, SIGNAL(triggered()), this, SLOT(rotate90()));
	connect(ui->zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
	connect(ui->zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

	connect(ui->focusPreWindowAction, SIGNAL(triggered()), this, SLOT(focusPreProcessorWindow()));
	connect(ui->focusVerticalCrosssectionWindowAction, SIGNAL(triggered()), this, SLOT(focusVerticalCrossSectionWindow()));

	connect(ui->viewToolbarAction, SIGNAL(toggled(bool)), this, SLOT(viewToggleToolBar(bool)));
	connect(ui->viewStatusBarAction, SIGNAL(toggled(bool)), this, SLOT(viewToggleStatusBar(bool)));

	connect(ui->mouseHintAction, SIGNAL(triggered()), this, SLOT(helpMouseHint()));
}
