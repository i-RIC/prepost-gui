#ifndef SCALARSTOCOLORSCONTAINER_H
#define SCALARSTOCOLORSCONTAINER_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

class vtkScalarsToColors;

class GUICOREDLL_EXPORT ScalarsToColorsContainer : public ProjectDataItem
{
	Q_OBJECT

public:
	ScalarsToColorsContainer(ProjectDataItem* d);
	virtual ~ScalarsToColorsContainer();

	vtkScalarsToColors* vtkObj() const;
	vtkScalarsToColors* vtkDarkObj() const;

	virtual void update() = 0;

	virtual void getValueRange(double* min, double* max) const;
	virtual void setValueRange(double min, double max);

signals:
	void rangeChanged();

protected:
	void setVtkObj(vtkScalarsToColors* obj);
	void setVtkDarkObj(vtkScalarsToColors* obj);

private:
	vtkScalarsToColors* m_vtkObj;
	vtkScalarsToColors* m_vtkDarkObj;
};

#endif // SCALARSTOCOLORSCONTAINER_H
