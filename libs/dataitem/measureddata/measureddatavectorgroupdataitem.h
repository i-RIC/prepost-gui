#ifndef MEASUREDDATAVECTORGROUPDATAITEM_H
#define MEASUREDDATAVECTORGROUPDATAITEM_H

#include "measureddata_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class ColorMapSettingContainerI;
class MeasuredDataVectorGroupTopDataItem;

class MEASUREDDATA_API MeasuredDataVectorGroupDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	MeasuredDataVectorGroupDataItem(const std::string& target, GraphicsWindowDataItem* parent);
	~MeasuredDataVectorGroupDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();

	std::string target() const;

	ColorMapSettingContainerI* colorMapSetting(const std::string& target) const;
	ColorMapSettingContainerI* activeColorMapSetting() const;
	ColorMapSettingContainerI* activeColorMapSettingWithVisibleLegend() const;

public slots:
	void showPropertyDialog() override;

private:
	QDialog* propertyDialog(QWidget* p) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdate2Ds() override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	void doApplyOffset(double x, double y) override;
	void handleStandardItemChange() override;

	void updateActorSetting() override;
	void updateVisibility(bool visible) override;
	MeasuredDataVectorGroupTopDataItem* topDataItem() const;

	class Impl;
	Impl* impl;

	class PropertyDialog;
	class Setting;
	class SettingEditWidget;
	class UpdateSettingCommand;

public:
	friend class MeasuredDataVectorDataItem;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_H
