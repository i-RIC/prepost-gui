#ifndef GRAPH2DVERIFICATIONWINDOWGRIDIJKRESULTDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWGRIDIJKRESULTDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include "../graph2dverificationwindowresultsetting.h"
#include "graph2dverificationwindowresultdataitem.h"

class vtkStructuredGrid;

class Graph2dVerificationWindowGridIJKResultDataItem : public Graph2dVerificationWindowResultDataItem
{

public:
	Graph2dVerificationWindowGridIJKResultDataItem(const Graph2dVerificationWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dVerificationWindowGridIJKResultDataItem();
	Graph2dVerificationWindowResultCopyDataItem* copy(Graph2dVerificationWindowResultCopyGroupDataItem* parent) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateValues(int fn) override;
	//void updateValuesVertex(vtkStructuredGrid* extractedGrid);
	//void updateValuesCellCenter(vtkStructuredGrid* extractedGrid);
	//void updateValuesCellCenterStepWise(vtkStructuredGrid* extractedGrid);

private:
	QString m_physVal;
};

#endif // GRAPH2DVERIFICATIONWINDOWGRIDIJKRESULTDATAITEM_H
