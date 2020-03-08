#ifndef GRAPH2DHYBRIDWINDOWRESULTDATAITEM_H
#define GRAPH2DHYBRIDWINDOWRESULTDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"

#include <QVector>

class QwtPlotCustomCurve;
class QwtArrayData;
class Graph2dHybridWindowResultCopyDataItem;
class Graph2dHybridWindowResultCopyGroupDataItem;

class Graph2dHybridWindowResultDataItem : public Graph2dHybridWindowDataItem
{
public:
	Graph2dHybridWindowResultDataItem(const QString& title, int index, const Graph2dHybridWindowResultSetting::Setting& setting, Graph2dWindowDataItem* parent);
	virtual ~Graph2dHybridWindowResultDataItem();
	QwtPlotCustomCurve* curve() const {return m_curve;}
	void update(int fn);
	QString title() const;
	virtual Graph2dHybridWindowResultCopyDataItem* copy(Graph2dHybridWindowResultCopyGroupDataItem* parent);
	const Graph2dHybridWindowResultSetting::Setting& setting() const {return m_setting;}
	void setSetting(const Graph2dHybridWindowResultSetting::Setting& s);
	const QVector<double>& xValues() const {return m_xValues;}
	const QVector<double>& yValues() const {return m_yValues;}
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;

protected:
	virtual void updateValues(int fn) = 0;
	void setVisible(bool visible) override;
	QwtPlotCustomCurve* m_curve;
	QwtArrayData* m_data;

	Graph2dHybridWindowResultSetting::Setting m_setting;
	QVector<double> m_xValues;
	QVector<double> m_yValues;

private:
	bool m_attached;
};

#endif // GRAPH2DHYBRIDWINDOWRESULTDATAITEM_H
