#if SKIP
#ifndef GRAPH2DVERIFICATIONWINDOWIMPORTDATADATAITEM_H
#define GRAPH2DVERIFICATIONWINDOWIMPORTDATADATAITEM_H

#include "../graph2dverificationwindowdataitem.h"
#include "../graph2dverificationwindowresultsetting.h"

#include <QVector>

class QwtPlotCustomCurve;

class Graph2dVerificationWindowImportDataDataItem : public Graph2dVerificationWindowDataItem
{
	Q_OBJECT

public:
	Graph2dVerificationWindowImportDataDataItem(Graph2dWindowDataItem* parent);
	~Graph2dVerificationWindowImportDataDataItem();
	void setTitle(const QString& title);
	void setIndex(int index);
	void setData(const QVector<double>& xvals, const QVector<double>& yvals);
	void setSetting(const Graph2dVerificationWindowResultSetting::Setting& setting);
	const Graph2dVerificationWindowResultSetting::Setting& setting() const {return m_setting;}
	QwtPlotCustomCurve* curve() const {return m_curve;}
	void update();
	void setId(const QString filename, const QString name);
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

private:
	QVector<double> m_xValues;
	QVector<double> m_yValues;
	bool m_attached;
	int m_index;
	QString m_filename;
	QString m_name;
	Graph2dVerificationWindowResultSetting::Setting m_setting;
};

#endif // GRAPH2DVERIFICATIONWINDOWIMPORTDATADATAITEM_H
#endif
