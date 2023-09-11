#ifndef COORDINATESYSTEMDISPLAYWIDGET_H
#define COORDINATESYSTEMDISPLAYWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class CoordinateSystemDisplayWidget;
}

class ProjectData;

class GUICOREDLL_EXPORT CoordinateSystemDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	explicit CoordinateSystemDisplayWidget(QWidget *parent = nullptr);
	~CoordinateSystemDisplayWidget();

	void setProjectData(ProjectData* data);
	void updateDisplay();

private slots:
	void editCoordinateSystem();

private:
	ProjectData* m_projectData;
	Ui::CoordinateSystemDisplayWidget *ui;
};

#endif // COORDINATESYSTEMDISPLAYWIDGET_H
