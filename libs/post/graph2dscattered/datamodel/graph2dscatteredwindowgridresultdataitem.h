#ifndef GRAPH2DSCATTEREDWINDOWGRIDRESULTDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWGRIDRESULTDATAITEM_H

#include "../graph2dscatteredwindowdataitem.h"
#include "../graph2dscatteredwindowresultsetting.h"
#include "graph2dscatteredwindowresultdataitem.h"
#include <QVector>

class vtkPointSet;

class Graph2dScatteredWindowGridResultDataItem : public Graph2dScatteredWindowResultDataItem
{

public:
	Graph2dScatteredWindowGridResultDataItem(const Graph2dScatteredWindowResultSetting::Setting& setting, int index, Graph2dWindowDataItem* parent);
	virtual ~Graph2dScatteredWindowGridResultDataItem();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateValues(int fn) override;

private:
	static void setupData(QVector<double>& vals, const QString& name, vtkPointSet* ps);

	QString m_yAxis;
};

#endif // GRAPH2DSCATTEREDWINDOWGRIDRESULTDATAITEM_H
