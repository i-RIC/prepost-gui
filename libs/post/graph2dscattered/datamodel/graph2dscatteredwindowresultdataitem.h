#ifndef GRAPH2DSCATTEREDWINDOWRESULTDATAITEM_H
#define GRAPH2DSCATTEREDWINDOWRESULTDATAITEM_H

#include "../graph2dscatteredwindowdataitem.h"
#include "../graph2dscatteredwindowresultsetting.h"

#include <vector>

class QwtPlotCustomCurve;
class QwtArrayData;

class Graph2dScatteredWindowResultDataItem : public Graph2dScatteredWindowDataItem
{
	Q_OBJECT

public:
	Graph2dScatteredWindowResultDataItem(const QString& title, int index, const Graph2dScatteredWindowResultSetting::Setting& setting, Graph2dWindowDataItem* parent);
	~Graph2dScatteredWindowResultDataItem() override;

	QwtPlotCustomCurve* curve() const;
	void update();
	QString title() const;

	const Graph2dScatteredWindowResultSetting::Setting& setting() const;
	void setSetting(const Graph2dScatteredWindowResultSetting::Setting& s);

	const std::vector<double>& xValues() const;
	const std::vector<double>& yValues() const;
	bool axisNeeded(Graph2dScatteredWindowResultSetting::AxisSide as) const;

protected:
	virtual void updateValues(int fn) = 0;
	void setVisible(bool visible) override;
	QwtPlotCustomCurve* m_curve;

	Graph2dScatteredWindowResultSetting::Setting m_setting;
	std::vector<double> m_xValues;
	std::vector<double> m_yValues;

private:
	bool m_attached;
};

#endif // GRAPH2DSCATTEREDWINDOWRESULTDATAITEM_H
