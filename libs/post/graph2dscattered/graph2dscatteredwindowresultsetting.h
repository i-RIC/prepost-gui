#ifndef GRAPH2DSCATTEREDWINDOWRESULTSETTING_H
#define GRAPH2DSCATTEREDWINDOWRESULTSETTING_H

#include <guicore/postcontainer/postsolutioninfo.h>
#include <QColor>

#include <qwt_symbol.h>

class QString;
class ColorSource;
class QwtPlotCustomCurve;
class Graph2dWindowDataItem;
class Graph2dScatteredWindowResultGroupDataItem;

class Graph2dScatteredWindowResultSetting
{
public:
	enum AxisSide {
		asLeft,
		asRight
	};
	enum DimType {
		dim1D,
		dim2D,
		dim3D
	};
	enum SymbolType {
		symCircle,
		symRect,
		symDiamond,
		symTriangle,
		symCross,
		symXCross
	};
	const static QString XAXIS_POSITION_X;
	const static QString XAXIS_POSITION_Y;
	const static QString XAXIS_POSITION_Z;
	const static QString XAXIS_STREAM_WISE_DISTANCE;

	class Setting
	{
	public:
		Setting() {
			m_name = "";
			m_axisSide = asLeft;
			m_customColor = Qt::black;
			m_symbolType = symCircle;
			m_symbolSize = 7;
		}
		Setting(const QString& name) {
			m_name = name;
			m_axisSide = asLeft;
			m_customColor = Qt::black;
			m_symbolType = symCircle;
			m_symbolSize = 7;
		}
		void setupCurve(QwtPlotCustomCurve* curve) const;
		const QString& name() const {return m_name;}
		void setName(const QString& name) {m_name = name;}
		AxisSide axisSide() const {return m_axisSide;}
		void setAxisSide(AxisSide as) {m_axisSide = as;}
		const QColor& customColor() const {return m_customColor;}
		void setCustomColor(const QColor& c) {m_customColor = c;}
		SymbolType symbolType() const {return m_symbolType;}
		void setSymbolType(SymbolType st) {m_symbolType = st;}
		int symbolSize() const {return m_symbolSize;}
		void setSymbolSize(int size) {m_symbolSize = size;}
		void loadFromProjectMainFile(const QDomNode& node);
		void saveToProjectMainFile(QXmlStreamWriter& writer) const;
	private:
		QString m_name;
		AxisSide m_axisSide;
		QColor m_customColor;
		SymbolType m_symbolType;
		int m_symbolSize;
	};

	Graph2dScatteredWindowResultSetting();
	~Graph2dScatteredWindowResultSetting();

	bool init(PostSolutionInfo* sol);

	PostSolutionInfo::Dimension dimension() const {return m_dimension;}
	int zoneId() const {return m_zoneId;}
	QString zoneName() const {return m_zoneName;}

	QList<Graph2dWindowDataItem*> setupItems(Graph2dScatteredWindowResultGroupDataItem* gItem) const;

	QString xAxis() const {return m_xAxis;}
	void setXAxis(const QString& axis) {m_xAxis = axis;}

	const QString autoYAxisLabel(AxisSide as);
	bool axisNeeded(AxisSide as);

	QList<Setting>& targetDatas() {return m_targetDatas;}
	const QList<Setting>& targetDatas() const {return m_targetDatas;}

	bool xAxisAutoRange() const {return m_xAxisAutoRange;}
	void setXAxisAutoRange(bool a) {m_xAxisAutoRange = a;}
	double xAxisValueMin() const {return m_xAxisValueMin;}
	void setXAxisValueMin(double min) {m_xAxisValueMin = min;}
	double xAxisValueMax() const {return m_xAxisValueMax;}
	void setXAxisValueMax(double max) {m_xAxisValueMax = max;}
	bool xAxisReverse() const {return m_xAxisReverse;}
	void setXAxisReverse(bool rev) {m_xAxisReverse = rev;}
	bool xAxisLog() const {return m_xAxisLog;}
	void setXAxisLog(bool log) {m_xAxisLog = log;}
	const QString& xAxisLabel() const {return m_xAxisLabel;}
	void setXAxisLabel(const QString& l) {m_xAxisLabel = l;}
	void setAutoXAxisLabel();

	bool yAxisLeftAutoRange() const {return m_yAxisLeftAutoRange;}
	void setYAxisLeftAutoRange(bool a) {m_yAxisLeftAutoRange = a;}
	double yAxisLeftMin() const {return m_yAxisLeftMin;}
	void setYAxisLeftMin(double min) {m_yAxisLeftMin = min;}
	double yAxisLeftMax() const {return m_yAxisLeftMax;}
	void setYAxisLeftMax(double max) {m_yAxisLeftMax = max;}
	bool yAxisLeftReverse() const {return m_yAxisLeftReverse;}
	void setYAxisLeftReverse(bool reverse) {m_yAxisLeftReverse = reverse;}
	bool yAxisLeftLog() const {return m_yAxisLeftLog;}
	void setYAxisLeftLog(bool log) {m_yAxisLeftLog = log;}
	QString yAxisLeftTitle() const {return m_yAxisLeftTitle;}
	void setYAxisLeftTitle(const QString title) {m_yAxisLeftTitle = title;}

	bool yAxisRightAutoRange() const {return m_yAxisRightAutoRange;}
	void setYAxisRightAutoRange(bool a) {m_yAxisRightAutoRange = a;}
	double yAxisRightMin() const {return m_yAxisRightMin;}
	void setYAxisRightMin(double min) {m_yAxisRightMin = min;}
	double yAxisRightMax() const {return m_yAxisRightMax;}
	void setYAxisRightMax(double max) {m_yAxisRightMax = max;}
	bool yAxisRightReverse() const {return m_yAxisRightReverse;}
	void setYAxisRightReverse(bool reverse) {m_yAxisRightReverse = reverse;}
	bool yAxisRightLog() const {return m_yAxisRightLog;}
	void setYAxisRightLog(bool log) {m_yAxisRightLog = log;}
	QString yAxisRightTitle() const {return m_yAxisRightTitle;}
	void setYAxisRightTitle(const QString title) {m_yAxisRightTitle = title;}

	const QString& title() const {return m_title;}
	void setTitle(const QString& t) {m_title = t;}
	bool addTimeToTitle() const {return m_addTimeToTitle;}
	void setAddTimeToTitle(bool add) {m_addTimeToTitle = add;}

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

	static QwtSymbol::Style getSymbolStyle(SymbolType st);
	static QColor autoColor(int index);

private:
	PostSolutionInfo::Dimension m_dimension;
	int m_zoneId;
	QString m_zoneName;

	QList<Setting> m_targetDatas;

	QString m_xAxis;
	bool m_xAxisAutoRange;
	double m_xAxisValueMin;
	double m_xAxisValueMax;
	bool m_xAxisReverse;
	bool m_xAxisLog;
	QString m_xAxisLabel;

	bool m_yAxisLeftAutoRange;
	double m_yAxisLeftMin;
	double m_yAxisLeftMax;
	bool m_yAxisLeftReverse;
	bool m_yAxisLeftLog;
	QString m_yAxisLeftTitle;

	bool m_yAxisRightAutoRange;
	double m_yAxisRightMin;
	double m_yAxisRightMax;
	bool m_yAxisRightReverse;
	bool m_yAxisRightLog;
	QString m_yAxisRightTitle;

	QString m_title;
	bool m_addTimeToTitle;

	static ColorSource* m_colorSource;
};

#endif // GRAPH2DSCATTEREDWINDOWRESULTSETTING_H
