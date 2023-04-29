#ifndef GRIDBIRDEYEWINDOWDATAMODEL_H
#define GRIDBIRDEYEWINDOWDATAMODEL_H

#include "gridbirdeyewindowsetting.h"

#include <guicore/datamodel/graphics3dwindowdatamodel.h>

class GridBirdEyeWindow;
class GridBirdEyeWindowGraphicsView;
class GridBirdEyeWindowZoneDataItem;

class PreProcessorGridDataItem;
class PreProcessorGridTypeDataItem;

class GridBirdEyeWindowDataModel : public Graphics3DWindowDataModel
{
	Q_OBJECT

public:
	GridBirdEyeWindowDataModel(GridBirdEyeWindow* w, ProjectDataItem* parent);
	~GridBirdEyeWindowDataModel();

	GridBirdEyeWindowGraphicsView* graphicsView() const;
	void editZScale();
	void editDisplaySetting();

	void updateGrid();

	PreProcessorGridTypeDataItem* gridTypeDataItem() const;
	PreProcessorGridDataItem* gridDataItem() const;
	GridBirdEyeWindowZoneDataItem* zoneDataItem() const;

	void handleResize(QResizeEvent* event) override;

private:
	void init(GridBirdEyeWindow* w);

	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	double m_zScale;
};

#endif // GRIDBIRDEYEWINDOWDATAMODEL_H
