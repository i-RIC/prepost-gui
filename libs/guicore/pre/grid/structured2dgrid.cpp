#define _USE_MATH_DEFINES
#include <cmath>

#include "structured2dgrid.h"
#include "../../project/projectcgnsfile.h"
#include "../gridcond/base/gridattributecontainer.h"
#include <misc/stringtool.h>
#include <misc/mathsupport.h>
#include <guibase/graphicsmisc.h>

#include <cgnslib.h>
#include <iriclib.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkGeometryFilter.h>
#include <vtkMaskPoints.h>
#include <vtkExtractGrid.h>
#include <vtkCellArray.h>
#include <vtkStringArray.h>
#include <vtkPointData.h>
#include <vtkTrivialProducer.h>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QLineF>
#include <QMessageBox>
#include <QPen>
#include <QPointF>
#include <QSettings>
#include <QTextStream>

#include <vector>

bool RectRegion::intersect(const QLineF& line) const
{
	bool isInside1 = pointIsInside(line.x1(), line.y1());
	bool isInside2 = pointIsInside(line.x2(), line.y2());
	return (isInside1 != isInside2);
}

Structured2DGrid::Structured2DGrid(ProjectDataItem* parent) :
	Structured2DGrid("", parent)
{}

Structured2DGrid::Structured2DGrid(const std::string& zonename, ProjectDataItem* parent) :
	Grid2D {vtkStructuredGrid::New(), zonename, SolverDefinitionGridType::gtStructured2DGrid, parent},
	m_dimensionI {0},
	m_dimensionJ {0}
{}

Structured2DGrid::~Structured2DGrid()
{}

vtkStructuredGrid* Structured2DGrid::vtkGrid() const
{
	return dynamic_cast<vtkStructuredGrid*>(Grid::vtkGrid());
}

unsigned int Structured2DGrid::vertexCount() const
{
	return m_dimensionI * m_dimensionJ;
}

unsigned int Structured2DGrid::cellCount() const
{
	return vtkGrid()->GetNumberOfCells();
}

unsigned int Structured2DGrid::vertexIndex(unsigned int i, unsigned int j) const
{
	return m_dimensionI * j + i;
}

void Structured2DGrid::getIJIndex(unsigned int index, unsigned int* i, unsigned int* j)
{
	*i = index % m_dimensionI;
	*j = index / m_dimensionI;
}

unsigned int Structured2DGrid::cellIndex(unsigned int i, unsigned int j) const
{
	return (m_dimensionI - 1) * j + i;
}

void Structured2DGrid::getCellIJIndex(unsigned int index, unsigned int* i, unsigned int* j)
{
	*i = index % (m_dimensionI - 1);
	*j = index / (m_dimensionI - 1);
}

QPointF Structured2DGrid::vertex(unsigned int index) const
{
	double v[3];
	vtkGrid()->GetPoints()->GetPoint(index, v);
	return QPointF(v[0], v[1]);
}

QPointF Structured2DGrid::vertex(unsigned int i, unsigned int j) const
{
	return vertex(vertexIndex(i, j));
}

void Structured2DGrid::setVertex(unsigned int i, unsigned int j, const QPointF& v)
{
	setVertex(vertexIndex(i, j), v);
}

void Structured2DGrid::setVertex(unsigned int index, const QPointF& v)
{
	vtkGrid()->GetPoints()->SetPoint(index, v.x(), v.y(), 0);
}

bool Structured2DGrid::loadFromCgnsFile(const int fn, int base, int zoneid)
{
	m_dimensionI = 0;
	m_dimensionJ = 0;

	int ier;
	cgsize_t size[9];
	char buffer[ProjectCgnsFile::BUFFERLEN];
	ier = cg_zone_read(fn, base, zoneid, buffer, size);
	if (ier != 0) {return false;}
	// for structured 2d grid, size[0] = NVertexI, size[1] = NVertexJ.
	m_dimensionI = size[0];
	m_dimensionJ = size[1];

	// prepare memory area.
	vtkStructuredGrid* grid = vtkGrid();
	grid->SetDimensions(m_dimensionI, m_dimensionJ, 1);
	ier = cg_goto(fn, base, "Zone_t", zoneid, "GridCoordinates", 0, "end");
	if (ier != 0) {
		// grid data does not exists.
		return false;
	}
	// the first one must be X.
	DataType_t dType;
	int narrays;
	int dim;
	cgsize_t dimV[3];
	cg_narrays(&narrays);
	ier = cg_array_info(1, buffer, &dType, &dim, dimV);
	std::vector<double> dataX(m_dimensionI * m_dimensionJ, 0);
	std::vector<double> dataY(m_dimensionI * m_dimensionJ, 0);

	cg_array_read_as(1, RealDouble, dataX.data());
	cg_array_read_as(2, RealDouble, dataY.data());

	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	auto offset = this->offset();
	for (unsigned int j = 0; j < m_dimensionJ; ++j) {
		for (unsigned int i = 0; i < m_dimensionI; ++i) {
			points->InsertNextPoint(dataX[m_dimensionI * j + i] - offset.x(), dataY[m_dimensionI * j + i] - offset.y(), 0);
		}
	}
	grid->SetPoints(points);
	points->Delete();

	// Grid coordinates are loaded.
	// Next, grid related condition data is loaded.
	loadGridAttributes(fn, base, zoneid);
	return true;
}

bool Structured2DGrid::saveToCgnsFile(const int fn, int B, const char* zonename)
{
	int ier;
	cgsize_t sizes[9];
	// Check whether the corresponsing zone already exists.
	int zoneid = zoneId(zonename, fn, B, sizes);
	if (zoneid != 0) {
		// Zone already exists! remove this zone first!
		ier = cg_delete_node(const_cast<char*>(iRIC::toStr(zonename).c_str()));
		if (ier != 0) {return false;}
	}
	// Now, create new zone.
	sizes[0] = m_dimensionI;
	sizes[1] = m_dimensionJ;
	sizes[2] = m_dimensionI - 1;
	sizes[3] = m_dimensionJ - 1;
	sizes[4] = 0;
	sizes[5] = 0;
	ier = cg_zone_write(fn, B, iRIC::toStr(zonename).c_str(), sizes, Structured, &zoneid);
	if (ier != 0) {return false;}

	// save grid coordinates.
	int G;
	ier = cg_grid_write(fn, B, zoneid, "GridCoordinates", &G);
	if (ier != 0) {return false;}
	int C;
	// save coordinates.
	std::vector<double> dataX(m_dimensionI * m_dimensionJ, 0);
	std::vector<double> dataY(m_dimensionI * m_dimensionJ, 0);
	auto offset = this->offset();
	double points[3];

	for (unsigned int i = 0; i < m_dimensionI; ++i) {
		for (unsigned int j = 0; j < m_dimensionJ; ++j) {
			vtkGrid()->GetPoints()->GetPoint(i + m_dimensionI * j, points);
			dataX[i + m_dimensionI * j] = points[0] + offset.x();
			dataY[i + m_dimensionI * j] = points[1] + offset.y();
		}
	}
	ier = cg_coord_write(fn, B, zoneid, RealDouble, "CoordinateX", dataX.data(), &C);
	if (ier != 0) {return false;}
	ier = cg_coord_write(fn, B, zoneid, RealDouble, "CoordinateY", dataY.data(), &C);
	if (ier != 0) {return false;}

	// Grid coordinates are saved.
	// Next grid related condition data is saved.
	// Create "GridConditions" node under the zone node.
	saveGridAttributes(fn, B, zoneid);
	return true;
}

void Structured2DGrid::dimensions(unsigned int* i, unsigned int* j)
{
	*i = m_dimensionI;
	*j = m_dimensionJ;
}

unsigned int Structured2DGrid::dimensionI() const
{
	return m_dimensionI;
}

unsigned int Structured2DGrid::dimensionJ() const
{
	return m_dimensionJ;
}

void Structured2DGrid::setDimensions(unsigned int i, unsigned int j)
{
	m_dimensionI = i;
	m_dimensionJ = j;

	// prepare memory area.
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(vtkGrid());
	grid->SetDimensions(m_dimensionI, m_dimensionJ, 1);
}

const QStringList Structured2DGrid::checkShape(QTextStream& stream)
{
	QStringList messages;
	QSettings settings;
	settings.beginGroup("gridcheck");
	if (settings.value("checkcrushed", true).value<bool>()) {
		// check whether it is crushed
		if (! isValid(stream)) {
			messages.append(tr("An edge line intersects with that of another cell in this grid."));
		}
	}
	if (settings.value("checkaspectratio", false).value<bool>()) {
		double limit = settings.value("aspectratiolimit", 1.5).value<double>();
		if (! isAspectRatioOk(limit, stream)) {
			messages.append(tr("Aspect ratio of grid cell exceed limit value %1 in more than one cell.").arg(limit));
		}
	}
	if (settings.value("checkvariation", false).value<bool>()) {
		double ilimit = settings.value("variationilimit", 1.5).value<double>();
		double jlimit = settings.value("variationjlimit", 1.5).value<double>();
		if (! isVariationOk(ilimit, jlimit, stream)) {
			messages.append(tr("Variation of grid cell edge lengths exceed limit values (I-direction: %1, J-direction: %2) in more than one cell.").arg(ilimit).arg(jlimit));
		}
	}
	if (settings.value("checkangle", false).value<bool>()) {
		double limit = settings.value("anglelimit", 40).value<double>();
		if (! isAngleOk(limit, stream)) {
			messages.append(tr("Angle of grid cell edge fall below limit value %1 in more than one cell.").arg(limit));
		}
	}
	return messages;
}

bool Structured2DGrid::isValid(QTextStream& stream) const
{
	bool allOk = true;
	stream << tr("* Cells where edges intersect") << endl;
	for (unsigned int j = 0; j < m_dimensionJ - 1; ++j) {
		for (unsigned int i = 0; i < m_dimensionI - 1; ++i) {
			QPointF v0 = vertex(i, j);
			QPointF v1 = vertex(i + 1, j);
			QPointF v2 = vertex(i + 1, j + 1);
			QPointF v3 = vertex(i, j + 1);
			double tmpv1, tmpv2;
			tmpv1 = iRIC::outerProduct(v2 - v1, v0 - v1);
			tmpv2 = iRIC::outerProduct(v0 - v3, v2 - v3);

			if (tmpv1 * tmpv2 < 0.0) {
				if (allOk) {
					stream << "  I, J" << endl;
				}
				stream << "  " << i + 1 << ", " << j + 1 << endl;
				allOk = false;
			}
		}
	}
	if (allOk) {
		stream << "  " << tr("Not exist") << endl;
	}
	stream << endl;
	return allOk;
}

bool Structured2DGrid::isAspectRatioOk(double limit, QTextStream& stream)
{
	double lengths[4];
	QPointF v[4];
	bool allOk = true;
	stream << tr("* Cells where aspect ratio of cell edges exceed %1").arg(limit) << endl;
	for (unsigned int j = 0; j < m_dimensionJ - 1; ++j) {
		for (unsigned int i = 0; i < m_dimensionI - 1; ++i) {
			v[0] = vertex(i, j);
			v[1] = vertex(i + 1, j);
			v[2] = vertex(i + 1, j + 1);
			v[3] = vertex(i, j + 1);

			for (int k = 0; k < 4; ++k) {
				lengths[k] = iRIC::length(v[(k + 1) % 4] - v[k]);
			}
			for (int k = 0; k < 4; ++k) {
				// check whether the length is too small.
				if (lengths[k] < 1.0E-12) {
					if (allOk) {
						stream << "  " << tr("I, J, Aspect ratio") << endl;
					}
					stream << "  " << i + 1 << ", " << j + 1 << ", " << tr("Edge length too small");
					allOk = false;
					break;
				}
				double ratio = lengths[(k + 1) % 4] / lengths[k];
				if (ratio > limit || ratio < 1. / limit) {
					if (ratio < 1. / limit) {ratio = 1 / ratio;}
					if (allOk) {
						stream << "  " << tr("I, J, Aspect ratio") << endl;
					}
					stream << "  " << i + 1 << ", " << j + 1 << ", " << ratio << endl;
					allOk = false;
					break;
				}
			}
		}
	}
	if (allOk) {
		stream << "  " << tr("Not exist") << endl;
	}
	stream << endl;
	return allOk;
}

bool Structured2DGrid::isAngleOk(double limitAngle, QTextStream& stream)
{
	bool allOk = true;
	stream << tr("* Cells where angle of cell vertex is below %1").arg(limitAngle) << endl;
	QPointF v[4];
	for (unsigned int j = 0; j < m_dimensionJ - 1; ++j) {
		for (unsigned int i = 0; i < m_dimensionI - 1; ++i) {
			v[0] = vertex(i, j);
			v[1] = vertex(i + 1, j);
			v[2] = vertex(i + 1, j + 1);
			v[3] = vertex(i, j + 1);

			for (int k = 0; k < 4; ++k) {
				QPointF vec0 = v[(k + 1) % 4] - v[k];
				QPointF vec1 = v[(k + 3) % 4] - v[k];

				double dotProd = QPointF::dotProduct(vec0, vec1);
				double cosVal = dotProd / (iRIC::length(vec0) * iRIC::length(vec1));
				double angle = acos(cosVal) / M_PI * 180.;
				if (angle < limitAngle) {
					if (allOk) {
						stream << "  " << tr("I, J, Angle") << endl;
					}
					stream << "  " << i + 1 << ", " << j + 1 << ", " << angle << endl;
					allOk = false;
				}
			}
		}
	}
	if (allOk) {
		stream << "  " << tr("Not exist") << endl;
	}
	stream << endl;
	return allOk;
}

bool Structured2DGrid::isVariationOk(double ilimit, double jlimit, QTextStream& stream)
{
	bool iAllOk = true;
	stream << tr("* Cells where variation of cell length along i-direction exceed %1").arg(ilimit) << endl;
	double lengths[2];
	QPointF v[3];
	for (unsigned int j = 0; j < m_dimensionJ; ++j) {
		for (unsigned int i = 0; i < m_dimensionI - 2; ++i) {
			v[0] = vertex(i, j);
			v[1] = vertex(i + 1, j);
			v[2] = vertex(i + 2, j);

			for (int k = 0; k < 2; ++k) {
				lengths[k] = iRIC::length(v[k + 1] - v[k]);
			}
			double variation = lengths[1] / lengths[0];
			if (variation < 1) {variation = 1. / variation;}
			if (variation > ilimit) {
				if (iAllOk) {
					stream << "  " << tr("I, J, Variation") << endl;
				}
				unsigned int tmpj = j;
				if (tmpj == m_dimensionJ - 1) {tmpj = m_dimensionJ - 2;}
				stream << "  " << i + 2 << ", " << tmpj + 1 << ", " << variation << endl;
				iAllOk = false;
			}
		}
	}
	if (iAllOk) {
		stream << "  " << tr("Not exist") << endl;
	}
	stream << endl;

	bool jAllOk = true;
	stream << tr("* Cells where variation of cell length along j-direction exceed %1").arg(jlimit) << endl;
	for (unsigned int i = 0; i < m_dimensionI; ++i) {
		for (unsigned int j = 0; j < m_dimensionJ - 2; ++j) {
			v[0] = vertex(i, j);
			v[1] = vertex(i, j + 1);
			v[2] = vertex(i, j + 2);

			for (int k = 0; k < 2; ++k) {
				lengths[k] = iRIC::length(v[k + 1] - v[k]);
			}
			double variation = lengths[1] / lengths[0];
			if (variation < 1) {variation = 1. / variation;}
			if (variation > jlimit) {
				if (jAllOk) {
					stream << "  " << tr("I, J, Variation") << endl;
				}
				unsigned int tmpi = i;
				if (tmpi == m_dimensionI - 1) {tmpi = m_dimensionI - 2;}
				stream << "  " << tmpi + 1 << ", " << j + 2 << ", " << variation << endl;
				jAllOk = false;
			}
		}
	}
	if (jAllOk) {
		stream << "  " << tr("Not exist") << endl;
	}
	stream << endl;
	return iAllOk && jAllOk;
}

void Structured2DGrid::updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax)
{
	setMasked(false);

	double xcenter = (xmin + xmax) * 0.5;
	double ycenter = (ymin + ymax) * 0.5;

	double xwidth = (xmax - xmin);
	double ywidth = (ymax - ymin);

	xmin -= xwidth * 0.2;
	xmax += xwidth * 0.2;
	ymin -= ywidth * 0.2;
	ymax += ywidth * 0.2;

	bool outOfRegion = false;

	// 1. Find the grid vertex that is the nearest to the region center.
	vtkIdType vid = vtkGrid()->FindPoint(xcenter, ycenter, 0);

	if (vid == -1) {outOfRegion = true;}
	if (! outOfRegion) {
		double* cv = vtkGrid()->GetPoint(vid);
		if (*cv < xmin || *cv > xmax || *(cv + 1) < ymin || *(cv + 1) > ymax) {
			outOfRegion = true;
		}
	}
	if (outOfRegion) {
		// 2. If the point is out of the region, the whole grid is out of the region.
		vtkSmartPointer<vtkTrivialProducer> emptyAlgo = vtkSmartPointer<vtkTrivialProducer>::New();
		vtkSmartPointer<vtkPolyData> emptyPoly = vtkSmartPointer<vtkPolyData>::New();
		emptyPoly->SetPoints(vtkGrid()->GetPoints());
		emptyAlgo->SetOutput(emptyPoly);

		setFilteredShapeAlgorithm(emptyAlgo);
		setFilteredPointsAlgorithm(emptyAlgo);
		setFilteredCellsAlgorithm(emptyAlgo);
		return;
	}

	RectRegion region(xmin, xmax, ymin, ymax);

	unsigned int centerI, centerJ;
	getIJIndex(vid, &centerI, &centerJ);
	unsigned int lineLimitIMin, lineLimitIMax, lineLimitJMin, lineLimitJMax;
	double tmpv[3];

	// test I = 0
	vtkGrid()->GetPoint(vertexIndex(0, centerJ), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitIMin = 0;
	} else {
		lineLimitIMin = lineLimitI(centerJ, centerI, 0, region);
	}
	// test I = imax
	vtkGrid()->GetPoint(vertexIndex(m_dimensionI - 1, centerJ), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitIMax = m_dimensionI - 1;
	} else {
		lineLimitIMax = lineLimitI(centerJ, centerI, m_dimensionI - 1, region);
	}
	// test J = 0
	vtkGrid()->GetPoint(vertexIndex(centerI, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitJMin = 0;
	} else {
		lineLimitJMin = lineLimitJ(centerI, centerJ, 0, region);
	}
	// test J = jmax
	vtkGrid()->GetPoint(vertexIndex(centerI, m_dimensionJ - 1), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitJMax = m_dimensionJ - 1;
	} else {
		lineLimitJMax = lineLimitJ(centerI, centerJ, m_dimensionJ - 1, region);
	}

	int lineLimitIMin2, lineLimitIMax2, lineLimitJMin2, lineLimitJMax2;

	// test I min direction
	if (lineLimitIMin == 0) {
		lineLimitIMin2 = 0;
	} else {
		if (lineAtIIntersect(0, region)) {
			lineLimitIMin2 = 0;
		} else {
			lineLimitIMin2 = lineLimitI2(lineLimitIMin, 0, region);
		}
	}
	// test I max direction
	if (lineLimitIMax == m_dimensionI - 1) {
		lineLimitIMax2 = m_dimensionI - 1;
	} else {
		if (lineAtIIntersect(m_dimensionI - 1, region)) {
			lineLimitIMax2 = m_dimensionI - 1;
		} else {
			lineLimitIMax2 = lineLimitI2(lineLimitIMax, m_dimensionI - 1, region);
		}
	}

	// test J min direction
	if (lineLimitJMin == 0) {
		lineLimitJMin2 = 0;
	} else {
		if (lineAtJIntersect(0, region)) {
			lineLimitJMin2 = 0;
		} else {
			lineLimitJMin2 = lineLimitJ2(lineLimitJMin, 0, region);
		}
	}
	// test J max direction
	if (lineLimitJMax == m_dimensionJ - 1) {
		lineLimitJMax2 = m_dimensionJ - 1;
	} else {
		if (lineAtJIntersect(m_dimensionJ - 1, region)) {
			lineLimitJMax2 = m_dimensionJ - 1;
		} else {
			lineLimitJMax2 = lineLimitI2(lineLimitJMax, m_dimensionJ - 1, region);
		}
	}

	vtkSmartPointer<vtkExtractGrid> exGrid = vtkSmartPointer<vtkExtractGrid>::New();
	exGrid->SetVOI(lineLimitIMin2, lineLimitIMax2, lineLimitJMin2, lineLimitJMax2, 0, 0);
	exGrid->SetInputData(vtkGrid());
	exGrid->Update();
	vtkSmartPointer<vtkStructuredGrid> extractedGrid = exGrid->GetOutput();
	int exRate = 1;
	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);
	while (cullEnable && extractedGrid->GetNumberOfCells() > cullCellLimit){
		exRate *= 2;
		exGrid->SetSampleRate(exRate, exRate, 1);
		exGrid->Update();
		extractedGrid = exGrid->GetOutput();

		setMasked(true);
	}

	vtkSmartPointer<vtkGeometryFilter> geo = vtkSmartPointer<vtkGeometryFilter>::New();
	geo->SetInputConnection(exGrid->GetOutputPort());

	m_drawnIMin = lineLimitIMin2;
	m_drawnIMax = lineLimitIMax2;
	m_drawnJMin = lineLimitJMin2;
	m_drawnJMax = lineLimitJMax2;

	setFilteredShapeAlgorithm(geo);
	setFilteredPointsAlgorithm(geo);
	setFilteredCellsAlgorithm(geo);

	int tmpIMin = (m_drawnIMin / exRate) * exRate;
	int tmpIMax = qMin((m_drawnIMax / exRate + 1) * exRate, m_dimensionI - 1);
	int tmpJMin = (m_drawnJMin / exRate) * exRate;
	int tmpJMax = qMin((m_drawnJMax / exRate + 1) * exRate, m_dimensionJ - 1);

	vtkSmartPointer<vtkPolyData> filteredIndexGrid = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> igPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkStringArray> sa = vtkSmartPointer<vtkStringArray>::New();
	sa->SetName(Grid::LABEL_NAME);
	vtkIdType cellid = 0;
	double tmpp[3];
	QString label("(%1,%2)");
	for (int i = tmpIMin; i <= tmpIMax; i += exRate) {
		vtkGrid()->GetPoint(vertexIndex(i, 0), tmpp);
		igPoints->InsertNextPoint(tmpp);
		ca->InsertNextCell(1, &cellid);
		sa->InsertNextValue(iRIC::toStr(label.arg(i + 1).arg(1)));
		++ cellid;
	}
	for (int j = tmpJMin; j <= tmpJMax; j += exRate) {
		vtkGrid()->GetPoint(vertexIndex(0, j), tmpp);
		igPoints->InsertNextPoint(tmpp);
		ca->InsertNextCell(1, &cellid);
		sa->InsertNextValue(iRIC::toStr(label.arg(1).arg(j + 1)));
		++ cellid;
	}
	filteredIndexGrid->SetPoints(igPoints);
	filteredIndexGrid->SetVerts(ca);
	filteredIndexGrid->GetPointData()->AddArray(sa);


	vtkSmartPointer<vtkTrivialProducer> prod = vtkSmartPointer<vtkTrivialProducer>::New();
	prod->SetOutput(filteredIndexGrid);
	m_vtkFilteredIndexGridAlgorithm = prod;
}

int Structured2DGrid::drawnIMin() const
{
	return m_drawnIMin;
}

int Structured2DGrid::drawnIMax() const
{
	return m_drawnIMax;
}

int Structured2DGrid::drawnJMin() const
{
	return m_drawnJMin;
}

int Structured2DGrid::drawnJMax() const
{
	return m_drawnJMax;
}

vtkAlgorithm* Structured2DGrid::vtkFilteredIndexGridAlgorithm() const
{
	return m_vtkFilteredIndexGridAlgorithm;
}

int Structured2DGrid::lineLimitI(int j, int iIn, int iOut, const RectRegion& region)
{
	if (qAbs(iOut - iIn) == 1) {
		return iIn;
	}
	int i = (iIn + iOut) / 2;
	double tmpv[3];
	vtkGrid()->GetPoint(vertexIndex(i, j), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		return lineLimitI(j, i, iOut, region);
	} else {
		return lineLimitI(j, iIn, i, region);
	}
}

int Structured2DGrid::lineLimitJ(int i, int jIn, int jOut, const RectRegion& region)
{
	if (qAbs(jOut - jIn) == 1) {
		return jIn;
	}
	int j = (jIn + jOut) / 2;
	double tmpv[3];
	vtkGrid()->GetPoint(vertexIndex(i, j), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		return lineLimitJ(i, j, jOut, region);
	} else {
		return lineLimitJ(i, jIn, j, region);
	}
}

int Structured2DGrid::lineLimitI2(int iIn, int iOut, const RectRegion& region)
{
	if (qAbs(iOut - iIn) == 1) {
		return iIn;
	}
	int i = (iIn + iOut) / 2;
	if (lineAtIIntersect(i, region)) {
		return lineLimitI2(i, iOut, region);
	} else {
		return lineLimitI2(iIn, i, region);
	}
}

int Structured2DGrid::lineLimitJ2(int jIn, int jOut, const RectRegion& region)
{
	if (qAbs(jOut - jIn) == 1) {
		return jIn;
	}
	int j = (jIn + jOut) / 2;
	if (lineAtJIntersect(j, region)) {
		return lineLimitJ2(j, jOut, region);
	} else {
		return lineLimitJ2(jIn, j, region);
	}
}

bool Structured2DGrid::lineAtIIntersect(int i, const RectRegion& region)
{
	QPointF p1, p2;
	double tmpv[3];
	vtkGrid()->GetPoint(vertexIndex(i, 0), tmpv);
	p1 = QPointF(tmpv[0], tmpv[1]);
	for (unsigned int j = 1; j < m_dimensionJ; ++j) {
		vtkGrid()->GetPoint(vertexIndex(i, j), tmpv);
		p2 = QPointF(tmpv[0], tmpv[1]);
		QLineF line(p1, p2);
		if (region.intersect(line)) {return true;}
	}
	return false;
}

bool Structured2DGrid::lineAtJIntersect(int j, const RectRegion& region)
{
	QPointF p1, p2;
	double tmpv[3];
	vtkGrid()->GetPoint(vertexIndex(0, j), tmpv);
	p1 = QPointF(tmpv[0], tmpv[1]);
	for (unsigned int i = 1; i < m_dimensionI; ++i) {
		vtkGrid()->GetPoint(vertexIndex(i, j), tmpv);
		p2 = QPointF(tmpv[0], tmpv[1]);
		QLineF line(p1, p2);
		if (region.intersect(line)) {return true;}
	}
	return false;
}
