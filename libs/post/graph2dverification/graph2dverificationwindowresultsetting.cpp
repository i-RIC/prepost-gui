#define _USE_MATH_DEFINES  1  // set to 1 to match qwt_math.h
#include <cmath>

#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowresultsetting.h"

#include <geodata/polyline/geodatapolyline.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/measured/measureddata.h>
#include <guibase/qwtplotcustomcurve.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/misc/cgnsfileopener.h>
#include <guicore/project/colorsource.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QList>
#include <QXmlStreamWriter>

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

#include <cgnslib.h>
#include <qwt_plot.h>

#include <stdexcept>

Graph2dVerificationWindowResultSetting::Graph2dVerificationWindowResultSetting()
{
	m_xAxisMode = xaTime;
#if SKIP
	m_timeValueType = tvtTime;
	m_positionValueType = pvtDistance;
	m_xAxisReverse = false;
	m_xAxisLog = false;

	m_targetDataTypeInfo = nullptr;
	m_targetPolyLine = nullptr;
	m_postSolutionInfo = nullptr;

	m_colorSource = new ColorSource(nullptr);

	m_addIndicesToTitle = false;

	m_yAxisLeftAutoRange = true;
	m_yAxisLeftTitle = "";
	m_yAxisLeftReverse = false;
	m_yAxisLeftLog = false;
	m_yAxisRightAutoRange = true;
	m_yAxisRightTitle = "";
	m_yAxisRightReverse = false;
	m_yAxisRightLog = false;

	m_xAxisAutoRange = true;
	m_xAxisValueMin = 0;
	m_xAxisValueMax = 0;

	m_I = 0;
	m_J = 0;
	m_K = 0;
	m_index = 0;
#endif

	m_timeStep = 0;
	m_graphType = Graph2dVerificationWindowResultSetting::gtSWDvsValues;
	m_activePostData = nullptr;
	m_activeMeasuredData = nullptr;
}

Graph2dVerificationWindowResultSetting::~Graph2dVerificationWindowResultSetting()
{
#if SKIP
	delete m_colorSource;
#endif
}

bool Graph2dVerificationWindowResultSetting::init(PostSolutionInfo* sol, const std::vector<MeasuredData*>& measuredData, const QString& cgnsFilename)
{
	m_measuredData = measuredData;
	return init(sol, cgnsFilename);
}

bool Graph2dVerificationWindowResultSetting::init(PostSolutionInfo* sol, const QString& cgnsFilename)
{
	int fn, ier;
	CgnsFileOpener* opener = nullptr;

	if (sol->fileId() == 0) {
		try {
			opener = new CgnsFileOpener(iRIC::toStr(cgnsFilename), CG_MODE_READ);
			fn = opener->fileId();
		} catch (std::runtime_error&) {
			return false;
		}
	} else {
		fn = sol->fileId();
	}

	int nbases;
	ier = cg_nbases(fn, &nbases);
	if (ier != 0) {
		delete opener;
		return false;
	}

	m_postSolutionInfo = sol;
	m_timeStep = m_postSolutionInfo->currentStep();

	for (int baseid = 1; baseid <= nbases; ++baseid) {
		int celldim, physdim;
		char basename[32];
		char bitername[32];
		int nsteps;
		cg_base_read(fn, baseid, basename, &celldim, &physdim);

		// setup baseIterative.
		ier = cg_biter_read(fn, baseid, bitername, &nsteps);
		if (ier == 0) {
			cg_goto(fn, baseid, bitername, 0, "end");
			int narrays;
			cg_narrays(&narrays);
			DataTypeInfo ti;
			ti.dimension = PostSolutionInfo::fromIntDimension(celldim);
			ti.dataType = dtBaseIterative;
			ti.gridType = 0;
			ti.zoneId = 0;
			ti.gridLocation = GridLocationNull;
			for (int i = 1; i <= narrays; ++i) {
				char arrayname[32];
				DataType_t datatype;
				int datadim;
				cgsize_t dimVec[3];
				cg_array_info(i, arrayname, &datatype, &datadim, dimVec);
				QString aName(arrayname);
				if (aName != "TimeValues" && aName != "IterationValues") {
					ti.dataNamesMap[ti.gridLocation].append(aName);
				}
			}
			if (ti.dataNamesMap[ti.gridLocation].count() > 0) {
				m_dataTypeInfos.append(ti);
			}
		}
		// setup zone datas.
		// for zone datas, use sol.
		PostSolutionInfo::Dimension dim = PostSolutionInfo::fromIntDimension(celldim);
		QList<PostZoneDataContainer*> conts = sol->zoneContainers(dim);
		for (int i = 0; i < conts.count(); ++i) {
			PostZoneDataContainer* cont = conts.at(i);
			DataTypeInfo ti;
			ti.dimension = dim;
			vtkStructuredGrid* sgrid = dynamic_cast<vtkStructuredGrid*>(cont->data());
			if (sgrid != nullptr) {
				// structured data.
				switch (dim) {
				case PostSolutionInfo::dim1D:
					ti.dataType = dtDim1DStructured;
					break;
				case PostSolutionInfo::dim2D:
					ti.dataType = dtDim2DStructured;
					break;
				case PostSolutionInfo::dim3D:
					ti.dataType = dtDim3DStructured;
					break;
				default:
					ti.dataType = dtDim3DStructured;
				}
			} else {
				// unstructured data.
				switch (dim) {
				case PostSolutionInfo::dim1D:
					ti.dataType = dtDim1DUnstructured;
					break;
				case PostSolutionInfo::dim2D:
					ti.dataType = dtDim2DUnstructured;
					break;
				case PostSolutionInfo::dim3D:
					ti.dataType = dtDim3DUnstructured;
					break;
				default:
					ti.dataType = dtDim3DUnstructured;
				}
			}
			ti.gridType = cont->gridType();
			ti.zoneId = cont->zoneId();
			ti.zoneName = cont->zoneName();
			if (cont->data() == nullptr) {return false;}

			ti.gridLocation = Vertex;
			for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData())) {
				ti.dataNamesMap[Vertex].append(name.c_str());
			}
			for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetCellData())) {
				ti.dataNamesMap[CellCenter].append(name.c_str());
			}
			m_dataTypeInfos.append(ti);
		}
	}

	delete opener;

	setupMap();
#if SKIP
	setupPolyLines();
#endif
	return (m_dataTypeInfos.size() > 0);
}

void Graph2dVerificationWindowResultSetting::setupMap()
{
#if SKIP
	m_dataTypeInfoMap.clear();

	// setup properly. setup map next.
	// for time
	QMap<DimType, QList<DataTypeInfo*> > tmpmap;
	QList<DataTypeInfo*> emptylist;
	tmpmap.insert(dimBase, emptylist);
	tmpmap.insert(dim1D, emptylist);
	tmpmap.insert(dim2D, emptylist);
	tmpmap.insert(dim3D, emptylist);

	for (int i = 0; i < m_dataTypeInfos.count(); ++i) {
		DataTypeInfo& di = m_dataTypeInfos[i];
		DimType dt = dimTypeFromDataType(di.dataType);
		tmpmap[dt].append(&di);
	}
	m_dataTypeInfoMap.insert(xaTime, tmpmap);

	// for I
	tmpmap.clear();
	tmpmap.insert(dimBase, emptylist);
	tmpmap.insert(dim1D, emptylist);
	tmpmap.insert(dim2D, emptylist);
	tmpmap.insert(dim3D, emptylist);

	for (int i = 0; i < m_dataTypeInfos.count(); ++i) {
		DataTypeInfo& di = m_dataTypeInfos[i];
		DimType dt;
		switch (di.dataType) {
		case dtDim1DStructured:
		case dtDim2DStructured:
		case dtDim3DStructured:
			dt = dimTypeFromDataType(di.dataType);
			tmpmap[dt].append(&di);
			break;
		default:
			;
		}
	}
	m_dataTypeInfoMap.insert(xaI, tmpmap);

	// for J
	tmpmap.clear();
	tmpmap.insert(dimBase, emptylist);
	tmpmap.insert(dim1D, emptylist);
	tmpmap.insert(dim2D, emptylist);
	tmpmap.insert(dim3D, emptylist);

	for (int i = 0; i < m_dataTypeInfos.count(); ++i) {
		DataTypeInfo& di = m_dataTypeInfos[i];
		DimType dt;
		switch (di.dataType) {
		case dtDim2DStructured:
		case dtDim3DStructured:
			dt = dimTypeFromDataType(di.dataType);
			tmpmap[dt].append(&di);
			break;
		default:
			;
		}
	}
	m_dataTypeInfoMap.insert(xaJ, tmpmap);

	// for K
	tmpmap.clear();
	tmpmap.insert(dimBase, emptylist);
	tmpmap.insert(dim1D, emptylist);
	tmpmap.insert(dim2D, emptylist);
	tmpmap.insert(dim3D, emptylist);

	for (int i = 0; i < m_dataTypeInfos.count(); ++i) {
		DataTypeInfo& di = m_dataTypeInfos[i];
		DimType dt;
		switch (di.dataType) {
		case dtDim3DStructured:
			dt = dimTypeFromDataType(di.dataType);
			tmpmap[dt].append(&di);
			break;
		default:
			;
		}
	}
	m_dataTypeInfoMap.insert(xaK, tmpmap);

	// for Polyline
	if (m_polyLines.size() == 0) return;
	tmpmap.clear();
	tmpmap.insert(dimBase, emptylist);
	tmpmap.insert(dim1D, emptylist);
	tmpmap.insert(dim2D, emptylist);
	tmpmap.insert(dim3D, emptylist);

	for (int i = 0; i < m_dataTypeInfos.count(); ++i) {
		DataTypeInfo& di = m_dataTypeInfos[i];
		DimType dt;
		switch (di.dataType) {
		case dtDim1DStructured:
		case dtDim1DUnstructured:
			// @todo implement this!
			break;
		case dtDim2DStructured:
		case dtDim2DUnstructured:
			dt = dimTypeFromDataType(di.dataType);
			tmpmap[dt].append(&di);
			break;
		case dtDim3DStructured:
		case dtDim3DUnstructured:
			// @todo implement this!
			break;
		default:
			;
		}
	}
	m_dataTypeInfoMap.insert(xaPolyline, tmpmap);
#endif
}

#if SKIP
void Graph2dVerificationWindowResultSetting::setupPolyLines()
{
	m_polyLines.clear();
	if (m_postSolutionInfo) {
		m_polyLines = polyLines(m_postSolutionInfo);
	}
}

QList<GeoDataPolyLine*> Graph2dVerificationWindowResultSetting::polyLines(const PostSolutionInfo* info)
{
	auto preModel = info->iricMainWindow()->preProcessorWindow()->dataModel();
	auto refGroup = preModel->geoDataTopDataItem()->groupDataItem(std::string("_referenceinformation"));

	QList<GeoDataPolyLine*> ret;

	for (auto item : refGroup->geoDatas()) {
		auto data = item->geoData();
		auto polyLine = dynamic_cast<GeoDataPolyLine*>(data);
		if (polyLine == nullptr) {continue;}

		ret.push_back(polyLine);
	}
	return ret;
}
#endif


Graph2dVerificationWindowResultSetting::DimType Graph2dVerificationWindowResultSetting::dimTypeFromDataType(DataType dt)
{
	switch (dt) {
	case dtBaseIterative:
		return dimBase;
		break;
	case dtDim1DStructured:
	case dtDim1DUnstructured:
		return dim1D;
		break;
	case dtDim2DStructured:
	case dtDim2DUnstructured:
		return dim2D;
		break;
	case dtDim3DStructured:
	case dtDim3DUnstructured:
		return dim3D;
		break;
	}
	return dimBase;
}

bool Graph2dVerificationWindowResultSetting::settingExists()
{
#if 0 || 1
	if (m_targetDataTypeInfo == nullptr) {return false;}
#endif
#if SKIP
	if (m_targetDatas.count() == 0) {return false;}
#endif
	return true;
}

QList<Graph2dWindowDataItem*> Graph2dVerificationWindowResultSetting::setupItems(Graph2dVerificationWindowResultGroupDataItem* gItem) const
{
	QList<Graph2dWindowDataItem*> ret;
	if (m_targetDataTypeInfo == nullptr) {return ret;}
	if (m_xAxisMode == xaTime) {
		if (m_targetDataTypeInfo->dataType == Graph2dVerificationWindowResultSetting::dtBaseIterative) {
#if SKIP
			for (int i = 0; i < m_targetDatas.count(); ++i) {
				Graph2dVerificationWindowBaseIterativeResultDataItem* item = new Graph2dVerificationWindowBaseIterativeResultDataItem(m_targetDatas[i], i, gItem);
				ret.append(item);
			}
#endif
		} else {
#if SKIP
			for (int i = 0; i < m_targetDatas.count(); ++i) {
				Graph2dVerificationWindowGridPointResultDataItem* item = new Graph2dVerificationWindowGridPointResultDataItem(m_targetDatas[i], i, gItem);
				ret.append(item);
			}
#endif
		}
	//} else if (m_xAxisMode == xaPolyline) {
	//	// xaxis is distance (using a polyline)
	//	for (int i = 0; i < m_targetDatas.count(); ++i) {
	//		Graph2dVerificationWindowGridPolylineResultDataItem* item = new Graph2dVerificationWindowGridPolylineResultDataItem(m_targetDatas[i], i, gItem);
	//		ret.append(item);
	//	}
	} else {
		// xaxis is I or J or K
#if SKIP
		for (int i = 0; i < m_targetDatas.count(); ++i) {
			Graph2dVerificationWindowGridIJKResultDataItem* item = new Graph2dVerificationWindowGridIJKResultDataItem(m_targetDatas[i], i, gItem);
			ret.append(item);
		}
#endif
	}
	return ret;
}

QString Graph2dVerificationWindowResultSetting::autoYAxisLabel(AxisSide as) const
{
	QStringList labels;
#if SKIP
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		const Setting& s = m_targetDatas[i];
		if (s.axisSide() == as) {
			labels.append(s.name());
		}
	}
#endif
	return labels.join(", ");
}

bool Graph2dVerificationWindowResultSetting::axisNeeded(AxisSide as)
{
#if SKIP
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		const Setting& s = m_targetDatas[i];
		if (s.axisSide() == as) {return true;}
	}
#endif
	return false;
}

Graph2dVerificationWindowResultSetting& Graph2dVerificationWindowResultSetting::operator=(const Graph2dVerificationWindowResultSetting& s)
{
#if 1
	m_graphType = s.m_graphType;
	m_postSolutionInfo = s.m_postSolutionInfo;
	m_measuredData = s.m_measuredData;

	m_timeStep = s.m_timeStep;
	m_activePostData = s.m_activePostData;
	m_activeResult = s.m_activeResult;

	m_activeMeasuredData = s.m_activeMeasuredData;
	m_activeValue = s.m_activeValue;

	m_activeFile = s.m_activeFile;
	m_activeZone = s.m_activeZone;
#endif

	m_xAxisMode = s.m_xAxisMode;
#if SKIP
	m_timeValueType = s.m_timeValueType;
	m_positionValueType = s.m_positionValueType;
	m_xAxisLabel = s.m_xAxisLabel;
	m_xAxisReverse = s.m_xAxisReverse;
	m_xAxisAutoRange = s.m_xAxisAutoRange;
	m_xAxisValueMin = s.m_xAxisValueMin;
	m_xAxisValueMax = s.m_xAxisValueMax;
	m_xAxisLog = s.m_xAxisLog;

	m_yAxisLeftAutoRange = s.m_yAxisLeftAutoRange;
	m_yAxisLeftMin = s.m_yAxisLeftMin;
	m_yAxisLeftMax = s.m_yAxisLeftMax;
	m_yAxisLeftTitle = s.m_yAxisLeftTitle;
	m_yAxisLeftReverse = s.m_yAxisLeftReverse;
	m_yAxisLeftLog = s.m_yAxisLeftLog;
	m_yAxisRightAutoRange = s.m_yAxisRightAutoRange;
	m_yAxisRightMin = s.m_yAxisRightMin;
	m_yAxisRightMax = s.m_yAxisRightMax;
	m_yAxisRightTitle = s.m_yAxisRightTitle;
	m_yAxisRightReverse = s.m_yAxisRightReverse;
	m_yAxisRightLog = s.m_yAxisRightLog;
#endif

#if 0 || 1
	m_dataTypeInfos = s.m_dataTypeInfos;
#endif
#if SKIP
	m_targetDatas = s.m_targetDatas;
	m_polyLines = s.m_polyLines;
	m_targetPolyLine = s.m_targetPolyLine;
#endif
	m_postSolutionInfo = s.m_postSolutionInfo;
	setupMap();
#if SKIP
	setupPolyLines();
#endif
#if 0 || 1
	if (s.m_targetDataTypeInfo == nullptr) {
		m_targetDataTypeInfo = nullptr;
	} else {
		m_targetDataTypeInfo = nullptr;
		for (int i = 0; i < m_dataTypeInfos.count(); ++i) {
			DataTypeInfo& info = m_dataTypeInfos[i];
			if (*s.m_targetDataTypeInfo == info) {
				m_targetDataTypeInfo = &info;
			}
		}
	}
#endif
#if SKIP
	m_title = s.m_title;
	m_addIndicesToTitle = s.m_addIndicesToTitle;

	m_I = s.m_I;
	m_J     = s.m_J;
	m_K     = s.m_K;
	m_index = s.m_index;
#endif
	return *this;
}

bool Graph2dVerificationWindowResultSetting::dataAvailable()
{
#if 0 || 1
#if SKIP
	for (auto it = m_dataTypeInfoMap.begin(); it != m_dataTypeInfoMap.end(); ++it) {
		for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
			if (it2.value().count() > 0) {return true;}
		}
	}
#endif
#endif
	return false;
}

#if SKIP
QColor Graph2dVerificationWindowResultSetting::autoColor(int index) const
{
	return m_colorSource->getColor(index);
}

QString Graph2dVerificationWindowResultSetting::autoXAxisLabel(Graph2dVerificationWindowResultSetting::XAxisMode xm)
{
	switch (xm) {
	case Graph2dVerificationWindowResultSetting::xaTime:
		return Graph2dVerificationWindow::tr("Time");
		break;
	case Graph2dVerificationWindowResultSetting::xaI:
		return Graph2dVerificationWindow::tr("I");
		break;
	case Graph2dVerificationWindowResultSetting::xaJ:
		return Graph2dVerificationWindow::tr("J");
		break;
	case Graph2dVerificationWindowResultSetting::xaK:
		return Graph2dVerificationWindow::tr("K");
		break;
	case Graph2dVerificationWindowResultSetting::xaPolyline:
		return Graph2dVerificationWindow::tr("Polyline");
		break;
	}
	return "";
}

void Graph2dVerificationWindowResultSetting::setAutoXAxisLabel()
{
	QString xAxisLabel;
	if (m_xAxisMode == Graph2dVerificationWindowResultSetting::xaTime) {
		xAxisLabel = autoXAxisTimeLabel(m_xAxisMode, m_timeValueType);
	} else {
		xAxisLabel = autoXAxisPositionLabel(m_xAxisMode, m_positionValueType);
	}
	m_xAxisLabel = xAxisLabel;
}

QString Graph2dVerificationWindowResultSetting::autoXAxisTimeLabel(Graph2dVerificationWindowResultSetting::XAxisMode /*mode*/, Graph2dVerificationWindowResultSetting::TimeValueType t)
{
	if (t == Graph2dVerificationWindowResultSetting::tvtTime) {
		return Graph2dVerificationWindow::tr("Time");
	} else {
		return Graph2dVerificationWindow::tr("Count");
	}
}

QString Graph2dVerificationWindowResultSetting::autoXAxisPositionLabel(Graph2dVerificationWindowResultSetting::XAxisMode mode, Graph2dVerificationWindowResultSetting::PositionValueType t)
{
	if (t == Graph2dVerificationWindowResultSetting::pvtDistance) {
		return Graph2dVerificationWindow::tr("Distance");
	} else {
		switch (mode) {
		case Graph2dVerificationWindowResultSetting::xaI:
			return Graph2dVerificationWindow::tr("I");
			break;
		case Graph2dVerificationWindowResultSetting::xaJ:
			return Graph2dVerificationWindow::tr("J");
			break;
		case Graph2dVerificationWindowResultSetting::xaK:
			return Graph2dVerificationWindow::tr("K");
			break;
		default:
			break;
		}
	}
	return "";
}
#endif


Qt::PenStyle Graph2dVerificationWindowResultSetting::getPenStyle(LineType lt)
{

	switch (lt) {
	case ltSolidLine:
		return Qt::SolidLine;
		break;
	case ltDashLine:
		return Qt::DashLine;
		break;
	case ltDotLine:
		return Qt::DotLine;
		break;
	case ltDashDotLine:
		return Qt::DashDotLine;
		break;
	default:
		return Qt::SolidLine;
	}
}

QwtSymbol::Style Graph2dVerificationWindowResultSetting::getSymbolStyle(SymbolType st)
{
	switch (st) {
	case symCircle:
		return QwtSymbol::Ellipse;
		break;
	case symRect:
		return QwtSymbol::Rect;
		break;
	case symDiamond:
		return QwtSymbol::Diamond;
		break;
	case symTriangle:
		return QwtSymbol::Triangle;
		break;
	case symCross:
		return QwtSymbol::Cross;
		break;
	case symXCross:
		return QwtSymbol::XCross;
		break;
	default:
		return QwtSymbol::Ellipse;
	}
}

GridLocation_t Graph2dVerificationWindowResultSetting::getGridLocation(QString string)
{
	static QMap<QString, GridLocation_t> map{
		{ "GridLocationNull", GridLocationNull },
		{ "Vertex",           Vertex },
		{ "CellCenter",       CellCenter }
	};
	Q_ASSERT(map.find(string) != map.end());
	return map[string];
}

QString Graph2dVerificationWindowResultSetting::getGridLocationString(GridLocation_t location)
{
	static QMap<GridLocation_t, QString> map{
		{ GridLocationNull, "GridLocationNull" },
		{ Vertex,           "Vertex" },
		{ CellCenter,       "CellCenter" }
	};
	Q_ASSERT(map.find(location) != map.end());
	return map[location];
}

GridLocation_t Graph2dVerificationWindowResultSetting::getGridLocationTranslated(QString string)
{
	static QMap<QString, GridLocation_t> map{
		{ QObject::tr("GridLocationNull"), GridLocationNull },
		{ QObject::tr("Vertex"),           Vertex },
		{ QObject::tr("CellCenter"),       CellCenter }
	};
	Q_ASSERT(map.find(string) != map.end());
	return map[string];
}

QString Graph2dVerificationWindowResultSetting::getGridLocationStringTranslated(GridLocation_t location)
{
	static QMap<GridLocation_t, QString> map{
		{ GridLocationNull, QObject::tr("GridLocationNull") },
		{ Vertex,           QObject::tr("Vertex") },
		{ CellCenter,       QObject::tr("CellCenter") }
	};
	Q_ASSERT(map.find(location) != map.end());
	return map[location];
}

#if SKIP
void Graph2dVerificationWindowResultSetting::Setting::setupCurve(QwtPlotCustomCurve* curve) const
{
	if (m_axisSide == Graph2dVerificationWindowResultSetting::asLeft) {
		curve->setYAxis(QwtPlot::yLeft);
	} else {
		curve->setYAxis(QwtPlot::yRight);
	}

	if (m_styleType == Graph2dVerificationWindowResultSetting::stLine) {
		// line
		QPen pen;
		pen.setColor(m_customColor);
		pen.setWidth(m_lineWidth);
		pen.setStyle(Graph2dVerificationWindowResultSetting::getPenStyle(m_lineType));
		curve->setPen(pen);
		QwtSymbol* symbol = new QwtSymbol();
		symbol->setStyle(QwtSymbol::NoSymbol);
		curve->setSymbol(symbol);
	} else {
		// symbol
		// no line.
		QPen pen;
		pen.setColor(Qt::transparent);
		pen.setWidth(1);
		pen.setStyle(Qt::SolidLine);
		curve->setPen(pen);
		QwtSymbol* symbol = new QwtSymbol();
		symbol->setStyle(Graph2dVerificationWindowResultSetting::getSymbolStyle(m_symbolType));
		symbol->setSize(9);
		pen.setColor(m_customColor);
		pen.setWidth(m_lineWidth);
		pen.setStyle(Qt::SolidLine);
		symbol->setPen(pen);
		QBrush brush;
		brush.setColor(m_customColor);
		curve->setBrush(brush);
		curve->setSymbol(symbol);
	}
}
#endif

void Graph2dVerificationWindowResultSetting::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_xAxisMode = static_cast<XAxisMode>(iRIC::getIntAttribute(node, "xAxisMode"));
#if SKIP
	m_title = elem.attribute("title");
	m_addIndicesToTitle = iRIC::getBooleanAttribute(node, "addIndicesToTitle");
	m_timeValueType = static_cast<TimeValueType>(iRIC::getIntAttribute(node, "timeValueType"));
	m_positionValueType = static_cast<PositionValueType>(iRIC::getIntAttribute(node, "positionValueType"));
	m_xAxisReverse = iRIC::getBooleanAttribute(node, "xAxisReverse");
	m_xAxisLabel = elem.attribute("xAxisLabel");
	m_xAxisLog = iRIC::getBooleanAttribute(node, "xAxisLog");

	m_yAxisLeftAutoRange = iRIC::getBooleanAttribute(node, "yAxisLeftAutoRange");
	m_yAxisLeftMin = iRIC::getDoubleAttribute(node, "yAxisLeftMin");
	m_yAxisLeftMax = iRIC::getDoubleAttribute(node, "yAxisLeftMax");
	m_yAxisLeftTitle = elem.attribute("yAxisLeftTitle");
	m_yAxisLeftReverse = iRIC::getBooleanAttribute(node, "yAxisLeftReverse");
	m_yAxisLeftLog = iRIC::getBooleanAttribute(node, "yAxisLeftLog");

	m_yAxisRightAutoRange = iRIC::getBooleanAttribute(node, "yAxisRightAutoRange");
	m_yAxisRightMin = iRIC::getDoubleAttribute(node, "yAxisRightMin");
	m_yAxisRightMax = iRIC::getDoubleAttribute(node, "yAxisRightMax");
	m_yAxisRightTitle = elem.attribute("yAxisRightTitle");
	m_yAxisRightReverse = iRIC::getBooleanAttribute(node, "yAxisRightReverse");
	m_yAxisRightLog = iRIC::getBooleanAttribute(node, "yAxisRightLog");

	m_xAxisAutoRange = iRIC::getBooleanAttribute(node, "xAxisAutoRange");
	m_xAxisValueMin = iRIC::getDoubleAttribute(node, "xAxisValueMin");
	m_xAxisValueMax = iRIC::getDoubleAttribute(node, "xAxisValueMax");

#if 0
	m_I = iRIC::getIntAttribute(node, "i");
	m_J = iRIC::getIntAttribute(node, "j");
	m_K = iRIC::getIntAttribute(node, "k");
#endif
	m_index = iRIC::getIntAttribute(node, "index");
#endif


	//{{
	m_graphType = static_cast<GraphType>(iRIC::getIntAttribute(node, "graphType"));
	m_activeResult = elem.attribute("activeResult");
	m_activeValue = elem.attribute("activeValue");
	m_activeFile = elem.attribute("activeFile");
	m_activeZone = elem.attribute("activeZone");


	for (MeasuredData* md : m_measuredData) {
		if (m_activeFile == md->name()) {
			m_activeMeasuredData = md;
			break;
		}
	}

	for (PostZoneDataContainer* cont : m_postSolutionInfo->zoneContainers2D()) {
		if (m_activeZone == cont->zoneName().c_str()) {
			m_activePostData = cont;
			break;
		}
	}
	//}}

	QDomNode typeNode = iRIC::getChildNode(node, "targetDataType");
	if (! typeNode.isNull()) {
#if 0
		DataTypeInfo info;
		info.loadFromProjectMainFile(typeNode);
		for (int i = 0; i < m_dataTypeInfos.count(); ++i) {
			DataTypeInfo& tmpInfo = m_dataTypeInfos[i];
			if (tmpInfo == info) {
				Q_ASSERT(m_targetDataTypeInfo == nullptr);
				m_targetDataTypeInfo = &tmpInfo;
				m_targetDataTypeInfo->gridLocation = info.gridLocation;
			}
		}
#endif
	}
#if 0
	Q_ASSERT(m_targetDataTypeInfo != nullptr);
#endif
#if SKIP
	m_targetDatas.clear();
	QDomNode datasNode = iRIC::getChildNode(node, "TargetDatas");
	if (! datasNode.isNull()) {
		for (int i = 0; i < datasNode.childNodes().count(); ++i) {
			Graph2dVerificationWindowResultSetting::Setting setting;
			setting.loadFromProjectMainFile(datasNode.childNodes().at(i));
			m_targetDatas.append(setting);
		}
	}

	m_targetPolyLine = nullptr;
	QDomNode targetPolyLineNode = iRIC::getChildNode(node, "TargetPolyLine");
	if (! targetPolyLineNode.isNull()) {
		QDomElement targetPolyLineElement = targetPolyLineNode.toElement();
		QString name = targetPolyLineElement.attribute("name", "nullptr");
		for (auto &line : m_polyLines) {
			if (line->name() == name) {
				m_targetPolyLine = line;
				break;
			}
		}
		if (name == "nullptr") Q_ASSERT(m_targetPolyLine == nullptr);
		Q_ASSERT(m_targetPolyLine == nullptr || name != "nullptr");
	}
#endif
}

void Graph2dVerificationWindowResultSetting::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	//// targetDatas
	iRIC::setIntAttribute(writer, "xAxisMode", static_cast<int>(m_xAxisMode));
#if SKIP
	writer.writeAttribute("title", m_title);
	iRIC::setBooleanAttribute(writer, "addIndicesToTitle", m_addIndicesToTitle);
	iRIC::setIntAttribute(writer, "timeValueType", static_cast<int>(m_timeValueType));
	iRIC::setIntAttribute(writer, "positionValueType", static_cast<int>(m_positionValueType));
	iRIC::setBooleanAttribute(writer, "xAxisReverse", m_xAxisReverse);
	writer.writeAttribute("xAxisLabel", m_xAxisLabel);
	iRIC::setBooleanAttribute(writer, "xAxisLog", m_xAxisLog);

	iRIC::setBooleanAttribute(writer, "yAxisLeftAutoRange", m_yAxisLeftAutoRange);
	iRIC::setDoubleAttribute(writer, "yAxisLeftMin", m_yAxisLeftMin);
	iRIC::setDoubleAttribute(writer, "yAxisLeftMax", m_yAxisLeftMax);
	writer.writeAttribute("yAxisLeftTitle", m_yAxisLeftTitle);
	iRIC::setBooleanAttribute(writer, "yAxisLeftReverse", m_yAxisLeftReverse);
	iRIC::setBooleanAttribute(writer, "yAxisLeftLog", m_yAxisLeftLog);

	iRIC::setBooleanAttribute(writer, "yAxisRightAutoRange", m_yAxisRightAutoRange);
	iRIC::setDoubleAttribute(writer, "yAxisRightMin", m_yAxisRightMin);
	iRIC::setDoubleAttribute(writer, "yAxisRightMax", m_yAxisRightMax);
	writer.writeAttribute("yAxisRightTitle", m_yAxisRightTitle);
	iRIC::setBooleanAttribute(writer, "yAxisRightReverse", m_yAxisRightReverse);
	iRIC::setBooleanAttribute(writer, "yAxisRightLog", m_yAxisRightLog);

	iRIC::setBooleanAttribute(writer, "xAxisAutoRange", m_xAxisAutoRange);
	iRIC::setDoubleAttribute(writer, "xAxisValueMin", m_xAxisValueMin);
	iRIC::setDoubleAttribute(writer, "xAxisValueMax", m_xAxisValueMax);

#if 0
	iRIC::setIntAttribute(writer, "i", m_I);
	iRIC::setIntAttribute(writer, "j", m_J);
	iRIC::setIntAttribute(writer, "k", m_K);
#endif
	iRIC::setIntAttribute(writer, "index", m_index);
#endif

	//{{
	iRIC::setIntAttribute(writer, "graphType", static_cast<int>(m_graphType));
	writer.writeAttribute("activeResult", m_activeResult);
	writer.writeAttribute("activeValue", m_activeValue);
	writer.writeAttribute("activeFile", m_activeFile);
	writer.writeAttribute("activeZone", m_activeZone);
	//}}


#if 0
	writer.writeStartElement("targetDataType");
	Q_ASSERT(m_targetDataTypeInfo != nullptr);
	m_targetDataTypeInfo->saveToProjectMainFile(writer);
	writer.writeEndElement();
#endif

#if 0
	writer.writeStartElement("TargetDatas");
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		writer.writeStartElement("TargetData");
		const Graph2dVerificationWindowResultSetting::Setting& setting = m_targetDatas.at(i);
		setting.saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
#endif

#if 0
	writer.writeStartElement("TargetPolyLine");
	if (m_targetPolyLine == nullptr) {
		writer.writeAttribute("name", "nullptr");
	} else {
		writer.writeAttribute("name", m_targetPolyLine->name());
	}
	writer.writeEndElement();
#endif
}

void Graph2dVerificationWindowResultSetting::setActivePostData(PostZoneDataContainer* postZoneDataContainer)
{
	m_activePostData = postZoneDataContainer;
}
#if 0 || 1
void Graph2dVerificationWindowResultSetting::DataTypeInfo::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();	// targetDataType
	dataType = static_cast<DataType>(iRIC::getIntAttribute(node, "dataType"));
	dimension = static_cast<PostSolutionInfo::Dimension>(iRIC::getIntAttribute(node, "dimension"));
	zoneId = iRIC::getIntAttribute(node, "zoneId");
	zoneName = iRIC::toStr(elem.attribute("zoneName"));
	QString loc = elem.attribute("gridLocation", Graph2dVerificationWindowResultSetting::getGridLocationString(Vertex));
	gridLocation = Graph2dVerificationWindowResultSetting::getGridLocation(loc);

	QDomNode namesNode = node.firstChild();
	while (! namesNode.isNull()) {
		if (namesNode.nodeName() == "DataNames") {
			QString att = namesNode.toElement().attribute("gridLocation", Graph2dVerificationWindowResultSetting::getGridLocationString(Vertex));
			GridLocation_t loc = Graph2dVerificationWindowResultSetting::getGridLocation(att);
			QDomNodeList names = namesNode.childNodes();
			for (int i = 0; i < names.count(); ++i) {
				QDomElement elem = names.at(i).toElement();
				QString name = elem.attribute("name");
				dataNamesMap[loc].append(name);
			}
		}
		namesNode = namesNode.nextSibling();
	}
}
void Graph2dVerificationWindowResultSetting::DataTypeInfo::saveToProjectMainFile(QXmlStreamWriter& writer) const
{
	iRIC::setIntAttribute(writer, "dataType", static_cast<int>(dataType));
	iRIC::setIntAttribute(writer, "dimension", static_cast<int>(dimension));
	iRIC::setIntAttribute(writer, "zoneId", zoneId);
	writer.writeAttribute("zoneName", zoneName.c_str());
	writer.writeAttribute("gridLocation", Graph2dVerificationWindowResultSetting::getGridLocationString(gridLocation));

	for (auto loc : dataNamesMap.keys()) {
		writer.writeStartElement("DataNames");
		writer.writeAttribute("gridLocation", Graph2dVerificationWindowResultSetting::getGridLocationString(loc));
		for (auto name : dataNamesMap[loc]) {
			writer.writeStartElement("DataName");
			writer.writeAttribute("name", name);
			writer.writeEndElement();
		}
		writer.writeEndElement();
	}
}
#endif

#if SKIP
void Graph2dVerificationWindowResultSetting::Setting::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();

	m_name = elem.attribute("name");
	m_axisSide = static_cast<AxisSide>(iRIC::getIntAttribute(node, "axisSide"));
	m_lineWidth = iRIC::getIntAttribute(node, "lineWidth");
	m_customColor = iRIC::getColorAttribute(node, "customColor");
	m_styleType = static_cast<StyleType>(iRIC::getIntAttribute(node, "styleType"));
	m_lineType = static_cast<LineType>(iRIC::getIntAttribute(node, "lineType"));
	m_symbolType = static_cast<SymbolType>(iRIC::getIntAttribute(node, "symbolType"));
	m_barChart = iRIC::getBooleanAttribute(node, "barChart", false);
}

void Graph2dVerificationWindowResultSetting::Setting::saveToProjectMainFile(QXmlStreamWriter& writer) const
{
	writer.writeAttribute("name", m_name);
	iRIC::setIntAttribute(writer, "axisSide", static_cast<int>(m_axisSide));
	iRIC::setIntAttribute(writer, "lineWidth", m_lineWidth);
	iRIC::setColorAttribute(writer, "customColor", m_customColor);
	iRIC::setIntAttribute(writer, "styleType", static_cast<int>(m_styleType));
	iRIC::setIntAttribute(writer, "lineType", static_cast<int>(m_lineType));
	iRIC::setIntAttribute(writer, "symbolType", static_cast<int>(m_symbolType));
	iRIC::setBooleanAttribute(writer, "barChart", m_barChart);
}
#endif