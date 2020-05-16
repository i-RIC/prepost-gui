#ifndef GRAPH2DVERIFICATIONWINDOWPROJECTDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWPROJECTDATAITEM_H

#include "graph2dverification_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <misc/windowgeometrycontainer.h>

class Graph2dVerificationWindow;

class GRAPH2DVERIFICATION_EXPORT Graph2dVerificationWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{

public:
	Graph2dVerificationWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
	bool setupInitialSetting();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	WindowGeometryContainer m_geometry;
};

#endif // GRAPH2DVERIFICATIONWINDOWPROJECTDATAITEM_H
