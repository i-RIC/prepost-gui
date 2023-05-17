#ifndef GRIDBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H
#define GRIDBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H

#include "../gridbirdeyewindowdataitem.h"

class GridBirdEyeWindowNodeScalarGroupDataItem;
class GridBirdEyeWindowZoneDataItem;

class GridBirdEyeWindowNodeScalarGroupTopDataItem : public GridBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	GridBirdEyeWindowNodeScalarGroupTopDataItem(GridBirdEyeWindowDataItem* parent);
	~GridBirdEyeWindowNodeScalarGroupTopDataItem();

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

	friend class GridBirdEyeWindowNodeScalarGroupDataItem;
};

#endif // GRIDBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H
