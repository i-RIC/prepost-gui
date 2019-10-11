#if SKIP
#ifndef GRAPH2DVERIFICATIONWINDOWRESULTCOPYDATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWRESULTCOPYDATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include "../graph2dverificationwindowresultsetting.h"

#include <QVector>

class QwtPlotCustomCurve;
template <typename T> class QwtSeriesData;

class Graph2dVerificationWindowResultCopyDataItem : public Graph2dVerificationWindowDataItem
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

	Graph2dVerificationWindowResultCopyDataItem(Graph2dWindowDataItem* parent);
	~Graph2dVerificationWindowResultCopyDataItem();
	void setTitle(const QString& title);
	void setIndex(int index);
	void setData(const QVector<double>& xvals, const QVector<double>& yvals);
	void setSetting(const Graph2dVerificationWindowResultSetting::Setting& setting);
	const Graph2dVerificationWindowResultSetting::Setting& setting() const {return m_setting;}
	QwtPlotCustomCurve* curve() const {return m_curve;}
	void update();
	void setId(IDType idtype, const QStringList args);
	QString id() const;
	const QVector<double>& xValues() const {return m_xValues;}
	const QVector<double>& yValues() const {return m_yValues;}
	QString title() const;
	bool axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void setVisible(bool visible) override;
	QwtPlotCustomCurve* m_curve;
	QwtSeriesData<double>* m_data;

private:
	QVector<double> m_xValues;
	QVector<double> m_yValues;
	bool m_attached;
	int m_index;
	IDType m_idType;
	QStringList m_idArgs;
	Graph2dVerificationWindowResultSetting::Setting m_setting;
};

#endif // GRAPH2DVERIFICATIONWINDOWRESULTCOPYDATAITEM_H
#endif
