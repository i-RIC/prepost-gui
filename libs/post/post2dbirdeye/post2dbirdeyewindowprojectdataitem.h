#ifndef POST2DBIRDEYEWINDOWPROJECTDATAITEM_H
#define POST2DBIRDEYEWINDOWPROJECTDATAITEM_H

#include "post2dbirdeye_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>

class Post2dBirdEyeWindow;

class POST2DBIRDEYE_EXPORT Post2dBirdEyeWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{
public:
	Post2dBirdEyeWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST2DBIRDEYEWINDOWPROJECTDATAITEM_H
