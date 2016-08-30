#ifndef MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H
#define MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H

#include "../measureddatavectorgroupdataitem.h"
#include "../measureddatavectorsetting.h"

class vtkActor;
class vtkActor2D;
class vtkAppendPolyData;
class vtkConeSource;
class vtkGlyph3D;
class vtkHedgeHog;
class vtkPointSet;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTextActor;
class vtkUnstructuredGrid;
class vtkWarpVector;

class MeasuredDataVectorGroupDataItem::Impl
{
public:
	Impl(MeasuredDataVectorGroupDataItem* item);
	~Impl();

	void setupActors();

	void updatePolyData();
	void updateLegendData();
	void updateScaleFactor();
	void updateColorSetting();

	void calculateStandardValue();

	vtkPointSet* getPointSet();

	vtkActor* m_arrowActor;
	vtkPolyDataMapper* m_arrowMapper;

	vtkAppendPolyData* m_appendPolyData;
	vtkPolyData* m_polyData;

	vtkHedgeHog* m_hedgeHog;
	vtkGlyph3D* m_arrowGlyph;
	vtkWarpVector* m_warpVector;

	vtkPolyData* m_activePoints;
	vtkConeSource* m_arrowSource;

	vtkTextActor* m_legendTextActor;
	vtkActor2D* m_baseArrowActor;
	vtkUnstructuredGrid* m_baseArrowPolyData;

	MeasuredDataVectorSetting m_setting;
	double m_scaleFactor;

private:
	MeasuredDataVectorGroupDataItem* m_item;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H
