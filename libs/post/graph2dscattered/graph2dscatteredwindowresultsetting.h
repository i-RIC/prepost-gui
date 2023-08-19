#ifndef GRAPH2DSCATTEREDWINDOWRESULTSETTING_H
#define GRAPH2DSCATTEREDWINDOWRESULTSETTING_H

#include <guicore/postcontainer/postsolutioninfo.h>
#include <misc/qfontcontainer.h>

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
	const static std::string XAXIS_POSITION_X;
	const static std::string XAXIS_POSITION_Y;
	const static std::string XAXIS_POSITION_Z;
	const static std::string XAXIS_STREAM_WISE_DISTANCE;

	class Setting
	{

	public:
		Setting();
		Setting(const std::string& name);

		void setupCurve(QwtPlotCustomCurve* curve) const;

		const std::string& name() const;
		void setName(const std::string& name);

		const QString& caption() const;
		void setCaption(const QString& caption);

		AxisSide axisSide() const;
		void setAxisSide(AxisSide as);

		const QColor& customColor() const;
		void setCustomColor(const QColor& c);

		SymbolType symbolType() const;
		void setSymbolType(SymbolType st);

		int symbolSize() const;
		void setSymbolSize(int size);

		void loadFromProjectMainFile(const QDomNode& node);
		void saveToProjectMainFile(QXmlStreamWriter& writer) const;

	private:
		std::string m_name;
		QString m_caption;
		AxisSide m_axisSide;
		QColor m_customColor;
		SymbolType m_symbolType;
		int m_symbolSize;
	};

	Graph2dScatteredWindowResultSetting();
	~Graph2dScatteredWindowResultSetting();

	bool init(PostSolutionInfo* sol);

	PostSolutionInfo::Dimension dimension() const;
	const std::string& zoneName() const;

	QList<Graph2dWindowDataItem*> setupItems(Graph2dScatteredWindowResultGroupDataItem* gItem) const;

	std::string xAxis() const;
	QString xAxisCaption() const;
	void setXAxis(const std::string& axis, const QString& caption);

	QString autoYAxisLabel(AxisSide as);
	bool axisNeeded(AxisSide as);

	QList<Setting>& targetDatas();
	const QList<Setting>& targetDatas() const;

	bool xAxisAutoRange() const;
	void setXAxisAutoRange(bool a);
	double xAxisValueMin() const;
	void setXAxisValueMin(double min);
	double xAxisValueMax() const;
	void setXAxisValueMax(double max);
	bool xAxisReverse() const;
	void setXAxisReverse(bool rev);
	bool xAxisLog() const;
	void setXAxisLog(bool log);
	const QString& xAxisLabel() const;
	void setXAxisLabel(const QString& l);
	void setAutoXAxisLabel();

	bool yAxisLeftAutoRange() const;
	void setYAxisLeftAutoRange(bool a);
	double yAxisLeftMin() const;
	void setYAxisLeftMin(double min);
	double yAxisLeftMax() const;
	void setYAxisLeftMax(double max);
	bool yAxisLeftReverse() const;
	void setYAxisLeftReverse(bool reverse);
	bool yAxisLeftLog() const;
	void setYAxisLeftLog(bool log);
	QString yAxisLeftTitle() const;
	void setYAxisLeftTitle(const QString title);

	bool yAxisRightAutoRange() const;
	void setYAxisRightAutoRange(bool a);
	double yAxisRightMin() const;
	void setYAxisRightMin(double min);
	double yAxisRightMax() const;
	void setYAxisRightMax(double max);
	bool yAxisRightReverse() const;
	void setYAxisRightReverse(bool reverse);
	bool yAxisRightLog() const;
	void setYAxisRightLog(bool log);
	QString yAxisRightTitle() const;
	void setYAxisRightTitle(const QString title);

	const QString& title() const;
	void setTitle(const QString& t);
	bool addTimeToTitle() const;
	void setAddTimeToTitle(bool add);

	QFont chartTitleFont() const;
	void setChartTitleFont(const QFont& font);
	QFont legendFont() const;
	void setLegendFont(const QFont& font);
	QFont xAxisTitleFont() const;
	void setXAxisTitleFont(const QFont& font);
	QFont xAxisTickFont() const;
	void setXAxisTickFont(const QFont& font);
	QFont yAxisTitleFont() const;
	void setYAxisTitleFont(const QFont& font);
	QFont yAxisTickFont() const;
	void setYAxisTickFont(const QFont& font);

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

	static QwtSymbol::Style getSymbolStyle(SymbolType st);
	static QColor autoColor(int index);

private:
	PostSolutionInfo::Dimension m_dimension;
	std::string m_zoneName;

	QList<Setting> m_targetDatas;

	std::string m_xAxis;
	QString m_xAxisCaption;
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

	QFontContainer m_chartTitleFont;
	QFontContainer m_legendFont;
	QFontContainer m_xAxisTitleFont;
	QFontContainer m_xAxisTickFont;
	QFontContainer m_yAxisTitleFont;
	QFontContainer m_yAxisTickFont;

	static ColorSource* m_colorSource;
};

#endif // GRAPH2DSCATTEREDWINDOWRESULTSETTING_H
