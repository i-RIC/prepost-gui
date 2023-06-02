#define _USE_MATH_DEFINES
#include <math.h>

#include "structured2dgrid.h"
#include "../../project/projectcgnsfile.h"
#include "../gridcond/base/gridattributecontainer.h"

#include <guibase/vtktool/vtkpointsetregionandcellsizefilter.h>
#include <guibase/graphicsmisc.h>
#include <misc/mathsupport.h>
#include <misc/rectregion.h>
#include <misc/stringtool.h>

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkGeometryFilter.h>
#include <vtkMaskPoints.h>
#include <vtkExtractGrid.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>

#include <QPointF>
#include <QTextStream>

#include <h5cgnsbase.h>
#include <h5cgnsgridcoordinates.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

#include <vector>

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

int Structured2DGrid::loadFromCgnsFile(const iRICLib::H5CgnsZone& zone)
{
	auto size = zone.size();

	m_dimensionI = size.at(0);
	m_dimensionJ = size.at(1);

	auto grid = vtkGrid();
	grid->SetDimensions(m_dimensionI, m_dimensionJ, 1);

	std::vector<double> xvec, yvec;
	int ier = 0;

	ier = zone.gridCoordinates()->readCoordinatesX(&xvec);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = zone.gridCoordinates()->readCoordinatesY(&yvec);
	if (ier != IRIC_NO_ERROR) {return ier;}

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto offset = this->offset();
	for (unsigned int j = 0; j < m_dimensionJ; ++j) {
		for (unsigned int i = 0; i < m_dimensionI; ++i) {
			points->InsertNextPoint(xvec[m_dimensionI * j + i] - offset.x(), yvec[m_dimensionI * j + i] - offset.y(), 0);
		}
	}
	grid->SetPoints(points);

	ier = loadGridAttributes(*(zone.gridAttributes()));
	if (ier != IRIC_NO_ERROR) {return ier;}

	return IRIC_NO_ERROR;
}

int Structured2DGrid::saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName)
{
	int ier;
	std::vector<int> sizes;
	sizes.push_back(m_dimensionI);
	sizes.push_back(m_dimensionJ);
	sizes.push_back(m_dimensionI - 1);
	sizes.push_back(m_dimensionJ - 1);

	auto zone = base->createZone(zoneName, iRICLib::H5CgnsZone::Type::Structured, sizes);

	// save coordinates.
	std::vector<double> dataX(vtkGrid()->GetNumberOfPoints(), 0);
	std::vector<double> dataY(vtkGrid()->GetNumberOfPoints(), 0);
	auto offset = this->offset();
	double points[3];

	for (int i = 0; i < vtkGrid()->GetNumberOfPoints(); ++i) {
		vtkGrid()->GetPoints()->GetPoint(i, points);
		dataX[i] = points[0] + offset.x();
		dataY[i] = points[1] + offset.y();
	}
	ier = zone->gridCoordinates()->writeCoordinatesX(dataX);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = zone->gridCoordinates()->writeCoordinatesY(dataY);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = saveGridAttributes(zone->gridAttributes());
	if (ier != IRIC_NO_ERROR) {return ier;}

	return IRIC_NO_ERROR;
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

void Structured2DGrid::setDimensionsFromVtkGrid()
{
	auto grid = dynamic_cast<vtkStructuredGrid*>(vtkGrid());
	int dims[3];
	grid->GetDimensions(dims);
	m_dimensionI = dims[0];
	m_dimensionJ = dims[1];
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

	bool outOfRegion;
	int iMin, iMax, jMin, jMax;
	int rate;

	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);

	RectRegion region(xmin, xmax, ymin, ymax);

	int maxcell = -1;
	if (cullEnable) {
		maxcell = cullCellLimit;
	}
	vtkPointSetRegionAndCellSizeFilter::calcExtractParameters(vtkGrid(), region, pointLocator(), maxcell, &outOfRegion, &iMin, &iMax, &jMin, &jMax, &rate);

	if (outOfRegion) {
		auto emptyPolyData = vtkSmartPointer<vtkPolyData>::New();
		setFilteredGrid(emptyPolyData);
		setFilteredIndexGrid(emptyPolyData);

		m_drawnIMin = -1;
		m_drawnIMax = -2;
		m_drawnJMin = -1;
		m_drawnJMax = -2;

		return;
	}

	auto filteredGrid = vtkPointSetRegionAndCellSizeFilter::extract(vtkGrid(), iMin, iMax, jMin, jMax, rate);
	setFilteredGrid(filteredGrid);
	filteredGrid->Delete();

	m_drawnIMin = iMin;
	m_drawnIMax = iMax;
	m_drawnJMin = jMin;
	m_drawnJMax = jMax;

	setMasked(rate > 1);

	int tmpIMin = (m_drawnIMin / rate) * rate;
	int tmpIMax = std::min((m_drawnIMax / rate + 1) * rate, m_dimensionI - 1);
	int tmpJMin = (m_drawnJMin / rate) * rate;
	int tmpJMax = std::min((m_drawnJMax / rate + 1) * rate, m_dimensionJ - 1);

	auto filteredIndexGrid = vtkSmartPointer<vtkPolyData>::New();
	auto igPoints = vtkSmartPointer<vtkPoints>::New();
	auto ca = vtkSmartPointer<vtkCellArray>::New();
	auto sa = vtkSmartPointer<vtkStringArray>::New();
	sa->SetName(Grid::LABEL_NAME);

	vtkIdType cellid = 0;
	double tmpp[3];
	QString label("(%1,%2)");
	for (int i = tmpIMin; i <= tmpIMax; i += rate) {
		vtkGrid()->GetPoint(vertexIndex(i, 0), tmpp);
		igPoints->InsertNextPoint(tmpp);
		ca->InsertNextCell(1, &cellid);
		sa->InsertNextValue(iRIC::toStr(label.arg(i + 1).arg(1)));
		++ cellid;
	}
	for (int j = tmpJMin; j <= tmpJMax; j += rate) {
		vtkGrid()->GetPoint(vertexIndex(0, j), tmpp);
		igPoints->InsertNextPoint(tmpp);
		ca->InsertNextCell(1, &cellid);
		sa->InsertNextValue(iRIC::toStr(label.arg(1).arg(j + 1)));
		++ cellid;
	}

	filteredIndexGrid->SetPoints(igPoints);
	filteredIndexGrid->SetVerts(ca);
	filteredIndexGrid->GetPointData()->AddArray(sa);

	setFilteredIndexGrid(filteredIndexGrid);
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
