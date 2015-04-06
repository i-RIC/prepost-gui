#ifndef POST2DWINDOWPROJECTDATAITEM_H
#define POST2DWINDOWPROJECTDATAITEM_H

#include "post2d_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>

class Post2dWindow;

class POST2D_EXPORT Post2dWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{
public:
	Post2dWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
};

#endif // POST2DWINDOWPROJECTDATAITEM_H
