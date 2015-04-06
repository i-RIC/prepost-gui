#ifndef GRAPH2DHYBRIDWINDOWIMPORTDATADATAITEM_H
#define GRAPH2DHYBRIDWINDOWIMPORTDATADATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"

#include <QVector>

class QwtPlotCustomCurve;

class Graph2dHybridWindowImportDataDataItem : public Graph2dHybridWindowDataItem
{
	Q_OBJECT
public:
	Graph2dHybridWindowImportDataDataItem(Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowImportDataDataItem();
	void setTitle(const QString& title);
	void setIndex(int index);
	void setData(const QVector<double>& xvals, const QVector<double>& yvals);
	void setSetting(const Graph2dHybridWindowResultSetting::Setting& setting);
	const Graph2dHybridWindowResultSetting::Setting& setting() const {return m_setting;}
	QwtPlotCustomCurve* curve(){return m_curve;}
	void update();
	void setId(const QString filename, const QString name);
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
private:
	QVector<double> m_xValues;
	QVector<double> m_yValues;
	bool m_attached;
	int m_index;
	QString m_filename;
	QString m_name;
	Graph2dHybridWindowResultSetting::Setting m_setting;
};

#endif // GRAPH2DHYBRIDWINDOWIMPORTDATADATAITEM_H
