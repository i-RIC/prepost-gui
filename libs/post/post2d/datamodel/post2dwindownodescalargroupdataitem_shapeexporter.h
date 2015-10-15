#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_SHAPEEXPORTER_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_SHAPEEXPORTER_H

#include "post2dwindownodescalargroupdataitem.h"

class Post2dWindowNodeScalarGroupDataItem::ShapeExporter
{
public:
	ShapeExporter(Post2dWindowNodeScalarGroupDataItem* parent);
	bool exportContourFigure(const QString& filename, double time);

private:
	static vtkPolyData* setupClippedPolygon(vtkPolyData* polyData, double lowValue, double highValue);

	Post2dWindowNodeScalarGroupDataItem* m_parent;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_SHAPEEXPORTER_H
