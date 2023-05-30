#ifndef POST3DWINDOWGRIDSHAPEDATAITEM_H
#define POST3DWINDOWGRIDSHAPEDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guibase/gridshape/gridshapesettingcontainer.h>

class Post3dWindowZoneDataItem;

class Post3dWindowGridShapeDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowGridShapeDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowGridShapeDataItem();

	void update();
	void handleStandardItemDoubleClicked() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;

	void setupActors();
	void updateActorSetting() override;
	Post3dWindowZoneDataItem* zoneDataItem() const;

	GridShapeSettingContainer m_setting;

private:
	class SettingEditWidget;
};

#endif // POST3DWINDOWGRIDSHAPEDATAITEM_H
