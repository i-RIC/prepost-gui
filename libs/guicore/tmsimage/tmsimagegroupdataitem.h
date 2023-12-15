#ifndef TMSIMAGEGROUPDATAITEM_H
#define TMSIMAGEGROUPDATAITEM_H

#include "../guicore_global.h"
#include "../datamodel/graphicswindowdataitem.h"
#include "../misc/targeted/targetedqstringitemi.h"

class NamedQStringGraphicWindowDataItem;

class GUICOREDLL_EXPORT TmsImageGroupDataItem : public GraphicsWindowDataItem, public TargetedQStringItemI
{
	Q_OBJECT

public:
	TmsImageGroupDataItem(GraphicsWindowDataItem* parent);
	~TmsImageGroupDataItem();

	QString target() const override;
	void setTarget(const QString& target) override;

	void rebuildChildItems();

	void viewOperationEndedGlobal(VTKGraphicsView* v) override;

	void updateZDepthRangeItemCount() override;

public slots:
	void handleNamedItemChange(NamedQStringGraphicWindowDataItem* item);

private slots:
	void handleImageUpdate(int requestId);

protected:
	void doApplyOffset(double x_diff, double y_diff) override;

private:
	void requestImage();

	void assignActorZValues(const ZDepthRange& range) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/tmsimagegroupdataitem_impl.h"
#endif // _DEBUG

#endif // TMSIMAGEGROUPDATAITEM_H
