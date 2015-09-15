#ifndef GRAPH2DSCATTEREDWINDOWPROJECTDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWPROJECTDATAITEM_H

#include "graph2dscattered_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <misc/windowgeometrycontainer.h>

class Graph2dScatteredWindow;

class GRAPH2DSCATTERED_EXPORT Graph2dScatteredWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{

public:
	Graph2dScatteredWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
	bool setupInitialSetting();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	WindowGeometryContainer m_geometry;
};

#endif // GRAPH2DSCATTEREDWINDOWPROJECTDATAITEM_H
