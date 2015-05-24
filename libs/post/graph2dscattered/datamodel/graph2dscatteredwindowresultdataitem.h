#ifndef GRAPH2DSCATTEREDWINDOWRESULTDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWRESULTDATAITEM_H

#include "../graph2dscatteredwindowdataitem.h"
#include "../graph2dscatteredwindowresultsetting.h"

#include <QVector>

class QwtPlotCustomCurve;
class QwtArrayData;

class Graph2dScatteredWindowResultDataItem : public Graph2dScatteredWindowDataItem
{
	Q_OBJECT

public:
	Graph2dScatteredWindowResultDataItem(const QString& title, int index, const Graph2dScatteredWindowResultSetting::Setting& setting, Graph2dWindowDataItem* parent);
	virtual ~Graph2dScatteredWindowResultDataItem();
	QwtPlotCustomCurve* curve() const {return m_curve;}
	void update(int fn);
	const QString title() const;
	const Graph2dScatteredWindowResultSetting::Setting& setting() const {return m_setting;}
	void setSetting(const Graph2dScatteredWindowResultSetting::Setting& s);
	const QVector<double>& xValues() const {return m_xValues;}
	const QVector<double>& yValues() const {return m_yValues;}
	bool axisNeeded(Graph2dScatteredWindowResultSetting::AxisSide as) const;

protected:
	virtual void updateValues(int fn) = 0;
	void setVisible(bool visible);
	QwtPlotCustomCurve* m_curve;

	Graph2dScatteredWindowResultSetting::Setting m_setting;
	QVector<double> m_xValues;
	QVector<double> m_yValues;

private:
	bool m_attached;
};

#endif // GRAPH2DSCATTEREDWINDOWRESULTDATAITEM_H
