#ifndef GRAPH2DHYBRIDWINDOWRESULTCOPYDATAITEM_H
#define GRAPH2DHYBRIDWINDOWRESULTCOPYDATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"

#include <QVector>

class QwtPlotCustomCurve;
template <typename T> class QwtSeriesData;

class Graph2dHybridWindowResultCopyDataItem : public Graph2dHybridWindowDataItem
{
	Q_OBJECT

public:
	enum IDType {
		idtTimeBaseIterative,
		idtTime1D,
		idtTime2D,
		idtTime3D,
		idtTimeUnst,
		idt1D,
		idtI2D,
		idtI3D,
		idtJ2D,
		idtJ3D,
		idtK3D
	};

	Graph2dHybridWindowResultCopyDataItem(Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowResultCopyDataItem();

	void setTitle(const QString& title);
	void setIndex(int index);
	void setData(const std::vector<double>& xvals, const std::vector<double>& yvals);

	void setSetting(const Graph2dHybridWindowResultSetting::Setting& setting);
	const Graph2dHybridWindowResultSetting::Setting& setting() const;

	QwtPlotCustomCurve* curve() const;
	void update();
	void setId(IDType idtype, const QStringList args);
	QString id() const;
	const std::vector<double>& xValues() const;
	const std::vector<double>& yValues() const;
	QString title() const;
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;

protected:
	void setVisible(bool visible) override;
	QwtPlotCustomCurve* m_curve;
	QwtSeriesData<double>* m_data;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	std::vector<double> m_xValues;
	std::vector<double> m_yValues;
	bool m_attached;
	int m_index;
	IDType m_idType;
	QStringList m_idArgs;
	Graph2dHybridWindowResultSetting::Setting m_setting;
};

#endif // GRAPH2DHYBRIDWINDOWRESULTCOPYDATAITEM_H
