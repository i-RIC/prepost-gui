#ifndef ATTRIBUTEBROWSERTARGETDATAITEM_H
#define ATTRIBUTEBROWSERTARGETDATAITEM_H

#include "../guicore_global.h"
#include "graphicswindowdataitem.h"

#include <guibase/polygon/polygoncontroller.h>
#include <guibase/polyline/polylinecontroller.h>

class QPointF;
class QPolygonF;

class vtkCell;

class GUICOREDLL_EXPORT AttributeBrowserTargetDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	AttributeBrowserTargetDataItem(GraphicsWindowDataItem* parent);
	~AttributeBrowserTargetDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;

	void setPoint(const QPointF& v);
	void setPolygon(const QPolygonF& p);
	void setPolyline(const std::vector<QPointF>& l);
	void setCell(vtkCell* cell);
	void clear();

private:
	void setupActors();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	PolygonController m_polygonController;
	PolyLineController m_polylineController;
};

#endif // ATTRIBUTEBROWSERTARGETDATAITEM_H
