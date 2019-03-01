#include "datamodel/graph2dhybridwindowbaseiterativeresultdataitem.h"
#include "datamodel/graph2dhybridwindowgridijkresultdataitem.h"
#include "datamodel/graph2dhybridwindowgridpointresultdataitem.h"
#include "datamodel/graph2dhybridwindowgridpolylineresultdataitem.h"
#include "datamodel/graph2dhybridwindowresultgroupdataitem.h"
#include "graph2dhybridwindow.h"
#include "graph2dhybridwindowresultsetting.h"

#include <geodata/polyline/geodatapolyline.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>

#if defined(_MSC_VER)
// disable macro redefinition warnings
#pragma warning( push )
#pragma warning( disable : 4005 )
#endif

#include <guibase/qwtplotcustomcurve.h>
#include <guicore/postcontainer/postzonedatacontainer.h>

#if defined(_MSC_VER)
// re-enable macro redefinition warnings
#pragma warning( pop )
#endif

#include <guicore/misc/cgnsfileopener.h>
#include <guicore/project/colorsource.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QList>

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

#include <cgnslib.h>
#include <qwt_plot.h>

#include <stdexcept>

Graph2dHybridWindowResultSetting::Graph2dHybridWindowResultSetting()
{
	m_xAxisMode = xaTime;
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
}

Graph2dHybridWindowResultSetting::~Graph2dHybridWindowResultSetting()
{
	delete m_colorSource;
}

bool Graph2dHybridWindowResultSetting::init(PostSolutionInfo* sol, const QString& cgnsFilename)
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
	setupPolyLines();
	return (m_dataTypeInfos.size() > 0);
}

void Graph2dHybridWindowResultSetting::setupMap()
{
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

	/*
		// for index
		tmpmap.clear();
		tmpmap.insert(dimBase, emptylist);
		tmpmap.insert(dim1D, emptylist);
		tmpmap.insert(dim2D, emptylist);
		tmpmap.insert(dim3D, emptylist);

		for (int i = 0; i < m_dataTypeInfos.count(); ++i){
			DataTypeInfo& di = m_dataTypeInfos[i];
			DimType dt;
			switch (di.dataType){
			case dtDim1DUnstructured:
			case dtDim2DUnstructured:
			case dtDim3DUnstructured:
				dt = dimTypeFromDataType(di.dataType);
				tmpmap[dt].append(&di);
				break;
			default:
				;
			}
		}
		m_dataTypeInfoMap.insert(xaIndex, tmpmap);
	*/
}

void Graph2dHybridWindowResultSetting::setupPolyLines()
{
	m_polyLines.clear();
	if (m_postSolutionInfo) {
		m_polyLines = polyLines(m_postSolutionInfo);
	}
}

QList<GeoDataPolyLine*> Graph2dHybridWindowResultSetting::polyLines(const PostSolutionInfo* info)
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

Graph2dHybridWindowResultSetting::DimType Graph2dHybridWindowResultSetting::dimTypeFromDataType(DataType dt)
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

bool Graph2dHybridWindowResultSetting::settingExists()
{
	if (m_targetDataTypeInfo == nullptr) {return false;}
	if (m_targetDatas.count() == 0) {return false;}
	return true;
}

QList<Graph2dWindowDataItem*> Graph2dHybridWindowResultSetting::setupItems(Graph2dHybridWindowResultGroupDataItem* gItem) const
{
	QList<Graph2dWindowDataItem*> ret;
	if (m_targetDataTypeInfo == nullptr) {return ret;}
	if (m_xAxisMode == xaTime) {
		if (m_targetDataTypeInfo->dataType == Graph2dHybridWindowResultSetting::dtBaseIterative) {
			for (int i = 0; i < m_targetDatas.count(); ++i) {
				Graph2dHybridWindowBaseIterativeResultDataItem* item = new Graph2dHybridWindowBaseIterativeResultDataItem(m_targetDatas[i], i, gItem);
				ret.append(item);
			}
		} else {
			for (int i = 0; i < m_targetDatas.count(); ++i) {
				Graph2dHybridWindowGridPointResultDataItem* item = new Graph2dHybridWindowGridPointResultDataItem(m_targetDatas[i], i, gItem);
				ret.append(item);
			}
		}
	} else if (m_xAxisMode == xaPolyline) {
		// xaxis is distance (using a polyline)
		for (int i = 0; i < m_targetDatas.count(); ++i) {
			Graph2dHybridWindowGridPolylineResultDataItem* item = new Graph2dHybridWindowGridPolylineResultDataItem(m_targetDatas[i], i, gItem);
			ret.append(item);
		}
	} else {
		// xaxis is I or J or K
		for (int i = 0; i < m_targetDatas.count(); ++i) {
			Graph2dHybridWindowGridIJKResultDataItem* item = new Graph2dHybridWindowGridIJKResultDataItem(m_targetDatas[i], i, gItem);
			ret.append(item);
		}
	}
	return ret;
}

QString Graph2dHybridWindowResultSetting::autoYAxisLabel(AxisSide as) const
{
	QStringList labels;
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		const Setting& s = m_targetDatas[i];
		if (s.axisSide() == as) {
			labels.append(s.name());
		}
	}
	return labels.join(", ");
}

bool Graph2dHybridWindowResultSetting::axisNeeded(AxisSide as)
{
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		const Setting& s = m_targetDatas[i];
		if (s.axisSide() == as) {return true;}
	}
	return false;
}

Graph2dHybridWindowResultSetting& Graph2dHybridWindowResultSetting::operator=(const Graph2dHybridWindowResultSetting& s)
{
	m_xAxisMode = s.m_xAxisMode;
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

	m_dataTypeInfos = s.m_dataTypeInfos;
	m_targetDatas = s.m_targetDatas;
	m_polyLines = s.m_polyLines;
	m_targetPolyLine = s.m_targetPolyLine;
	m_postSolutionInfo = s.m_postSolutionInfo;
	setupMap();
	setupPolyLines();
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
	m_title = s.m_title;
	m_addIndicesToTitle = s.m_addIndicesToTitle;

	m_I     = s.m_I;
	m_J     = s.m_J;
	m_K     = s.m_K;
	m_index = s.m_index;
	return *this;
}

bool Graph2dHybridWindowResultSetting::dataAvailable()
{
	for (auto it = m_dataTypeInfoMap.begin(); it != m_dataTypeInfoMap.end(); ++it) {
		for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
			if (it2.value().count() > 0) {return true;}
		}
	}
	return false;
}

QColor Graph2dHybridWindowResultSetting::autoColor(int index) const
{
	return m_colorSource->getColor(index);
}

QString Graph2dHybridWindowResultSetting::autoXAxisLabel(Graph2dHybridWindowResultSetting::XAxisMode xm)
{
	switch (xm) {
	case Graph2dHybridWindowResultSetting::xaTime:
		return Graph2dHybridWindow::tr("Time");
		break;
	case Graph2dHybridWindowResultSetting::xaI:
		return Graph2dHybridWindow::tr("I");
		break;
	case Graph2dHybridWindowResultSetting::xaJ:
		return Graph2dHybridWindow::tr("J");
		break;
	case Graph2dHybridWindowResultSetting::xaK:
		return Graph2dHybridWindow::tr("K");
		break;
	case Graph2dHybridWindowResultSetting::xaPolyline:
		return Graph2dHybridWindow::tr("Polyline");
		break;
	}
	return "";
}

void Graph2dHybridWindowResultSetting::setAutoXAxisLabel()
{
	QString xAxisLabel;
	if (m_xAxisMode == Graph2dHybridWindowResultSetting::xaTime) {
		xAxisLabel = autoXAxisTimeLabel(m_xAxisMode, m_timeValueType);
	} else {
		xAxisLabel = autoXAxisPositionLabel(m_xAxisMode, m_positionValueType);
	}
	m_xAxisLabel = xAxisLabel;
}

QString Graph2dHybridWindowResultSetting::autoXAxisTimeLabel(Graph2dHybridWindowResultSetting::XAxisMode /*mode*/, Graph2dHybridWindowResultSetting::TimeValueType t)
{
	if (t == Graph2dHybridWindowResultSetting::tvtTime) {
		return Graph2dHybridWindow::tr("Time");
	} else {
		return Graph2dHybridWindow::tr("Count");
	}
}

QString Graph2dHybridWindowResultSetting::autoXAxisPositionLabel(Graph2dHybridWindowResultSetting::XAxisMode mode, Graph2dHybridWindowResultSetting::PositionValueType t)
{
	if (t == Graph2dHybridWindowResultSetting::pvtDistance) {
		return Graph2dHybridWindow::tr("Distance");
	} else {
		switch (mode) {
		case Graph2dHybridWindowResultSetting::xaI:
			return Graph2dHybridWindow::tr("I");
			break;
		case Graph2dHybridWindowResultSetting::xaJ:
			return Graph2dHybridWindow::tr("J");
			break;
		case Graph2dHybridWindowResultSetting::xaK:
			return Graph2dHybridWindow::tr("K");
			break;
		default:
			break;
		}
	}
	return "";
}

Qt::PenStyle Graph2dHybridWindowResultSetting::getPenStyle(LineType lt)
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

QwtSymbol::Style Graph2dHybridWindowResultSetting::getSymbolStyle(SymbolType st)
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

GridLocation_t Graph2dHybridWindowResultSetting::getGridLocation(QString string)
{
	static QMap<QString, GridLocation_t> map{
		{ "GridLocationNull", GridLocationNull },
		{ "Vertex",           Vertex },
		{ "CellCenter",       CellCenter }
	};
	Q_ASSERT(map.find(string) != map.end());
	return map[string];
}

QString Graph2dHybridWindowResultSetting::getGridLocationString(GridLocation_t location)
{
	static QMap<GridLocation_t, QString> map{
		{ GridLocationNull, "GridLocationNull" },
		{ Vertex,           "Vertex" },
		{ CellCenter,       "CellCenter" }
	};
	Q_ASSERT(map.find(location) != map.end());
	return map[location];
}

GridLocation_t Graph2dHybridWindowResultSetting::getGridLocationTranslated(QString string)
{
	static QMap<QString, GridLocation_t> map{
		{ QObject::tr("GridLocationNull"), GridLocationNull },
		{ QObject::tr("Vertex"),           Vertex },
		{ QObject::tr("CellCenter"),       CellCenter }
	};
	Q_ASSERT(map.find(string) != map.end());
	return map[string];
}

QString Graph2dHybridWindowResultSetting::getGridLocationStringTranslated(GridLocation_t location)
{
	static QMap<GridLocation_t, QString> map{
		{ GridLocationNull, QObject::tr("GridLocationNull") },
		{ Vertex,           QObject::tr("Vertex") },
		{ CellCenter,       QObject::tr("CellCenter") }
	};
	Q_ASSERT(map.find(location) != map.end());
	return map[location];
}

void Graph2dHybridWindowResultSetting::Setting::setupCurve(QwtPlotCustomCurve* curve) const
{
	if (m_axisSide == Graph2dHybridWindowResultSetting::asLeft) {
		curve->setYAxis(QwtPlot::yLeft);
	} else {
		curve->setYAxis(QwtPlot::yRight);
	}

	if (m_styleType == Graph2dHybridWindowResultSetting::stLine) {
		// line
		QPen pen;
		pen.setColor(m_customColor);
		pen.setWidth(m_lineWidth);
		pen.setStyle(Graph2dHybridWindowResultSetting::getPenStyle(m_lineType));
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
		symbol->setStyle(Graph2dHybridWindowResultSetting::getSymbolStyle(m_symbolType));
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

void Graph2dHybridWindowResultSetting::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_xAxisMode = static_cast<XAxisMode>(iRIC::getIntAttribute(node, "xAxisMode"));
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

	m_I = iRIC::getIntAttribute(node, "i");
	m_J = iRIC::getIntAttribute(node, "j");
	m_K = iRIC::getIntAttribute(node, "k");
	m_index = iRIC::getIntAttribute(node, "index");

	QDomNode typeNode = iRIC::getChildNode(node, "targetDataType");
	if (! typeNode.isNull()) {
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
	}
	Q_ASSERT(m_targetDataTypeInfo != nullptr);
	m_targetDatas.clear();
	QDomNode datasNode = iRIC::getChildNode(node, "TargetDatas");
	if (! datasNode.isNull()) {
		for (int i = 0; i < datasNode.childNodes().count(); ++i) {
			Graph2dHybridWindowResultSetting::Setting setting;
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
}

void Graph2dHybridWindowResultSetting::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	// targetDatas
	iRIC::setIntAttribute(writer, "xAxisMode", static_cast<int>(m_xAxisMode));
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

	iRIC::setIntAttribute(writer, "i", m_I);
	iRIC::setIntAttribute(writer, "j", m_J);
	iRIC::setIntAttribute(writer, "k", m_K);
	iRIC::setIntAttribute(writer, "index", m_index);

	writer.writeStartElement("targetDataType");
	m_targetDataTypeInfo->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("TargetDatas");
	for (int i = 0; i < m_targetDatas.count(); ++i) {
		writer.writeStartElement("TargetData");
		const Graph2dHybridWindowResultSetting::Setting& setting = m_targetDatas.at(i);
		setting.saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();

	writer.writeStartElement("TargetPolyLine");
	if (m_targetPolyLine == nullptr) {
		writer.writeAttribute("name", "nullptr");
	} else {
		writer.writeAttribute("name", m_targetPolyLine->name());
	}
	writer.writeEndElement();
}

void Graph2dHybridWindowResultSetting::DataTypeInfo::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();	// targetDataType
	dataType = static_cast<DataType>(iRIC::getIntAttribute(node, "dataType"));
	dimension = static_cast<PostSolutionInfo::Dimension>(iRIC::getIntAttribute(node, "dimension"));
	zoneId = iRIC::getIntAttribute(node, "zoneId");
	zoneName = iRIC::toStr(elem.attribute("zoneName"));
	QString loc = elem.attribute("gridLocation", Graph2dHybridWindowResultSetting::getGridLocationString(Vertex));
	gridLocation = Graph2dHybridWindowResultSetting::getGridLocation(loc);

	QDomNode namesNode = node.firstChild();
	while (! namesNode.isNull()) {
		if (namesNode.nodeName() == "DataNames") {
			QString att = namesNode.toElement().attribute("gridLocation", Graph2dHybridWindowResultSetting::getGridLocationString(Vertex));
			GridLocation_t loc = Graph2dHybridWindowResultSetting::getGridLocation(att);
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

void Graph2dHybridWindowResultSetting::DataTypeInfo::saveToProjectMainFile(QXmlStreamWriter& writer) const
{
	iRIC::setIntAttribute(writer, "dataType", static_cast<int>(dataType));
	iRIC::setIntAttribute(writer, "dimension", static_cast<int>(dimension));
	iRIC::setIntAttribute(writer, "zoneId", zoneId);
	writer.writeAttribute("zoneName", zoneName.c_str());
	writer.writeAttribute("gridLocation", Graph2dHybridWindowResultSetting::getGridLocationString(gridLocation));

	for (auto loc : dataNamesMap.keys()) {
		writer.writeStartElement("DataNames");
		writer.writeAttribute("gridLocation", Graph2dHybridWindowResultSetting::getGridLocationString(loc));
		for (auto name : dataNamesMap[loc]) {
			writer.writeStartElement("DataName");
			writer.writeAttribute("name", name);
			writer.writeEndElement();
		}
		writer.writeEndElement();
	}
}

void Graph2dHybridWindowResultSetting::Setting::loadFromProjectMainFile(const QDomNode& node)
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

void Graph2dHybridWindowResultSetting::Setting::saveToProjectMainFile(QXmlStreamWriter& writer) const
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
