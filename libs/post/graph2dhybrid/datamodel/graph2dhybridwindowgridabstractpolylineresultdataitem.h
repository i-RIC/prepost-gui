#ifndef GRAPH2DHYBRIDWINDOWGRIDABSTRACTPOLYLINERESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWGRIDABSTRACTPOLYLINERESULTDATAITEM_H

#include "../graph2dhybridwindowresultsetting.h"
#include "graph2dhybridwindowresultdataitem.h"

class vtkPointSet;

class Graph2dHybridWindowGridAbstractPolylineResultDataItem : public Graph2dHybridWindowResultDataItem
{
public:
	Graph2dHybridWindowGridAbstractPolylineResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowGridAbstractPolylineResultDataItem();

	Graph2dHybridWindowResultCopyDataItem* copy(Graph2dHybridWindowResultCopyGroupDataItem* parent) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateValues() override;

	void updateValuesVertex(vtkPointSet* grid);
	void updateValuesCellCenter(vtkPointSet* grid);

	virtual std::vector<QPointF> getPolyLine() = 0;

	QString m_physVal;
};

#endif // GRAPH2DHYBRIDWINDOWGRIDABSTRACTPOLYLINERESULTDATAITEM_H
