#ifndef GRIDBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H
#define GRIDBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H

#include "../gridbirdeyewindowdataitem.h"

class GridBirdEyeWindowCellScalarGroupDataItem;
class GridBirdEyeWindowZoneDataItem;

class GridBirdEyeWindowCellScalarGroupTopDataItem : public GridBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	GridBirdEyeWindowCellScalarGroupTopDataItem(GridBirdEyeWindowDataItem* parent);
	~GridBirdEyeWindowCellScalarGroupTopDataItem();

	void update();
	GridBirdEyeWindowZoneDataItem* zoneDataItem() const;

private:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* addDialog) override;
	void innerUpdateZScale(double scale) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	double m_zScale;

	friend class GridBirdEyeWindowCellScalarGroupDataItem;
};

#endif // GRIDBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H
