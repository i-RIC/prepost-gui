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
	void setData(const QVector<double>& xvals, const QVector<double>& yvals);
	void setSetting(const Graph2dHybridWindowResultSetting::Setting& setting);
	const Graph2dHybridWindowResultSetting::Setting& setting() const {return m_setting;}
	QwtPlotCustomCurve* curve() const {return m_curve;}
	void update();
	void setId(IDType idtype, const QStringList args);
	const QString id();
	const QVector<double>& xValues() const {return m_xValues;}
	const QVector<double>& yValues() const {return m_yValues;}
	const QString title() const;
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void setVisible(bool visible);
	QwtPlotCustomCurve* m_curve;
	QwtSeriesData<double>* m_data;
private:
	QVector<double> m_xValues;
	QVector<double> m_yValues;
	bool m_attached;
	int m_index;
	IDType m_idType;
	QStringList m_idArgs;
	Graph2dHybridWindowResultSetting::Setting m_setting;
};

#endif // GRAPH2DHYBRIDWINDOWRESULTCOPYDATAITEM_H
