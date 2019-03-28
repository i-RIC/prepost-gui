#ifndef ATTRIBUTEBROWSERTARGETDATAITEM_H
#define ATTRIBUTEBROWSERTARGETDATAITEM_H

#include "../guicore_global.h"
#include "graphicswindowdataitem.h"

#include <guibase/vtktool/vtklineactor.h>
#include <guibase/vtktool/vtkpolygonsactor.h>

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
	void setPolygons(const std::vector<QPolygonF>& p);
	void setPolyline(const std::vector<QPointF>& l);
	void clear();

private:
	void setupActors();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkPolygonsActor m_polygonsActor;
	vtkLineActor m_lineActor;
};

#endif // ATTRIBUTEBROWSERTARGETDATAITEM_H
