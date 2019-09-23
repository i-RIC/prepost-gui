#ifndef GRAPH2DVERIFICATIONWINDOWRESULTDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWRESULTDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include "../graph2dverificationwindowresultsetting.h"

#include <QVector>

class QwtPlotCustomCurve;
class QwtArrayData;
class Graph2dVerificationWindowResultCopyDataItem;
class Graph2dVerificationWindowResultCopyGroupDataItem;

class Graph2dVerificationWindowResultDataItem : public Graph2dVerificationWindowDataItem
{

public:
	Graph2dVerificationWindowResultDataItem(const QString& title, int index, const Graph2dVerificationWindowResultSetting::Setting& setting, Graph2dWindowDataItem* parent);
	virtual ~Graph2dVerificationWindowResultDataItem();
	QwtPlotCustomCurve* curve() const {return m_curve;}
	void update(int fn);
	QString title() const;
	virtual Graph2dVerificationWindowResultCopyDataItem* copy(Graph2dVerificationWindowResultCopyGroupDataItem* parent);
	const Graph2dVerificationWindowResultSetting::Setting& setting() const {return m_setting;}
	void setSetting(const Graph2dVerificationWindowResultSetting::Setting& s);
	const QVector<double>& xValues() const {return m_xValues;}
	const QVector<double>& yValues() const {return m_yValues;}
	bool axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const;

protected:
	virtual void updateValues(int fn) = 0;
	void setVisible(bool visible) override;
	QwtPlotCustomCurve* m_curve;
	QwtArrayData* m_data;

	Graph2dVerificationWindowResultSetting::Setting m_setting;
	QVector<double> m_xValues;
	QVector<double> m_yValues;

private:
	bool m_attached;
};

#endif // GRAPH2DVERIFICATIONWINDOWRESULTDATAITEM_H
