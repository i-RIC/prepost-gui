#ifndef POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_H
#define POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

#include <guibase/gridshape/gridshapesettingcontainer.h>
#include <misc/stringcontainer.h>

class Post2dBirdEyeWindowZoneDataItem;

class vtkWarpScalar;

class Post2dBirdEyeWindowGridShapeDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowGridShapeDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowGridShapeDataItem();

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
	void updateActorSettings();
	Post2dBirdEyeWindowZoneDataItem* zoneDataItem() const;

	vtkWarpScalar* m_gridWarp;
	vtkWarpScalar* m_labelWarp;

	GridShapeSettingContainer m_setting;
	StringContainer m_elevationTarget;

private:
	class PropertyDialog;
	class UpdateActorSettingsCommand;
};

#endif // POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_H
