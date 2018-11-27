#ifndef GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"
#include "graph2dhybridwindowresultdataitem.h"

class vtkStructuredGrid;

class Graph2dHybridWindowGridIJKResultDataItem : public Graph2dHybridWindowResultDataItem
{

public:
	Graph2dHybridWindowGridIJKResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dHybridWindowGridIJKResultDataItem();
	Graph2dHybridWindowResultCopyDataItem* copy(Graph2dHybridWindowResultCopyGroupDataItem* parent) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateValues(int fn) override;
	void updateValuesVertex(vtkStructuredGrid* extractedGrid);
	void updateValuesCellCenter(vtkStructuredGrid* extractedGrid);
	void updateValuesCellCenterStepWise(vtkStructuredGrid* extractedGrid);

private:
	QString m_physVal;
};

#endif // GRAPH2DHYBRIDWINDOWGRIDIJKRESULTDATAITEM_H
