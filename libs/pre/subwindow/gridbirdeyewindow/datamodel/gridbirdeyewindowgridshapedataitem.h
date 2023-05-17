#ifndef GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_H
#define GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_H

#include "../gridbirdeyewindowdataitem.h"

class GridBirdEyeWindowZoneDataItem;

class vtkWarpScalar;

class GridBirdEyeWindowGridShapeDataItem : public GridBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	GridBirdEyeWindowGridShapeDataItem(GridBirdEyeWindowDataItem* parent);
	~GridBirdEyeWindowGridShapeDataItem();

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
	GridBirdEyeWindowZoneDataItem* zoneDataItem() const;

	vtkWarpScalar* m_gridWarp;
	vtkWarpScalar* m_labelWarp;

private:
	class Impl;
	Impl* impl;

	class Setting;
	class SettingEditWidget;
};

#endif // GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_H
