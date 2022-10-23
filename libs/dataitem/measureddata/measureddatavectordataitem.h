#ifndef MEASUREDDATAVECTORDATAITEM_H
#define MEASUREDDATAVECTORDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class MeasuredDataVectorSetting;
class MeasuredDataPointGroupDataItem;

class MeasuredDataVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	MeasuredDataVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void handleResize(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

private:
	MeasuredDataPointGroupDataItem* pointGroupDataItem() const;
	MeasuredDataVectorSetting& setting();
};

#endif // MEASUREDDATAVECTORDATAITEM_H
