#ifndef PREPROCESSORWINDOW_H
#define PREPROCESSORWINDOW_H

#include "../viewwindowi.h"

#include <QMainWindow>

class CrossSection;
class Project;

class PreProcessorWindow : public QMainWindow, public ViewWindowI
{
	Q_OBJECT

public:
	explicit PreProcessorWindow(QWidget *parent = 0);
	~PreProcessorWindow();

	void setProject(Project* project);
	void setCrossSections(const std::vector<CrossSection*>& crossSections);

	void fit() override;
	void resetRotation() override;
	void rotate90() override;

	void zoomIn() override;
	void zoomOut() override;

	void importElevation();
	void importWaterSurfaceElevation();
	void importBaseLine();
	void exportBaseLine();

	void importCrossSections();
	void exportCrossSections();

	void crossSectionAdd();
	void crossSectionDelete();
	void crossSectionEditCoordinates();

	void baseLineAddVertex();
	void baseLineRemoveVertex();
	void baseLineEditCoordinates();
	void baseLineReverseDirection();
	void baseLineDelete();

private:
	void closeEvent(QCloseEvent *e);

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/preprocessorwindow_impl.h"
#endif // _DEBUG

#endif // PREPROCESSORWINDOW_H
