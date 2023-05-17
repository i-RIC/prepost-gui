#ifndef GRIDBIRDEYEWINDOWPROJECTDATAITEM_H
#define GRIDBIRDEYEWINDOWPROJECTDATAITEM_H

#include <guicore/project/projectdataitem.h>
#include <misc/windowgeometrycontainer.h>

class GridBirdEyeWindow;
class PreProcessorGridDataItem;

class GridBirdEyeWindowProjectDataItem : public ProjectDataItem
{
public:
	GridBirdEyeWindowProjectDataItem(PreProcessorGridDataItem* parent, QWidget* parentWindow);
	~GridBirdEyeWindowProjectDataItem();

	GridBirdEyeWindow* window() const;
	void informWindowClose();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	WindowGeometryContainer m_geometry;
	GridBirdEyeWindow* m_window;
};

#endif // GRIDBIRDEYEWINDOWPROJECTDATAITEM_H
