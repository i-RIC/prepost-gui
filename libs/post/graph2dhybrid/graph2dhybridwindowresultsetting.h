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
		xaPolyline
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
		Setting() {
			m_name = "";
			m_axisSide = asLeft;
			m_customColor = Qt::black;
			m_styleType = stLine;
			m_lineType = ltSolidLine;
			m_symbolType = symCircle;
			m_lineWidth = 1;
			m_barChart = false;
		}
		Setting(const QString& name) {
			m_name = name;
			m_axisSide = asLeft;
			m_customColor = Qt::black;
			m_styleType = stLine;
			m_lineType = ltSolidLine;
			m_symbolType = symCircle;
			m_lineWidth = 1;
			m_barChart = false;
		}
		void setupCurve(QwtPlotCustomCurve* curve) const;
		const QString& name() const {return m_name;}
		void setName(const QString& name) {m_name = name;}
		AxisSide axisSide() const {return m_axisSide;}
		void setAxisSide(AxisSide as) {m_axisSide = as;}
		int lineWidth() const {return m_lineWidth;}
		void setLineWidth(int width) {m_lineWidth = width;}
		const QColor& customColor() const {return m_customColor;}
		void setCustomColor(const QColor& c) {m_customColor = c;}
		StyleType styleType() const {return m_styleType;}
		void setStyleType(StyleType st) {m_styleType = st;}
		LineType lineType() const {return m_lineType;}
		void setLineType(LineType lt) {m_lineType = lt;}
		SymbolType symbolType() const {return m_symbolType;}
		void setSymbolType(SymbolType st) {m_symbolType = st;}
		void setBarChart(bool bar) {m_barChart = bar;}
		bool isBarChart() const {return m_barChart;}
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
		bool operator==(const DataTypeInfo& info) {
			if (dataType != info.dataType) {return false;}
			if (dimension != info.dimension) {return false;}
			if (zoneId != info.zoneId) {return false;}
			if (zoneName != info.zoneName) {return false;}
			return true;
		}
	};

	// constructor
	Graph2dHybridWindowResultSetting();
	~Graph2dHybridWindowResultSetting();
	bool init(PostSolutionInfo* sol, const QString& cgnsFilename);
	bool settingExists();

	QList<Graph2dWindowDataItem*> setupItems(Graph2dHybridWindowResultGroupDataItem* gItem) const ;

	QString autoYAxisLabel(AxisSide as) const;
	bool axisNeeded(AxisSide as);

	TimeValueType timeValueType() const {return m_timeValueType;}
	void setTimeValueType(TimeValueType t) {m_timeValueType = t;}
	PositionValueType positionValueType() const {return m_positionValueType;}
	void setPositionValueType(PositionValueType t) {m_positionValueType = t;}
	bool xAxisReverse() const {return m_xAxisReverse;}
	void setXAxisReverse(bool rev) {m_xAxisReverse = rev;}
	const QString& xAxisLabel() const {return m_xAxisLabel;}
	static QString autoXAxisLabel(Graph2dHybridWindowResultSetting::XAxisMode xm);
	void setAutoXAxisLabel();
	void setXAxisLabel(const QString& l) {m_xAxisLabel = l;}
	static QString autoXAxisTimeLabel(Graph2dHybridWindowResultSetting::XAxisMode mode, Graph2dHybridWindowResultSetting::TimeValueType t);
	static QString autoXAxisPositionLabel(Graph2dHybridWindowResultSetting::XAxisMode mode, Graph2dHybridWindowResultSetting::PositionValueType t);

	bool yAxisLeftAutoRange() const {return m_yAxisLeftAutoRange;}
	void setYAxisLeftAutoRange(bool a) {m_yAxisLeftAutoRange = a;}
	double yAxisLeftMin() const {return m_yAxisLeftMin;}
	void setYAxisLeftMin(double min) {m_yAxisLeftMin = min;}
	double yAxisLeftMax() const {return m_yAxisLeftMax;}
	void setYAxisLeftMax(double max) {m_yAxisLeftMax = max;}
	QString yAxisLeftTitle() const {return m_yAxisLeftTitle;}
	void setYAxisLeftTitle(const QString title) {m_yAxisLeftTitle = title;}
	bool yAxisLeftReverse() const {return m_yAxisLeftReverse;}
	void setYAxisLeftReverse(bool reverse) {m_yAxisLeftReverse = reverse;}
	bool yAxisLeftLog() const {return m_yAxisLeftLog;}
	void setYAxisLeftLog(bool log) {m_yAxisLeftLog = log;}

	bool yAxisRightAutoRange() const {return m_yAxisRightAutoRange;}
	void setYAxisRightAutoRange(bool a) {m_yAxisRightAutoRange = a;}
	double yAxisRightMin() const {return m_yAxisRightMin;}
	void setYAxisRightMin(double min) {m_yAxisRightMin = min;}
	double yAxisRightMax() const {return m_yAxisRightMax;}
	void setYAxisRightMax(double max) {m_yAxisRightMax = max;}
	QString yAxisRightTitle() const {return m_yAxisRightTitle;}
	void setYAxisRightTitle(const QString title) {m_yAxisRightTitle = title;}
	bool yAxisRightReverse() const {return m_yAxisRightReverse;}
	void setYAxisRightReverse(bool reverse) {m_yAxisRightReverse = reverse;}
	bool yAxisRightLog() const {return m_yAxisRightLog;}
	void setYAxisRightLog(bool log) {m_yAxisRightLog = log;}

	bool xAxisAutoRange() const {return m_xAxisAutoRange;}
	void setXAxisAutoRange(bool a) {m_xAxisAutoRange = a;}
	double xAxisValueMin() const {return m_xAxisValueMin;}
	void setXAxisValueMin(double min) {m_xAxisValueMin = min;}
	double xAxisValueMax() const {return m_xAxisValueMax;}
	void setXAxisValueMax(double max) {m_xAxisValueMax = max;}
	bool xAxisLog() const {return m_xAxisLog;}
	void setXAxisLog(bool log) {m_xAxisLog = log;}

	const QString& title() {return m_title;}
	void setTitle(const QString& t) {m_title = t;}
	bool addIndicesToTitle() const {return m_addIndicesToTitle;}
	void setAddIndicesToTitle(bool add) {m_addIndicesToTitle = add;}
	Graph2dHybridWindowResultSetting& operator=(const Graph2dHybridWindowResultSetting& s);
	bool dataAvailable();
	void setXAxisMode(XAxisMode m) {m_xAxisMode = m;}
	XAxisMode xAxisMode() const {return m_xAxisMode;}
	const QMap<XAxisMode, QMap<DimType, QList<DataTypeInfo*> > >& dataTypeInfoMap() {return m_dataTypeInfoMap;}
	DataTypeInfo* targetDataTypeInfo() const {return m_targetDataTypeInfo;}
	void setTargetDataTypeInfo(DataTypeInfo* type) {m_targetDataTypeInfo = type;}
	QList<Setting>& targetDatas() {return m_targetDatas;}
	const QList<Setting>& targetDatas() const {return m_targetDatas;}

	const QList<GeoDataPolyLine*>& polyLines() const {return m_polyLines;}

	const GeoDataPolyLine* targetPolyLine() const {return m_targetPolyLine;}
	void setTargetPolyLine(const GeoDataPolyLine* polyline) {m_targetPolyLine = polyline;}

	QColor autoColor(int index) const ;
	int gridI() const {return m_I;}
	void setGridI(int i) {m_I = i;}
	int gridJ() const {return m_J;}
	void setGridJ(int j) {m_J = j;}
	int gridK() const {return m_K;}
	void setGridK(int k) {m_K = k;}
	int gridIndex() const {return m_index;}
	void setGridIndex(int i) {m_index = i;}

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

private:
	static DimType dimTypeFromDataType(DataType dt);
	void setupMap();
	static QList<GeoDataPolyLine*> polyLines(const PostSolutionInfo* info);
	void setupPolyLines();

	QMap<XAxisMode, QMap<DimType, QList<DataTypeInfo*> > > m_dataTypeInfoMap;
	QList<DataTypeInfo> m_dataTypeInfos;

	XAxisMode m_xAxisMode;
	DataTypeInfo* m_targetDataTypeInfo;
	QList<Setting> m_targetDatas;
	QList<GeoDataPolyLine*> m_polyLines;
	const GeoDataPolyLine* m_targetPolyLine;
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
