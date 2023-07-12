#ifndef MEASUREDDATAPOINTGROUPDATAITEM_H
#define MEASUREDDATAPOINTGROUPDATAITEM_H

#include "measureddata_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <guicore/misc/targeted/targeteditemi.h>

#include <string>
#include <unordered_map>

class ColorMapSettingContainerI;
class NamedGraphicWindowDataItem;

class MEASUREDDATA_API MeasuredDataPointGroupDataItem : public GraphicsWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	MeasuredDataPointGroupDataItem(GraphicsWindowDataItem* parent);
	~MeasuredDataPointGroupDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	QDialog* propertyDialog(QWidget* parent) override;

	bool hasTransparentPart() override;
	ColorMapSettingContainerI* colorMapSetting(const std::string& target) const;
	std::unordered_map<std::string, ColorMapSettingContainerI*> colorMapSettings() const;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doApplyOffset(double x, double y) override;
	ColorMapSettingContainerI* activeSetting() const;

public slots:
	void showPropertyDialog() override;
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	void updateActorSetting() override;

	class Impl;
	Impl* impl;

	class Setting;
	class SettingEditWidget;

	class PropertyDialog;
	class UpdateSettingCommand;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_H
