#ifndef VTKGRAPHICSVIEWSCALEWIDGET_H
#define VTKGRAPHICSVIEWSCALEWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class VtkGraphicsViewScaleWidget;
}

class VTKGraphicsView;

class GUICOREDLL_EXPORT VtkGraphicsViewScaleWidget : public QWidget
{
	Q_OBJECT

public:
	VtkGraphicsViewScaleWidget(QWidget *parent);
	~VtkGraphicsViewScaleWidget();

	void setView(VTKGraphicsView* view);

public slots:
	void updateDisplay();

private slots:
	void editScale();

private:
	VTKGraphicsView* m_view;
	Ui::VtkGraphicsViewScaleWidget *ui;

	class ScaleEditDialog;
};

#endif // VTKGRAPHICSVIEWSCALEWIDGET_H
