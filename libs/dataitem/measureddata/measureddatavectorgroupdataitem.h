#ifndef MEASUREDDATAVECTORGROUPDATAITEM_H
#define MEASUREDDATAVECTORGROUPDATAITEM_H

#include "measureddata_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>
#include <guicore/misc/targeted/targeteditemi.h>

class NamedGraphicWindowDataItem;

class MEASUREDDATA_API MeasuredDataVectorGroupDataItem : public GraphicsWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	const static int STANDARD_LENGTH = 100;
	const static int AUTO_AVERAGECOUNT = 20;
	const static double MINLIMIT;

	MeasuredDataVectorGroupDataItem(GraphicsWindowDataItem* parent);
	~MeasuredDataVectorGroupDataItem();

	void updateActorSettings();
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();

	std::string target() const override;
	void setTarget(const std::string &target) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdate2Ds() override;
	void informGridUpdate();
	void doApplyOffset(double x, double y) override;

	class Impl;
	Impl* impl;

	class SetSettingCommand;
};

#ifdef _DEBUG
	#include "private/measureddatavectorgroupdataitem_impl.h"
#endif // _DEBUG

#endif // MEASUREDDATAVECTORGROUPDATAITEM_H
