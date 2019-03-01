#ifndef GRAPH2DHYBRIDWINDOWGRIDPOLYLINERESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWGRIDPOLYLINERESULTDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"
#include "graph2dhybridwindowresultdataitem.h"

class vtkStructuredGrid;
class vtkUnstructuredGrid;
class vtkPointSet;

class Graph2dHybridWindowGridPolylineResultDataItem : public Graph2dHybridWindowResultDataItem
{

public:
	Graph2dHybridWindowGridPolylineResultDataItem(const Graph2dHybridWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dHybridWindowGridPolylineResultDataItem();
	Graph2dHybridWindowResultCopyDataItem* copy(Graph2dHybridWindowResultCopyGroupDataItem* parent) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateValues(int fn) override;

	void updateValuesVertex(vtkPointSet* grid);
	void updateValuesCellCenter(vtkPointSet* grid);

private:
	QString m_physVal;
};

#endif // GRAPH2DHYBRIDWINDOWGRIDPOLYLINERESULTDATAITEM_H
