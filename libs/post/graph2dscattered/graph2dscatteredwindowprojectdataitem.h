#ifndef GRAPH2DSCATTEREDWINDOWPROJECTDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWPROJECTDATAITEM_H

#include "graph2dscattered_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>

class Graph2dScatteredWindow;

class GRAPH2DSCATTERED_EXPORT Graph2dScatteredWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{
public:
	Graph2dScatteredWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
	bool setupInitialSetting();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
};

#endif // GRAPH2DSCATTEREDWINDOWPROJECTDATAITEM_H
