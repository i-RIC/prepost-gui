#ifndef POST2DWINDOWPROJECTDATAITEM_H
#define POST2DWINDOWPROJECTDATAITEM_H

#include "post2d_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <misc/windowgeometrycontainer.h>

class Post2dWindow;

class POST2D_EXPORT Post2dWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{

public:
	Post2dWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
	QString windowId() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	WindowGeometryContainer m_geometry;
};

#endif // POST2DWINDOWPROJECTDATAITEM_H
