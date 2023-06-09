#ifndef MEASUREDDATAPOINTDATAITEM_H
#define MEASUREDDATAPOINTDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class ColorMapSettingContainer;
class MeasuredDataPointGroupDataItem;

class MeasuredDataPointDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	MeasuredDataPointDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

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

	MeasuredDataPointGroupDataItem* groupDataItem() const;
	ColorMapSettingContainer* activeSetting() const;
};

#endif // MEASUREDDATAPOINTDATAITEM_H
