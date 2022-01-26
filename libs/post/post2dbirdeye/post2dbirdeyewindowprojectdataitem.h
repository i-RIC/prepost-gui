#ifndef POST2DBIRDEYEWINDOWPROJECTDATAITEM_H
#define POST2DBIRDEYEWINDOWPROJECTDATAITEM_H

#include "post2dbirdeye_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <misc/windowgeometrycontainer.h>

class Post2dBirdEyeWindow;

class POST2DBIRDEYE_EXPORT Post2dBirdEyeWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{

public:
	Post2dBirdEyeWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
	QString windowId() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	WindowGeometryContainer m_geometry;
};

#endif // POST2DBIRDEYEWINDOWPROJECTDATAITEM_H
