#ifndef VTK2DGRAPHICSVIEWANGLEWIDGET_H
#define VTK2DGRAPHICSVIEWANGLEWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class Vtk2dGraphicsViewAngleWidget;
}

class VTK2DGraphicsView;

class GUICOREDLL_EXPORT Vtk2dGraphicsViewAngleWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Vtk2dGraphicsViewAngleWidget(QWidget *parent);
	~Vtk2dGraphicsViewAngleWidget();

	void setView(VTK2DGraphicsView* view);

public slots:
	void updateDisplay();

private slots:
	void editAngle();

private:
	VTK2DGraphicsView* m_view;
	Ui::Vtk2dGraphicsViewAngleWidget *ui;

	class AngleEditDialog;
};

#endif // VTK2DGRAPHICSVIEWANGLEWIDGET_H
