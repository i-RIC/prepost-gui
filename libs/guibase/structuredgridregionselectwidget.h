#ifndef STRUCTUREDGRIDREGIONSELECTWIDGET_H
#define STRUCTUREDGRIDREGIONSELECTWIDGET_H

#include "guibase_global.h"

#include "structuredgridregion.h"
#include <QWidget>

namespace Ui
{
	class StructuredGridRegionSelectWidget;
}
/// Widget to select two-dimensional structured grid region
class GUIBASEDLL_EXPORT StructuredGridRegionSelectWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StructuredGridRegionSelectWidget(QWidget* parent = nullptr);
	~StructuredGridRegionSelectWidget();
	/// Set the grid dimensions
	void setGridDimensions(int idim, int jdim);
	/// Set the iMin of the selected area
	void setiMin(int imin);
	/// Set the iMax of the selected area
	void setiMax(int imax);
	/// Set the jMin of the selected area
	void setjMin(int jmin);
	/// Set the jMax of the selected area
	void setjMax(int jmax);
	/// Set the selected region
	void setRegion(const StructuredGridRegion::Range2d& region);
	/// Apply setting to select whole grid region
	void selectAll();

	/// iMin of the selected area
	int iMin() const;
	/// iMax of the selected area
	int iMax() const;
	/// jMin of the selected area
	int jMin() const;
	/// jMax of the selected area
	int jMax() const;
	/// Selected area region
	StructuredGridRegion::Range2d region() const;

private slots:
	void handleIMinChange(int imin);
	void handleIMaxChange(int imax);
	void handleJMinChange(int jmin);
	void handleJMaxChange(int jmax);

private:
	StructuredGridRegion::Range2d m_region;

	Ui::StructuredGridRegionSelectWidget* ui;
};

#endif // STRUCTUREDGRIDREGIONSELECTWIDGET_H
