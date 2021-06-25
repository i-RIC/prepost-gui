#ifndef GRAPH2DHYBRIDWINDOWIMPORTDATADATAITEM_H
#define GRAPH2DHYBRIDWINDOWIMPORTDATADATAITEM_H

#include "../graph2dhybridwindowdataitem.h"
#include "../graph2dhybridwindowresultsetting.h"

#include <vector>

class QwtPlotCustomCurve;

class Graph2dHybridWindowImportDataDataItem : public Graph2dHybridWindowDataItem
{
	Q_OBJECT

public:
	Graph2dHybridWindowImportDataDataItem(Graph2dWindowDataItem* parent);
	~Graph2dHybridWindowImportDataDataItem();

	void setTitle(const QString& title);
	void setIndex(int index);
	void setData(const std::vector<double>& xvals, const std::vector<double>& yvals);

	const Graph2dHybridWindowResultSetting::Setting& setting() const;
	void setSetting(const Graph2dHybridWindowResultSetting::Setting& setting);

	QwtPlotCustomCurve* curve() const;
	void update();
	void setId(const QString filename, const QString name);

	QString id() const;
	const std::vector<double>& xValues() const;
	const std::vector<double>& yValues() const;

	QString title() const;
	bool axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void setVisible(bool visible) override;
	QwtPlotCustomCurve* m_curve;

private:
	std::vector<double> m_xValues;
	std::vector<double> m_yValues;

	bool m_attached;
	int m_index;

	QString m_filename;
	QString m_name;
	Graph2dHybridWindowResultSetting::Setting m_setting;
};

#endif // GRAPH2DHYBRIDWINDOWIMPORTDATADATAITEM_H
