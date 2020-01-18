#ifndef TMSIMAGEGROUPDATAITEM_H
#define TMSIMAGEGROUPDATAITEM_H

#include "../guicore_global.h"
#include "../datamodel/graphicswindowdataitem.h"
#include "../misc/targeted/targeteditemi.h"

class NamedGraphicWindowDataItem;

class GUICOREDLL_EXPORT TmsImageGroupDataItem : public GraphicsWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	TmsImageGroupDataItem(GraphicsWindowDataItem* parent);
	~TmsImageGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string &target) override;

	void rebuildChildItems();

	void viewOperationEndedGlobal(VTKGraphicsView* v) override;

	void updateZDepthRangeItemCount() override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private slots:
	void handleImageUpdate(int requestId);

protected:
	void doApplyOffset(double x_diff, double y_diff) override;

private:
	void requestImage();

	void assignActorZValues(const ZDepthRange& range) override;
	void doHandleResize(VTKGraphicsView* v) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/tmsimagegroupdataitem_impl.h"
#endif // _DEBUG

#endif // TMSIMAGEGROUPDATAITEM_H
