#ifndef MEASUREDDATAVECTORGROUPDATAITEM_H
#define MEASUREDDATAVECTORGROUPDATAITEM_H

#include "measureddata_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <guicore/misc/targeted/targeteditemi.h>

class ColorMapSettingContainerI;
class MeasuredDataVectorSetting;
class NamedGraphicWindowDataItem;

class MEASUREDDATA_API MeasuredDataVectorGroupDataItem : public GraphicsWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	MeasuredDataVectorGroupDataItem(GraphicsWindowDataItem* parent);
	~MeasuredDataVectorGroupDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();

	std::string target() const override;
	void setTarget(const std::string &target) override;

public slots:
	void showPropertyDialog() override;
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	ColorMapSettingContainerI* colorMapSetting(const std::string& target);

	QDialog* propertyDialog(QWidget* p) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdate2Ds() override;
	void informGridUpdate();

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	void doApplyOffset(double x, double y) override;

	void updateActorSetting() override;

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
