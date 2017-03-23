#include "rivmakermainwindow.h"
#include "ui_rivmakermainwindow.h"

void RivmakerMainWindow::setupConnections()
{
	connect(ui->newProjectAction, SIGNAL(triggered()), this, SLOT(newProject()));
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

	connect(ui->fileExportBaseLineAction, SIGNAL(triggered()), this, SLOT(exportBaseLine()));
	connect(ui->baseLineExportAction, SIGNAL(triggered()), this, SLOT(exportBaseLine()));

	connect(ui->fileExportCrosssectionLineAction, SIGNAL(triggered()), this, SLOT(exportCrossSectionLines()));
	connect(ui->crosssectionLineExportAction, SIGNAL(triggered()), this, SLOT(exportCrossSectionLines()));

	connect(ui->fileExportRiverSurveyDataAction, SIGNAL(triggered()), this, SLOT(exportRiverSurveyData()));
	connect(ui->riverSurveyDataExportAction, SIGNAL(triggered()), this, SLOT(exportRiverSurveyData()));

	connect(ui->fitAction, SIGNAL(triggered()), this, SLOT(fit()));
	connect(ui->resetRotationAction, SIGNAL(triggered()), this, SLOT(resetRotation()));
	connect(ui->rotate90Action, SIGNAL(triggered()), this, SLOT(rotate90()));
	connect(ui->zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
	connect(ui->zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

	connect(ui->focusPreWindowAction, SIGNAL(triggered()), this, SLOT(focusPreProcessorWindow()));
	connect(ui->focusVerticalCrosssectionWindowAction, SIGNAL(triggered()), this, SLOT(focusVerticalCrossSectionWindow()));

	connect(ui->baseLineAddVertexAction, SIGNAL(triggered()), this, SLOT(baseLineAddPoint()));
	connect(ui->baseLineRemoveVertexAction, SIGNAL(triggered()), this, SLOT(baseLineRemovePoint()));
	connect(ui->baseLineEditCoordinatesAction, SIGNAL(triggered()), this, SLOT(baseLineEditCoordinates()));
	connect(ui->baseLineReverseDirectionAction, SIGNAL(triggered()), this, SLOT(baseLineReverseDirection()));
	connect(ui->baseLineDeleteAction, SIGNAL(triggered()), this, SLOT(baseLineDelete()));

	connect(ui->crosssectionLineAddAction, SIGNAL(triggered()), this, SLOT(crossSectionAdd()));
	connect(ui->crosssectionLineEditCoordinatesAction, SIGNAL(triggered()), this, SLOT(crossSectionEditCoordinates()));
	connect(ui->crosssectionLineDeleteAction, SIGNAL(triggered()), this, SLOT(crossSectionDelete()));
	connect(ui->openCrosssectionLineWindowAction, SIGNAL(triggered()), this, SLOT(openCrossSectionWindow()));
	connect(ui->openCrosssectionWindowAction, SIGNAL(triggered()), this, SLOT(openCrossSectionWindow()));

	connect(ui->viewToolbarAction, SIGNAL(toggled(bool)), this, SLOT(viewToggleToolBar(bool)));
	connect(ui->viewStatusBarAction, SIGNAL(toggled(bool)), this, SLOT(viewToggleStatusBar(bool)));

	connect(ui->mouseHintAction, SIGNAL(triggered()), this, SLOT(helpMouseHint()));
	connect(ui->aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

	connect(ui->viewMenu, SIGNAL(aboutToShow()), this, SLOT(updateViewMenu()));
}
