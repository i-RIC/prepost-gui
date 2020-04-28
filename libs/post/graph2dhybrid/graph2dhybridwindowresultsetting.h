#ifndef GRAPH2DHYBRIDWINDOWRESULTSETTING_H
#define GRAPH2DHYBRIDWINDOWRESULTSETTING_H

#include <guicore/postcontainer/postsolutioninfo.h>

#include <qwt_symbol.h>

#include <QString>
#include <QList>
#include <QMap>
#include <QColor>

#include <cgnslib.h>

class GeoDataPolyLine;
class GeoDataPolyLineGroup;
class GeoDataPolyLineGroupPolyLine;
class Graph2dHybridWindowResultGroupDataItem;
class Graph2dHybridWindowResultDataItem;
class Graph2dWindowDataItem;
class PostZoneDataContainer;
class ColorSource;
class QwtPlotCustomCurve;
class SolverDefinitionGridType;
class QDomNode;
class QXmlStreamWriter;

class Graph2dHybridWindowResultSetting
{

public:
	enum TimeValueType {
		tvtTime,
		tvtTimeStep
	};

	enum PositionValueType {
		pvtDistance,
		pvtIndex
	};

	enum LabelMode {
		lmAuto,
		lmCustom
	};

	enum AxisSide {
		asLeft,
		asRight
	};

	enum XAxisMode {
		xaTime,
		xaI,
		xaJ,
		xaK,
		xaPolyLine,
		xaPolyLineGroup
	};

	enum DimType {
		dimBase,
		dim1D,
		dim2D,
		dim3D
	};

	enum DataType {
		dtBaseIterative,
		dtDim1DStructured,
		dtDim2DStructured,
		dtDim3DStructured,
		dtDim1DUnstructured,
		dtDim2DUnstructured,
		dtDim3DUnstructured
	};

	enum LineType {
		ltSolidLine,
		ltDashLine,
		ltDotLine,
		ltDashDotLine
	};

	enum StyleType {
		stLine,
		stSymbol
	};

	enum SymbolType {
		symCircle,
		symRect,
		symDiamond,
		symTriangle,
		symCross,
		symXCross
	};
	static Qt::PenStyle getPenStyle(LineType lt);
	static QwtSymbol::Style getSymbolStyle(SymbolType st);

	static GridLocation_t getGridLocation(QString string);
	static QString getGridLocationString(GridLocation_t location);

	static GridLocation_t getGridLocationTranslated(QString string);
	static QString getGridLocationStringTranslated(GridLocation_t location);

	struct DataTypeInfo;
	class Setting
	{

	public:
		Setting();
		Setting(const QString& name);

		void setupCurve(QwtPlotCustomCurve* curve) const;
		const QString& name() const;
		void setName(const QString& name);
		AxisSide axisSide() const;
		void setAxisSide(AxisSide as);
		int lineWidth() const;
		void setLineWidth(int width);
		const QColor& customColor() const;
		void setCustomColor(const QColor& c);
		StyleType styleType() const;
		void setStyleType(StyleType st);
		LineType lineType() const;
		void setLineType(LineType lt);
		SymbolType symbolType() const;
		void setSymbolType(SymbolType st);
		void setBarChart(bool bar);
		bool isBarChart() const;
		void loadFromProjectMainFile(const QDomNode& node);
		void saveToProjectMainFile(QXmlStreamWriter& writer) const;

	private:
		QString m_name;
		AxisSide m_axisSide;
		int m_lineWidth;
		QColor m_customColor;
		StyleType m_styleType;
		LineType m_lineType;
		SymbolType m_symbolType;
		bool m_barChart;
	};

	struct DataTypeInfo {
		DataType dataType;
		PostSolutionInfo::Dimension dimension;
		int zoneId;
		std::string zoneName;
		QMap<GridLocation_t, QStringList> dataNamesMap;
		SolverDefinitionGridType* gridType;
		GridLocation_t gridLocation;
		void loadFromProjectMainFile(const QDomNode& node);
		void saveToProjectMainFile(QXmlStreamWriter& writer) const;
		bool operator==(const DataTypeInfo& info);
	};

	// constructor
	Graph2dHybridWindowResultSetting();
	~Graph2dHybridWindowResultSetting();
	bool init(PostSolutionInfo* sol, const QString& cgnsFilename);
	bool settingExists();

	QList<Graph2dWindowDataItem*> setupItems(Graph2dHybridWindowResultGroupDataItem* gItem) const ;

	QString autoYAxisLabel(AxisSide as) const;
	bool axisNeeded(AxisSide as);

	TimeValueType timeValueType() const;
	void setTimeValueType(TimeValueType t);
	PositionValueType positionValueType() const;
	void setPositionValueType(PositionValueType t);
	bool xAxisReverse() const;
	void setXAxisReverse(bool rev);

	const QString& xAxisLabel() const;
	static QString autoXAxisLabel(Graph2dHybridWindowResultSetting::XAxisMode xm);
	void setAutoXAxisLabel();
	void setXAxisLabel(const QString& l);
	static QString autoXAxisTimeLabel(Graph2dHybridWindowResultSetting::XAxisMode mode, Graph2dHybridWindowResultSetting::TimeValueType t);
	static QString autoXAxisPositionLabel(Graph2dHybridWindowResultSetting::XAxisMode mode, Graph2dHybridWindowResultSetting::PositionValueType t);

	bool yAxisLeftAutoRange() const;
	void setYAxisLeftAutoRange(bool a);
	double yAxisLeftMin() const;
	void setYAxisLeftMin(double min);
	double yAxisLeftMax() const;
	void setYAxisLeftMax(double max);
	QString yAxisLeftTitle() const;
	void setYAxisLeftTitle(const QString title);
	bool yAxisLeftReverse() const;
	void setYAxisLeftReverse(bool reverse);
	bool yAxisLeftLog() const;
	void setYAxisLeftLog(bool log);

	bool yAxisRightAutoRange() const;
	void setYAxisRightAutoRange(bool a);
	double yAxisRightMin() const;
	void setYAxisRightMin(double min);
	double yAxisRightMax() const;
	void setYAxisRightMax(double max);
	QString yAxisRightTitle() const;
	void setYAxisRightTitle(const QString title);
	bool yAxisRightReverse() const;
	void setYAxisRightReverse(bool reverse);
	bool yAxisRightLog() const;
	void setYAxisRightLog(bool log);

	bool xAxisAutoRange() const;
	void setXAxisAutoRange(bool a);
	double xAxisValueMin() const;
	void setXAxisValueMin(double min);
	double xAxisValueMax() const;
	void setXAxisValueMax(double max);
	bool xAxisLog() const;
	void setXAxisLog(bool log);

	const QString& title() const;
	void setTitle(const QString& t);
	bool addIndicesToTitle() const;
	void setAddIndicesToTitle(bool add);
	Graph2dHybridWindowResultSetting& operator=(const Graph2dHybridWindowResultSetting& s);
	bool dataAvailable();
	void setXAxisMode(XAxisMode m);
	XAxisMode xAxisMode() const;
	const QMap<XAxisMode, QMap<DimType, QList<DataTypeInfo*> > >& dataTypeInfoMap();
	DataTypeInfo* targetDataTypeInfo() const;
	void setTargetDataTypeInfo(DataTypeInfo* type);
	QList<Setting>& targetDatas();
	const QList<Setting>& targetDatas() const;

	const QList<GeoDataPolyLine*>& polyLines() const;
	const QList<GeoDataPolyLineGroup*>& polyLineGroups() const;

	const GeoDataPolyLine* targetPolyLine() const;
	void setTargetPolyLine(const GeoDataPolyLine* polyline);

	const GeoDataPolyLineGroupPolyLine* targetPolyLineGroupPolyLine() const;
	void setTargetPolyLineGroupPolyLine(const GeoDataPolyLineGroupPolyLine* line);

	QColor autoColor(int index) const ;
	int gridI() const;
	void setGridI(int i);
	int gridJ() const;
	void setGridJ(int j);
	int gridK() const;
	void setGridK(int k);
	int gridIndex() const;
	void setGridIndex(int i);

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

	void setupPolyLineGroups();
	void setupPolyLines();

private:
	static DimType dimTypeFromDataType(DataType dt);
	void setupMap();
	static QList<GeoDataPolyLine*> polyLines(const PostSolutionInfo* info);
	static QList<GeoDataPolyLineGroup*> polyLineGroups(const PostSolutionInfo* info);

	QMap<XAxisMode, QMap<DimType, QList<DataTypeInfo*> > > m_dataTypeInfoMap;
	QList<DataTypeInfo> m_dataTypeInfos;

	XAxisMode m_xAxisMode;
	DataTypeInfo* m_targetDataTypeInfo;
	QList<Setting> m_targetDatas;
	QList<GeoDataPolyLine*> m_polyLines;
	QList<GeoDataPolyLineGroup*> m_polyLineGroups;
	const GeoDataPolyLine* m_targetPolyLine;
	const GeoDataPolyLineGroupPolyLine* m_targetPolyLineGroupPolyLine;
	const PostSolutionInfo* m_postSolutionInfo;

	QString m_title;
	bool m_addIndicesToTitle;

	ColorSource* m_colorSource;

	TimeValueType m_timeValueType;
	PositionValueType m_positionValueType;
	bool m_xAxisReverse;
	bool m_xAxisLog;
	QString m_xAxisLabel;

	bool m_yAxisLeftAutoRange;
	double m_yAxisLeftMin;
	double m_yAxisLeftMax;
	QString m_yAxisLeftTitle;
	bool m_yAxisLeftReverse;
	bool m_yAxisLeftLog;

	bool m_yAxisRightAutoRange;
	double m_yAxisRightMin;
	double m_yAxisRightMax;
	QString m_yAxisRightTitle;
	bool m_yAxisRightReverse;
	bool m_yAxisRightLog;

	bool m_xAxisAutoRange;
	double m_xAxisValueMin;
	double m_xAxisValueMax;

	int m_I;
	int m_J;
	int m_K;
	int m_index;
};

#endif // GRAPH2DHYBRIDWINDOWRESULTSETTING_H
