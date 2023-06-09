#ifndef MEASUREDDATAVECTORDATAITEM_H
#define MEASUREDDATAVECTORDATAITEM_H

#include "measureddatavectorgroupdataitem.h"

#include <guicore/named/namedgraphicwindowdataitem.h>

class MeasuredDataPointGroupDataItem;
class MeasuredDataVectorGroupDataItem;

class MeasuredDataVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	MeasuredDataVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	QDialog* propertyDialog(QWidget* parent) override;

public slots:
	void showPropertyDialog() override;

private:
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	MeasuredDataPointGroupDataItem* pointGroupDataItem() const;
	MeasuredDataVectorGroupDataItem* groupDataItem() const;

	MeasuredDataVectorGroupDataItem::Setting& setting();
};

#endif // MEASUREDDATAVECTORDATAITEM_H
