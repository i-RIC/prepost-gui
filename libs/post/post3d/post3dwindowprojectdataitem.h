#ifndef POST3DWINDOWPROJECTDATAITEM_H
#define POST3DWINDOWPROJECTDATAITEM_H

#include "post3d_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <misc/windowgeometrycontainer.h>

class Post3dWindow;

class POST3D_EXPORT Post3dWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{

public:
	Post3dWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
	QString windowId() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	WindowGeometryContainer m_geometry;
};

#endif // POST3DWINDOWPROJECTDATAITEM_H
