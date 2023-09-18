#ifndef POST2DWINDOWGRAPHGROUPDATAITEM_H
#define POST2DWINDOWGRAPHGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>

#include <memory>

class NamedGraphicWindowDataItem;
class Post2dWindowCalculationResultDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowGraphGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowGraphGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowGraphGroupDataItem();

	void update();

	std::string target() const override;
	void setTarget(const std::string& target) override;
	void setTargetWithoutSignal(const std::string& target);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void setDefaultSetting();
	Post2dWindowCalculationResultDataItem* resultDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	void assignActorZValues(const ZDepthRange& range) override;
	void updateZDepthRangeItemCount() override;

	void innerUpdateZScale(double scale) override;

	class Impl;
	std::unique_ptr<Impl> impl;

	class SetSettingCommand;
};

#ifdef _DEBUG
	#include "private/post2dwindowgraphgroupdataitem_impl.h"
#endif // _DEBUG

#endif // POST2DWINDOWGRAPHGROUPDATAITEM_H
