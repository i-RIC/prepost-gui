#ifndef RIVMAKERMAINWINDOW_H
#define RIVMAKERMAINWINDOW_H

#include <QMainWindow>

#include <vector>

class QCloseEvent;
class ViewWindowI;

namespace Ui {
class RivmakerMainWindow;
}

class RivmakerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit RivmakerMainWindow(QWidget *parent = 0);
	~RivmakerMainWindow();

	void openProject(const QString& filename);
	void exportWaterSurfaceElevationData();

private slots:
	void newProject();
	void openProject();
	bool saveProject();
	bool saveProjectAs();
	void openRecentProject();

	void importElevation();
	void importWaterSurfaceElevation();
	void importSACGUIFile();
	void importBaseLine();
	void importCrossSectionLines();

	void exportBaseLine();
	void exportCrossSectionLines();
	void exportRiverSurveyData();
	void exportRiverSurveyCsvData();

	void fit();
	void resetRotation();
	void rotate90();
	void zoomIn();
	void zoomOut();

	void focusPreProcessorWindow();
	void focusVerticalCrossSectionWindow();
	void openCrossSectionWindow();

	void baseLineAddPoint();
	void baseLineRemovePoint();
	void baseLineEditCoordinates();
	void baseLineReverseDirection();
	void baseLineDelete();

	void crossSectionAdd();
	void crossSectionEditCoordinates();
	void crossSectionDelete();

	void viewToggleMainToolBar(bool visible);
	void viewToggleWindowsToolBar(bool visible);
	void viewToggleStatusBar(bool visible);

	void optionMappingSetting();
	void optionGraphicsSetting();

	void helpMouseHint();
	void showAboutDialog();

	void updateViewMenu();

	void updateWindowsToolBar();
	void activateWindow(QWidget* w);

	void setupRecentProjectsMenu();

private:
	bool closeProject();

	void updateWindowTitle();

	void updateRecentProjects(const QString& filename);
	void removeFromRecentProjects(const QString& filename);

	void closeEvent(QCloseEvent *e) override;

	void setupConnections();
	void deleteProject();

	ViewWindowI* currentViewWindow() const;

	class Impl;
	Impl* impl;

	Ui::RivmakerMainWindow *ui;
};

#ifdef _DEBUG
	#include "private/rivmakermainwindow_impl.h"
#endif // _DEBUG

#endif // RIVMAKERMAINWINDOW_H
