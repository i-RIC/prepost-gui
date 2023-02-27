#ifndef GRIDBIRDEYEWINDOWDATAMODEL_H
#define GRIDBIRDEYEWINDOWDATAMODEL_H

#include "gridbirdeyewindowsetting.h"

#include <guicore/datamodel/graphicswindowsimpledatamodel.h>

class GridBirdEyeWindow;
class GridBirdEyeWindowGridDataItem;
class GridBirdEyeWindowGraphicsView;

class PreProcessorGridDataItem;
class PreProcessorGridTypeDataItem;

class GridBirdEyeWindowDataModel : public GraphicsWindowSimpleDataModel
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

	void handleResize() override;

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	GridBirdEyeWindowGridDataItem* m_gridDataItem;
};

#endif // GRIDBIRDEYEWINDOWDATAMODEL_H
