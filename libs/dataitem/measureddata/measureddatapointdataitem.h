#ifndef MEASUREDDATAPOINTDATAITEM_H
#define MEASUREDDATAPOINTDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class ColorMapSettingContainer;

class MeasuredDataPointDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	MeasuredDataPointDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void handleResize(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

private:
	ColorMapSettingContainer* activeSetting() const;
};

#endif // MEASUREDDATAPOINTDATAITEM_H
