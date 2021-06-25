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

	void update();

	QwtPlotCustomCurve* curve() const;
	QString title() const;
	virtual Graph2dHybridWindowResultCopyDataItem* copy(Graph2dHybridWindowResultCopyGroupDataItem* parent);

	const Graph2dHybridWindowResultSetting::Setting& setting() const;
	void setSetting(const Graph2dHybridWindowResultSetting::Setting& s);

	const std::vector<double>& xValues() const;
	const std::vector<double>& yValues() const;
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;

protected:
	virtual void updateValues() = 0;
	void setVisible(bool visible) override;
	QwtPlotCustomCurve* m_curve;
	QwtArrayData* m_data;

	Graph2dHybridWindowResultSetting::Setting m_setting;
	std::vector<double> m_xValues;
	std::vector<double> m_yValues;

private:
	bool m_attached;
};

#endif // GRAPH2DHYBRIDWINDOWRESULTDATAITEM_H
