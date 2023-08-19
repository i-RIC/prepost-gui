#ifndef GRAPH2DSCATTEREDWINDOWGRIDRESULTDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWGRIDRESULTDATAITEM_H

#include "../graph2dscatteredwindowdataitem.h"
#include "../graph2dscatteredwindowresultsetting.h"
#include "graph2dscatteredwindowresultdataitem.h"

#include <vector>

class vtkPointSet;

class Graph2dScatteredWindowGridResultDataItem : public Graph2dScatteredWindowResultDataItem
{

public:
	Graph2dScatteredWindowGridResultDataItem(const Graph2dScatteredWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dScatteredWindowGridResultDataItem();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateValues() override;

	static void setupData(std::vector<double>& vals, const std::string& name, vtkPointSet* ps);

	std::string m_yAxis;
};

#endif // GRAPH2DSCATTEREDWINDOWGRIDRESULTDATAITEM_H
