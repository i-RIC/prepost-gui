#ifndef POST3DWINDOWPROJECTDATAITEM_H
#define POST3DWINDOWPROJECTDATAITEM_H

#include "post3d_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>

class Post3dWindow;

class POST3D_EXPORT Post3dWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{

public:
	Post3dWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST3DWINDOWPROJECTDATAITEM_H
