#ifndef MEASUREDDATAVECTORGROUPDATAITEM_H
#define MEASUREDDATAVECTORGROUPDATAITEM_H

#include "measureddata_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <guicore/misc/targeted/targeteditemi.h>

class ColorMapSettingContainer;
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

	MeasuredDataVectorSetting& setting();

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	ColorMapSettingContainer* colorMapSetting(const std::string& target);

	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdate2Ds() override;
	void informGridUpdate();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void handleResize(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void doApplyOffset(double x, double y) override;

	class Impl;
	Impl* impl;

	class PropertyDialog;
	class UpdateSettingCommand;
};

#ifdef _DEBUG
	#include "private/measureddatavectorgroupdataitem_impl.h"
#endif // _DEBUG

#endif // MEASUREDDATAVECTORGROUPDATAITEM_H
