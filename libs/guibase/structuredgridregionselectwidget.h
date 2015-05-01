#ifndef STRUCTUREDGRIDREGIONSELECTWIDGET_H
#define STRUCTUREDGRIDREGIONSELECTWIDGET_H

#include "guibase_global.h"

#include "structuredgridregion.h"
#include <QWidget>

namespace Ui
{
	class StructuredGridRegionSelectWidget;
}

class GUIBASEDLL_EXPORT StructuredGridRegionSelectWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StructuredGridRegionSelectWidget(QWidget* parent = nullptr);
	~StructuredGridRegionSelectWidget();
	void setGridDimensions(int idim, int jdim);
	void setiMin(int imin);
	void setiMax(int imax);
	void setjMin(int jmin);
	void setjMax(int jmax);
	void setRegion(const StructuredGridRegion::Range2d& region);
	void selectAll();

	int iMin();
	int iMax();
	int jMin();
	int jMax();
	StructuredGridRegion::Range2d region();

private slots:
	void handleIMinChange(int imin);
	void handleIMaxChange(int imax);
	void handleJMinChange(int jmin);
	void handleJMaxChange(int jmax);

private:
	Ui::StructuredGridRegionSelectWidget* ui;
};

#endif // STRUCTUREDGRIDREGIONSELECTWIDGET_H
