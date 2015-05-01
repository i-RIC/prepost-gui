#ifndef SCALARSTOCOLORSCONTAINER_H
#define SCALARSTOCOLORSCONTAINER_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

class vtkScalarsToColors;

class GUICOREDLL_EXPORT ScalarsToColorsContainer : public ProjectDataItem
{
	Q_OBJECT
public:
	ScalarsToColorsContainer(ProjectDataItem* d) : ProjectDataItem(d) {
		m_vtkObj = nullptr;
		m_vtkDarkObj = nullptr;
	}
	virtual ~ScalarsToColorsContainer();
	vtkScalarsToColors* vtkObj() {
		return m_vtkObj;
	}
	vtkScalarsToColors* vtkDarkObj() {
		return m_vtkDarkObj;
	}
	virtual void update() = 0;
	virtual void setValueRange(double /*min*/, double /*max*/) {}
	virtual void getValueRange(double* /*min*/, double* /*max*/) {}

signals:
	void rangeChanged();

protected:
	vtkScalarsToColors* m_vtkObj;
	vtkScalarsToColors* m_vtkDarkObj;
};

#endif // SCALARSTOCOLORSCONTAINER_H
