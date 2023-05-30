#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guibase/gridshape/gridshapesettingcontainer.h>

class Post2dWindowZoneDataItem;

class Post2dWindowGridShapeDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowGridShapeDataItem();

	void update();
	void handleStandardItemDoubleClicked() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void setupActors();
	void updateActorSetting() override;
	Post2dWindowZoneDataItem* zoneDataItem() const;

	GridShapeSettingContainer m_setting;

private:
	class SettingEditWidget;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_H
