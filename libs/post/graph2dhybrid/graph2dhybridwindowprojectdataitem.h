#ifndef GRAPH2DHYBRIDWINDOWPROJECTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWPROJECTDATAITEM_H

#include "graph2dhybrid_global.h"
#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <misc/windowgeometrycontainer.h>

class Graph2dHybridWindow;

class GRAPH2DHYBRID_EXPORT Graph2dHybridWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{

public:
	Graph2dHybridWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
	bool setupInitialSetting();
	void setupInitialSettingForNode(const std::string& zoneName, vtkIdType index, const std::vector<std::string>& values);
	void setupInitialSettingForCell(const std::string& zoneName, vtkIdType index, const std::vector<std::string>& values);
	void setupInitialSettingForIFace(const std::string& zoneName, vtkIdType index, const std::vector<std::string>& values);
	void setupInitialSettingForJFace(const std::string& zoneName, vtkIdType index, const std::vector<std::string>& values);
	QString windowId() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	WindowGeometryContainer m_geometry;
};

#endif // GRAPH2DHYBRIDWINDOWPROJECTDATAITEM_H
