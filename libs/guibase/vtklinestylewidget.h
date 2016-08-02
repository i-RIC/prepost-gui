#ifndef VTKLINESTYLEWIDGET_H
#define VTKLINESTYLEWIDGET_H

#include "guibase_global.h"

#include <QWidget>

class vtkLineStyleContainer;

namespace Ui {
class vtkLineStyleWidget;
}

class GUIBASEDLL_EXPORT vtkLineStyleWidget : public QWidget
{
	Q_OBJECT
public:
	explicit vtkLineStyleWidget(QWidget *parent = 0);
	~vtkLineStyleWidget();

	vtkLineStyleContainer lineStyle() const;
	void setLineStyle(const vtkLineStyleContainer& c);

private:
	Ui::vtkLineStyleWidget *ui;
};

#endif // VTKLINESTYLEWIDGET_H
