#if SKIP
#ifndef GRAPH2DVERIFICATIONWINDOWGRIDPOLYLINERESULTDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWGRIDPOLYLINERESULTDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include "../graph2dverificationwindowresultsetting.h"
#include "graph2dverificationwindowresultdataitem.h"

class vtkStructuredGrid;
class vtkUnstructuredGrid;
class vtkPointSet;

class Graph2dVerificationWindowGridPolylineResultDataItem : public Graph2dVerificationWindowResultDataItem
{

public:
	Graph2dVerificationWindowGridPolylineResultDataItem(const Graph2dVerificationWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dVerificationWindowGridPolylineResultDataItem();
	Graph2dVerificationWindowResultCopyDataItem* copy(Graph2dVerificationWindowResultCopyGroupDataItem* parent) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateValues(int fn) override;

	void updateValuesVertex(vtkPointSet* grid);
	void updateValuesCellCenter(vtkPointSet* grid);

private:
	QString m_physVal;
};

#endif // GRAPH2DVERIFICATIONWINDOWGRIDPOLYLINERESULTDATAITEM_H
#endif
