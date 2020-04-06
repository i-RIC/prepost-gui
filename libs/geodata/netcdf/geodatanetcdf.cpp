#include "geodatanetcdf.h"

#include <cs/coordinatesystem.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/gridcond/base/gridattributedimensionintegercontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionrealcontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridattributeinteger.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegerdimension.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <guicore/solverdef/solverdefinitiongridattributerealdimension.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkExtractGrid.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>

#include <QDir>
#include <QLineF>
#include <QMessageBox>

#include <netcdf.h>
#include <vector>

#define VALUE "value"
#define X "x"
#define Y "y"

#define LON "lon"
#define LAT "lat"

const int GeoDataNetcdf::MAX_DRAWCELLCOUNT = 200000;

namespace {

	int calcIndex(int i, int j, int isize)
	{
		return i + j * isize;
	}

} // namespace

bool GeoDataNetcdf::RectRegion::pointIsInside(double x, double y) const
{
	if (x < xMin) {return false;}
	if (x > xMax) {return false;}
	if (y < yMin) {return false;}
	if (y > yMax) {return false;}
	return true;
}

bool GeoDataNetcdf::RectRegion::intersect(const QLineF& line) const
{
	bool isInside1 = pointIsInside(line.x1(), line.y1());
	bool isInside2 = pointIsInside(line.x2(), line.y2());
	return (isInside1 != isInside2);
}

GeoDataNetcdf::GeoDataNetcdf(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoData {d, creator, att},
	m_isMasked {false}
{
	m_opacityPercent = 50;

	m_grid = vtkSmartPointer<vtkStructuredGrid>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	m_grid->SetPoints(points);
}

GeoDataNetcdf::~GeoDataNetcdf()
{
	actorCollection()->RemoveItem(m_regionActor);
	actorCollection()->RemoveItem(m_actor);
	renderer()->RemoveActor(m_regionActor);
	renderer()->RemoveActor(m_actor);
}

void GeoDataNetcdf::setupActors()
{
	m_threshold = vtkSmartPointer<vtkThreshold>::New();
	m_threshold->SetInputData(m_grid);
	double tVal = thresholdValue();
	if (tVal < 0) {
		m_threshold->ThresholdByUpper(tVal);
	} else {
		m_threshold->ThresholdByLower(tVal);
	}
	m_threshold->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "values");
	m_mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_mapper->SetInputConnection(m_threshold->GetOutputPort());
	m_mapper->SetLookupTable(scalarsToColorsContainer()->vtkObj());
	m_mapper->SetUseLookupTableScalarRange(true);
	m_mapper->SetScalarVisibility(true);
	m_mapper->SetScalarModeToUseCellData();
	m_mapper->ImmediateModeRenderingOn();

	m_actor = vtkSmartPointer<vtkActor>::New();
	m_actor->SetMapper(m_mapper);
	vtkProperty* p = m_actor->GetProperty();
	p->SetLighting(false);

	actorCollection()->AddItem(m_actor);
	renderer()->AddActor(m_actor);

	m_regionPolyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> tmppoints = vtkSmartPointer<vtkPoints>::New();
	m_regionPolyData->SetPoints(tmppoints);

	m_regionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_regionMapper->SetInputData(m_regionPolyData);

	m_regionActor = vtkSmartPointer<vtkActor>::New();
	m_regionActor->SetMapper(m_regionMapper);
	p = m_regionActor->GetProperty();
	p->SetOpacity(0);
	p->SetColor(0, 0, 0);
	m_regionActor->VisibilityOff();

	renderer()->AddActor(m_regionActor);
}

void GeoDataNetcdf::loadExternalData(const QString& filename)
{
	std::string fname = iRIC::toStr(filename);
	int ncid, ret;
	ret = nc_open(fname.c_str(), NC_NOWRITE, &ncid);
	if (ret != NC_NOERR) {
		// error occured while opening the file.
		QMessageBox::critical(iricMainWindow(), tr("Error"), tr("Error occured while opening %1.").arg(QDir::toNativeSeparators(filename)));
		return;
	}

	// ------------------------
	// Load coordinates values
	// ------------------------

	int xVarId, yVarId, lonVarId, latVarId;

	// try to find x, y
	bool xyexist = true;
	ret = nc_inq_varid(ncid, X, &xVarId);
	xyexist = xyexist && (ret == NC_NOERR);
	ret = nc_inq_varid(ncid, Y, &yVarId);
	xyexist = xyexist && (ret == NC_NOERR);

	if (xyexist) {
		// the coordinate system type is XY.
		m_coordinateSystemType = XY;

		// load X, Y values
		int xDimId, yDimId;
		ret = nc_inq_dimid(ncid, X, &xDimId);
		ret = nc_inq_dimid(ncid, Y, &yDimId);

		size_t xSize, ySize, xySize;
		ret = nc_inq_dimlen(ncid, xDimId, &xSize);
		ret = nc_inq_dimlen(ncid, yDimId, &ySize);

		std::vector<double> xs(xSize);
		std::vector<double> ys(ySize);

		ret = nc_get_var_double(ncid, xVarId, xs.data());
		ret = nc_get_var_double(ncid, yVarId, ys.data());

		m_xValues.clear();
		m_xValues.reserve(static_cast<int>(xSize));
		for (size_t i = 0; i < xSize; ++i) {
			m_xValues.push_back(xs[i]);
		}
		m_yValues.clear();
		m_yValues.reserve(static_cast<int>(ySize));
		for (size_t i = 0; i < ySize; ++i) {
			m_yValues.push_back(ys[i]);
		}

		// load Lon Lat Values
		ret = nc_inq_varid(ncid, LAT, &latVarId);
		ret = nc_inq_varid(ncid, LON, &lonVarId);
		xySize = xSize * ySize;

		std::vector<double> lons(xySize);
		std::vector<double> lats(xySize);

		ret = nc_get_var_double(ncid, lonVarId, lons.data());
		ret = nc_get_var_double(ncid, latVarId, lats.data());

		m_lonValues.clear();
		m_lonValues.reserve(static_cast<int>(xySize));
		for (size_t i = 0; i < xySize; ++i) {
			m_lonValues.push_back(lons[i]);
		}
		m_latValues.clear();
		m_latValues.reserve(static_cast<int>(xySize));
		for (size_t i = 0; i < xySize; ++i) {
			m_latValues.push_back(lats[i]);
		}
	} else {
		// the coordinate system type is LonLat.
		m_coordinateSystemType = LonLat;
		ret = nc_inq_varid(ncid, LAT, &latVarId);
		ret = nc_inq_varid(ncid, LON, &lonVarId);

		// load lon, lat values
		int lonDimId, latDimId;
		ret = nc_inq_dimid(ncid, LAT, &latDimId);
		ret = nc_inq_dimid(ncid, LON, &lonDimId);

		size_t lonSize, latSize;
		ret = nc_inq_dimlen(ncid, latDimId, &latSize);
		ret = nc_inq_dimlen(ncid, lonDimId, &lonSize);

		std::vector<double> lons(lonSize);
		std::vector<double> lats(latSize);

		ret = nc_get_var_double(ncid, lonVarId, lons.data());
		ret = nc_get_var_double(ncid, latVarId, lats.data());

		m_lonValues.clear();
		m_lonValues.reserve(static_cast<int>(lonSize));
		for (size_t i = 0; i < lonSize; ++i) {
			m_lonValues.push_back(lons[i]);
		}
		m_latValues.clear();
		m_latValues.reserve(static_cast<int>(latSize));
		for (size_t i = 0; i < latSize; ++i) {
			m_latValues.push_back(lats[i]);
		}
	}

	// ------------------------
	// Load dimension values
	// Only for checking.
	// ------------------------

	GridAttributeDimensionsContainer* dims = dimensions();
	for (int i = 0; i < dims->containers().size(); ++i) {
		GridAttributeDimensionContainer* c = dims->containers().at(i);
		int dDimId, dVarId;
		ret = nc_inq_varid(ncid, c->name().c_str(), &dVarId);
		ret = nc_inq_dimid(ncid, c->name().c_str(), &dDimId);
		size_t dimSize;
		ret = nc_inq_dimlen(ncid, dDimId, &dimSize);

		if (dimSize == 0) {
			// no data to load for this dimension.
			break;
		}

		if (dynamic_cast<GridAttributeDimensionIntegerContainer*>(c) != nullptr) {
			std::vector<int> vals(dimSize);
			ret = nc_get_var_int(ncid, dVarId, vals.data());
			std::vector<QVariant> vals1(dimSize, 0);
			for (size_t j = 0; j < dimSize; ++j) {
				QVariant v(vals[j]);
				vals1.push_back(v);
			}

			auto vals2 = c->variantValues();
			if (vals1 != vals2) {
				// invalid!
				// @todo add error handling!
			}
		} else if (dynamic_cast<GridAttributeDimensionRealContainer*>(c) != nullptr) {
			std::vector<double> vals(dimSize);
			ret = nc_get_var_double(ncid, dVarId, vals.data());
			std::vector<QVariant> vals1(dimSize, 0);
			for (size_t j = 0; j < dimSize; ++j) {
				QVariant v(vals[j]);
				vals1.push_back(v);
			}

			auto vals2 = c->variantValues();
			if (vals1 != vals2) {
				// invalid!
				// @todo add error handling!
			}
		}
	}
	nc_close(ncid);

	updateShapeData();
	// load data for index 0;
	handleDimensionCurrentIndexChange(0, 0);
}

void GeoDataNetcdf::saveExternalData(const QString& /*filename*/)
{

}

void GeoDataNetcdf::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataNetcdf::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	m_opacityPercent = loadOpacityPercent(node);
}

void GeoDataNetcdf::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	writeOpacityPercent(m_opacityPercent, writer);
}

void GeoDataNetcdf::doApplyOffset(double x, double y)
{
	auto points = m_grid->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		double p[3];
		points->GetPoint(i, p);
		p[0] -= x;
		p[1] -= y;
		points->SetPoint(i, p);
	}
	points->Modified();

	auto sPoints = m_simplifiedGrid->GetPoints();
	if (sPoints != nullptr) {
		for (vtkIdType i = 0; i < sPoints->GetNumberOfPoints(); ++i) {
			double p[3];
			sPoints->GetPoint(i, p);
			p[0] -= x;
			p[1] -= y;
			sPoints->SetPoint(i, p);
		}
		sPoints->Modified();
	}

	updateRegionPolyData();
}

void GeoDataNetcdf::updateShapeData()
{
	auto offset = projectData()->mainfile()->offset();
	CoordinateSystem* cs = projectData()->mainfile()->coordinateSystem();

	vtkPoints* points = m_grid->GetPoints();
	if (m_coordinateSystemType == XY) {
		m_grid->SetDimensions(static_cast<int> (m_xValues.size()) + 1, static_cast<int> (m_yValues.size()) + 1, 1);
		points->Initialize();
		points->Allocate((m_xValues.size() + 1) * (m_yValues.size() + 1));
		std::vector<double> longitudes, latitudes;
		longitudes.assign((m_xValues.size() + 1) * (m_yValues.size() + 1), 0);
		latitudes.assign((m_xValues.size() + 1) * (m_yValues.size() + 1), 0);

		int xsize = m_xValues.size();
		int ysize = m_yValues.size();

		// middle points
		for (int j = 1; j < ysize; ++j) {
			for (int i = 1; i < xsize; ++i) {
				double longitude = 0;
				double latitude = 0;

				for (int jj = 0; jj <= 1; ++jj) {
					for (int ii = 0; ii <= 1; ++ii) {
						longitude += m_lonValues[calcIndex(i + ii - 1, j + jj - 1, xsize)];
						latitude  += m_latValues[calcIndex(i + ii - 1, j + jj - 1, xsize)];
					}
				}

				longitude /= 4;
				latitude  /= 4;

				longitudes[calcIndex(i, j, xsize + 1)] = longitude;
				latitudes[calcIndex(i, j, xsize + 1)] = latitude;
			}
		}
		// i = 0
		for (int j = 1; j < m_yValues.size(); ++j) {
			double longitude = longitudes[calcIndex(1, j, xsize + 1)] * 2;
			double latitude = latitudes[calcIndex(1, j, xsize + 1)] * 2;
			longitude -= longitudes[calcIndex(2, j, xsize + 1)];
			latitude -= latitudes[calcIndex(2, j, xsize + 1)];
			longitudes[calcIndex(0, j, xsize + 1)] = longitude;
			latitudes[calcIndex(0, j, xsize + 1)] = latitude;
		}

		// i = m_xValues.size()
		for (int j = 1; j < m_yValues.size(); ++j) {
			double longitude = longitudes[calcIndex(xsize - 1, j, xsize + 1)] * 2;
			double latitude = latitudes[calcIndex(xsize - 1, j, xsize + 1)] * 2;
			longitude -= longitudes[calcIndex(xsize - 2, j, xsize + 1)];
			latitude -= latitudes[calcIndex(xsize - 2, j, xsize + 1)];
			longitudes[calcIndex(xsize, j, xsize + 1)] = longitude;
			latitudes[calcIndex(xsize, j, xsize + 1)] = latitude;
		}

		// j = 0
		for (int i = 1; i < m_xValues.size(); ++i) {
			double longitude = longitudes[calcIndex(i, 1, xsize + 1)] * 2;
			double latitude = latitudes[calcIndex(i, 1, xsize + 1)] * 2;
			longitude -= longitudes[calcIndex(i, 2, xsize + 1)];
			latitude -= latitudes[calcIndex(i, 2, xsize + 1)];
			longitudes[calcIndex(i, 0, xsize + 1)] = longitude;
			latitudes[calcIndex(i, 0, xsize + 1)] = latitude;
		}

		// j = m_yValues.size()
		for (int i = 1; i < m_xValues.size(); ++i) {
			double longitude = longitudes[calcIndex(i, ysize - 1, xsize + 1)] * 2;
			double latitude = latitudes[calcIndex(i, ysize - 1, xsize + 1)] * 2;
			longitude -= longitudes[calcIndex(i, ysize - 2, xsize + 1)];
			latitude  -= latitudes[calcIndex(i, ysize - 2, xsize + 1)];
			longitudes[calcIndex(i, ysize, xsize + 1)] = longitude;
			latitudes[calcIndex(i, ysize, xsize + 1)] = latitude;
		}

		// bottom-left
		double longitude = longitudes[calcIndex(1, 1, xsize + 1)] * 2;
		double latitude  = latitudes[calcIndex(1, 1, xsize + 1)] * 2;
		longitude -= longitudes[calcIndex(2, 2, xsize + 1)];
		latitude  -= latitudes[calcIndex(2, 2, xsize + 1)];
		longitudes[calcIndex(0, 0, xsize + 1)] = longitude;
		latitudes[calcIndex(0, 0, xsize + 1)] = latitude;

		// bottom-right
		longitude = longitudes[calcIndex(xsize - 1, 1, xsize + 1)] * 2;
		latitude  = latitudes[calcIndex(xsize - 1, 1, xsize + 1)] * 2;
		longitude -= longitudes[calcIndex(xsize - 2, 2, xsize + 1)];
		latitude  -= latitudes[calcIndex(xsize - 2, 2, xsize + 1)];
		longitudes[calcIndex(xsize, 0, xsize + 1)] = longitude;
		latitudes[calcIndex(xsize, 0, xsize + 1)] = latitude;

		// top-left
		longitude = longitudes[calcIndex(1, ysize - 1, xsize + 1)] * 2;
		latitude  = latitudes[calcIndex(1, ysize - 1, xsize + 1)] * 2;
		longitude -= longitudes[calcIndex(2, ysize - 2, xsize + 1)];
		latitude  -= latitudes[calcIndex(2, ysize - 2, xsize + 1)];
		longitudes[calcIndex(0, ysize, xsize + 1)] = longitude;
		latitudes[calcIndex(0, ysize, xsize + 1)] = latitude;

		// top-right
		longitude = longitudes[calcIndex(xsize - 1, ysize - 1, xsize + 1)] * 2;
		latitude  = latitudes[calcIndex(xsize - 1, ysize - 1, xsize + 1)] * 2;
		longitude -= longitudes[calcIndex(xsize - 2, ysize - 2, xsize + 1)];
		latitude  -= latitudes[calcIndex(xsize - 2, ysize - 2, xsize + 1)];
		longitudes[calcIndex(xsize, ysize, xsize + 1)] = longitude;
		latitudes[calcIndex(xsize, ysize, xsize + 1)] = latitude;
		for (int j = 0; j < m_yValues.size() + 1; ++j) {
			for (int i = 0; i < m_xValues.size() + 1; ++i) {
				double longitude = longitudes[calcIndex(i, j, xsize + 1)];
				double latitude  = latitudes[calcIndex(i, j, xsize + 1)];
				double x, y;
				if (cs->isLongLat()) {
					x = longitude;
					y = latitude;
				} else {
					cs->mapGeoToGrid(longitude, latitude, &x, &y);
				}
				points->InsertNextPoint(x - offset.x(), y - offset.y(), 0);
			}
		}
	} else if (m_coordinateSystemType == LonLat) {
		m_grid->SetDimensions(static_cast<int> (m_lonValues.size()) + 1, static_cast<int> (m_latValues.size()) + 1, 1);
		points->Initialize();
		points->Allocate((m_lonValues.size() + 1) * (m_latValues.size() + 1));
		for (int j = 0; j < m_latValues.size() + 1; ++j) {
			double latitude;
			if (j == 0) {
				double cellsize = m_latValues.at(1) - m_latValues.at(0);
				latitude = m_latValues.at(0) - cellsize * 0.5;
			} else if (j == m_latValues.size()) {
				double cellsize = m_latValues.at(m_latValues.size() - 1) - m_latValues.at(m_latValues.size() - 2);
				latitude = m_latValues.at(m_latValues.size() - 1) + cellsize * 0.5;
			} else {
				latitude = (m_latValues.at(j - 1) + m_latValues.at(j)) * 0.5;
			}
			for (int i = 0; i < m_lonValues.size() + 1; ++i) {
				double longitude;
				if (i == 0) {
					double cellsize = m_lonValues.at(1) - m_lonValues.at(0);
					longitude = m_lonValues.at(0) - cellsize * 0.5;
				} else if (i == m_lonValues.size()) {
					double cellsize = m_lonValues.at(m_lonValues.size() - 1) - m_lonValues.at(m_lonValues.size() - 2);
					longitude = m_lonValues.at(m_lonValues.size() - 1) + cellsize * 0.5;
				} else {
					longitude = (m_lonValues.at(i - 1) + m_lonValues.at(i)) * 0.5;
				}
				double x, y;
				cs->mapGeoToGrid(longitude, latitude, &x, &y);
				points->InsertNextPoint(x - offset.x(), y - offset.y(), 0);
			}
		}
	}

	points->Modified();

	vtkDataArray* da = m_grid->GetCellData()->GetArray("values");
	da->Initialize();
	int size = xSize() * ySize();
	da->Allocate(size);
	for (int i = 0; i < size; ++i) {
		da->InsertNextTuple1(0);
	}
	da->Modified();

	updateRegionPolyData();
}

void GeoDataNetcdf::updateActorSettings()
{
	m_actor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
}

nc_type GeoDataNetcdf::getNcType(SolverDefinitionGridAttribute* cond)
{
	if (dynamic_cast<SolverDefinitionGridAttributeInteger*>(cond) != 0) {
		return NC_INT;
	} else if (dynamic_cast<SolverDefinitionGridAttributeReal*>(cond) != 0) {
		return NC_DOUBLE;
	}
	return NC_NAT;
}

nc_type GeoDataNetcdf::getNcType(SolverDefinitionGridAttributeDimension* dim)
{
	if (dynamic_cast<SolverDefinitionGridAttributeIntegerDimension*>(dim) != 0) {
		return NC_INT;
	} else if (dynamic_cast<SolverDefinitionGridAttributeRealDimension*>(dim) != 0) {
		return NC_DOUBLE;
	}
	return NC_NAT;
}

int GeoDataNetcdf::defineCoords(int ncid, int* xDimId, int* yDimId, int* lonDimId, int* latDimId, int* xVarId, int* yVarId, int* lonVarId, int* latVarId)
{
	int ret;
	QString tmp;

	if (m_coordinateSystemType == XY) {
		// define x, y dimensions
		ret = nc_def_dim(ncid, X, static_cast<size_t>(m_xValues.size()), xDimId);
		if (ret != NC_NOERR) {return ret;}
		ret = nc_def_dim(ncid, Y, static_cast<size_t>(m_yValues.size()), yDimId);
		if (ret != NC_NOERR) {return ret;}

		// define x, y variables
		ret = nc_def_var(ncid, X, NC_DOUBLE, 1, xDimId, xVarId);
		if (ret != NC_NOERR) {return ret;}
		ret = nc_def_var(ncid, Y, NC_DOUBLE, 1, yDimId, yVarId);
		if (ret != NC_NOERR) {return ret;}

		tmp = "x-coordinate in Cartesian system";
		ret = nc_put_att_text(ncid, *xVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "m";
		ret = nc_put_att_text(ncid, *xVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "X";
		ret = nc_put_att_text(ncid, *xVarId, "axis", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}

		tmp = "y-coordinate in Cartesian system";
		ret = nc_put_att_text(ncid, *yVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "m";
		ret = nc_put_att_text(ncid, *yVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "Y";
		ret = nc_put_att_text(ncid, *yVarId, "axis", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}

		// define lon, lat variables

		int dims[2];
		dims[0] = *yDimId;
		dims[1] = *xDimId;

		ret = nc_def_var(ncid, LON, NC_DOUBLE, 2, &(dims[0]), lonVarId);
		if (ret != NC_NOERR) {return ret;}
		ret = nc_def_var(ncid, LAT, NC_DOUBLE, 2, &(dims[0]), latVarId);
		if (ret != NC_NOERR) {return ret;}

		tmp = "Longitude";
		ret = nc_put_att_text(ncid, *lonVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "degrees_east";
		ret = nc_put_att_text(ncid, *lonVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "Latitude";
		ret = nc_put_att_text(ncid, *latVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "degrees_north";
		ret = nc_put_att_text(ncid, *latVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		return NC_NOERR;

	} else if (m_coordinateSystemType == LonLat) {
		// define lon, lat dimensions
		ret = nc_def_dim(ncid, LON, static_cast<size_t>(m_lonValues.size()), lonDimId);
		if (ret != NC_NOERR) {return ret;}
		ret = nc_def_dim(ncid, LAT, static_cast<size_t>(m_latValues.size()), latDimId);
		if (ret != NC_NOERR) {return ret;}

		// define lon, lat variables
		ret = nc_def_var(ncid, LON, NC_DOUBLE, 1, lonDimId, lonVarId);
		if (ret != NC_NOERR) {return ret;}
		ret = nc_def_var(ncid, LAT, NC_DOUBLE, 1, latDimId, latVarId);
		if (ret != NC_NOERR) {return ret;}

		tmp = "Longitude";
		ret = nc_put_att_text(ncid, *lonVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "degrees_east";
		ret = nc_put_att_text(ncid, *lonVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "Latitude";
		ret = nc_put_att_text(ncid, *latVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		tmp = "degrees_north";
		ret = nc_put_att_text(ncid, *latVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR) {return ret;}
		return NC_NOERR;
	}
	return NC_NOERR;
}

int GeoDataNetcdf::defineDimensions(int ncid, std::vector<int>* dimIds, std::vector<int>* varIds)
{
	const GridAttributeDimensionsContainer* dims = dimensions();
	int ret;
	for (int i = 0; i < dims->containers().size(); ++i) {
		int dimId;
		GridAttributeDimensionContainer* c = dims->containers().at(i);
		size_t len = c->count();
		// time is the special dimension: it is defined as unlimited.
		if (c->definition()->name() == "Time") {len = NC_UNLIMITED;}
		ret = nc_def_dim(ncid, c->definition()->name().c_str(), len, &dimId);
		if (ret != NC_NOERR) {return ret;}
		dimIds->push_back(dimId);
	}
	for (int i = 0; i < dims->containers().size(); ++i) {
		int varId;
		GridAttributeDimensionContainer* c = dims->containers().at(i);
		nc_type ncType = getNcType(c->definition());
		int dimId = dimIds->at(i);
		ret = nc_def_var(ncid, c->definition()->name().c_str(), ncType, 1, &dimId, &varId);
		if (ret != NC_NOERR) {return ret;}
		varIds->push_back(varId);

		// for special dimensions
		QString tmp;
		if (c->definition()->name() == "Time") {
			// time is normalized to seconds since 1970-01-01 00:00:00
			tmp = "time of measurement";
			ret = nc_put_att_text(ncid, varId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
			if (ret != NC_NOERR) {return ret;}
			tmp = "seconds since 1970-01-01 00:00:00";
			ret = nc_put_att_text(ncid, varId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
			if (ret != NC_NOERR) {return ret;}
		}
	}
	return NC_NOERR;
}

int GeoDataNetcdf::defineValue(int ncid, int xId, int yId, const std::vector<int> &dimIds, int* varId)
{
	int ret;
	int ndims = static_cast<int>(dimensions()->containers().size()) + 2;
	std::vector<int> dimids(ndims);
	std::vector<size_t> chunksizes(ndims);

	// reverse order!
	for (int i = 0; i < ndims - 2; ++i) {
		dimids[ndims - 3 - i] = dimIds.at(i);
		chunksizes[ndims - 3 - i] = 1;
	}
	dimids[ndims - 2] = yId;
	dimids[ndims - 1] = xId;
	if (m_coordinateSystemType == XY) {
		chunksizes[ndims - 2] = m_yValues.size();
		chunksizes[ndims - 1] = m_xValues.size();
	} else if (m_coordinateSystemType == LonLat) {
		chunksizes[ndims - 2] = m_latValues.size();
		chunksizes[ndims - 1] = m_lonValues.size();
	}
	ret = nc_def_var(ncid, VALUE, getNcType(gridAttribute()), ndims, dimids.data(), varId);
	ret = nc_def_var_deflate(ncid, *varId, 0, 1, 2);
	ret = nc_def_var_chunking(ncid, *varId, NC_CHUNKED, chunksizes.data());

	if (ret != NC_NOERR) {return ret;}

	return NC_NOERR;
}

int GeoDataNetcdf::outputCoords(int ncid, int xId, int yId, int lonId, int latId)
{
	if (m_coordinateSystemType == XY) {
		std::vector<double> xs(m_xValues.size());
		std::vector<double> ys(m_yValues.size());
		int ret = NC_NOERR;

		for (int i = 0; i < m_xValues.size(); ++i) {
			xs[i] = m_xValues.at(i);
		}
		for (int i = 0; i < m_yValues.size(); ++i) {
			ys[i] = m_yValues.at(i);
		}
		ret = nc_put_var_double(ncid, xId, xs.data());
		if (ret != NC_NOERR) {return ret;}
		ret = nc_put_var_double(ncid, yId, ys.data());
		if (ret != NC_NOERR) {return ret;}
	}

	if (m_coordinateSystemType == XY || m_coordinateSystemType == LonLat) {
		std::vector<double> lons(m_lonValues.size());
		std::vector<double> lats(m_latValues.size());
		int ret;

		for (int i = 0; i < m_lonValues.size(); ++i) {
			lons[i] = m_lonValues.at(i);
		}
		for (int i = 0; i < m_latValues.size(); ++i) {
			lats[i] = m_latValues.at(i);
		}
		ret = nc_put_var_double(ncid, lonId, lons.data());
		if (ret != NC_NOERR) {return ret;}
		ret = nc_put_var_double(ncid, latId, lats.data());
		if (ret != NC_NOERR) {return ret;}
	}
	return NC_NOERR;
}

int GeoDataNetcdf::outputDimensions(int ncid, const std::vector<int> &varIds)
{
	const GridAttributeDimensionsContainer* dims = dimensions();
	for (int i = 0; i < dims->containers().size(); ++i) {
		int ret;
		int varId = varIds.at(i);
		GridAttributeDimensionContainer* c = dims->containers().at(i);
		if (dynamic_cast<GridAttributeDimensionIntegerContainer*>(c) != nullptr) {
			GridAttributeDimensionIntegerContainer* c2 =
				dynamic_cast<GridAttributeDimensionIntegerContainer*>(c);
			std::vector<int> vals(c->count());
			const std::vector<int>& listVals = c2->values();
			for (int j = 0; j < listVals.size(); ++j) {
				vals[j] = listVals.at(j);
			}
			size_t start = 0;
			size_t len = listVals.size();
			ret = nc_put_vara_int(ncid, varId, &start, &len, vals.data());
		} else if (dynamic_cast<GridAttributeDimensionRealContainer*>(c) != 0) {
			GridAttributeDimensionRealContainer* c2 =
				dynamic_cast<GridAttributeDimensionRealContainer*>(c);
			std::vector<double> vals(c->count());
			const std::vector<double>& listVals = c2->values();
			for (int j = 0; j < listVals.size(); ++j) {
				vals[j] = listVals.at(j);
			}
			size_t start = 0;
			size_t len = listVals.size();
			ret = nc_put_vara_double(ncid, varId, &start, &len, vals.data());
		}
		if (ret != NC_NOERR) {return ret;}
	}
	return NC_NOERR;
}

void GeoDataNetcdf::handleDimensionCurrentIndexChange(int oldIndex, int newIndex)
{
	doHandleDimensionCurrentIndexChange(oldIndex, newIndex);
}

void GeoDataNetcdf::handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after)
{
	GridAttributeDimensionContainer* dim =
		dynamic_cast<GridAttributeDimensionContainer*>(sender());
	doHandleDimensionValuesChange(dim, before, after);
}

int GeoDataNetcdf::getValueVarId(int ncid, int* varId)
{
	return nc_inq_varid(ncid, VALUE, varId);
}

void GeoDataNetcdf::viewOperationEndedGlobal(PreProcessorGraphicsViewInterface* v)
{
	double xmin, xmax, ymin, ymax;
	v->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
	updateSimpifiedGrid(xmin, xmax, ymin, ymax);
}

void GeoDataNetcdf::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataNetcdf::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
	m_regionActor->SetPosition(0, 0, range.min());
}

void GeoDataNetcdf::updateSimpifiedGrid(double xmin, double xmax, double ymin, double ymax)
{
	m_isMasked = false;

	double xcenter = (xmin + xmax) * 0.5;
	double ycenter = (ymin + ymax) * 0.5;

	double xwidth = (xmax - xmin);
	double ywidth = (ymax - ymin);

	xmin -= xwidth * 0.2;
	xmax += xwidth * 0.2;
	ymin -= ywidth * 0.2;
	ymax += ywidth * 0.2;

	vtkIdType vid = m_grid->FindPoint(xcenter, ycenter, 0);
	if (vid == -1) {return;}

	double* cv = m_grid->GetPoint(vid);
	if (*cv < xmin || *cv > xmax || *(cv + 1) < ymin || *(cv + 1) > ymax) {
		m_simplifiedGrid = vtkSmartPointer<vtkStructuredGrid>::New();
		m_threshold->SetInputData(m_simplifiedGrid);
		m_threshold->Modified();
		return;
	}
	int dimI = 1, dimJ = 1;

	if (m_coordinateSystemType == GeoDataNetcdf::XY) {
		dimI = static_cast<int> (m_xValues.size());
		dimJ = static_cast<int> (m_yValues.size());
	} else if (m_coordinateSystemType == LonLat) {
		dimI = static_cast<int> (m_lonValues.size());
		dimJ = static_cast<int> (m_latValues.size());
	}

	RectRegion region(xmin, xmax, ymin, ymax);
	unsigned int centerI, centerJ;
	getIJIndex(vid, &centerI, &centerJ);

	int lineLimitIMin, lineLimitIMax, lineLimitJMin, lineLimitJMax;
	double tmpv[3];

	// test I = 0
	m_grid->GetPoint(vertexIndex(0, centerJ), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitIMin = 0;
	} else {
		lineLimitIMin = lineLimitI(centerJ, centerI, 0, dimI, dimJ, region);
	}
	// test I = imax
	m_grid->GetPoint(vertexIndex(dimI, centerJ), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitIMax = dimI;
	} else {
		lineLimitIMax = lineLimitI(centerJ, centerI, dimI, dimI, dimJ, region);
	}
	// test J = 0
	m_grid->GetPoint(vertexIndex(centerI, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitJMin = 0;
	} else {
		lineLimitJMin = lineLimitJ(centerI, centerJ, 0, dimI, dimJ, region);
	}
	// test J = jmax
	m_grid->GetPoint(vertexIndex(centerI, dimJ), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		lineLimitJMax = dimJ;
	} else {
		lineLimitJMax = lineLimitJ(centerI, centerJ, dimJ, dimI, dimJ, region);
	}

	int lineLimitIMin2, lineLimitIMax2, lineLimitJMin2, lineLimitJMax2;

	// test I min direction
	if (lineLimitIMin == 0) {
		lineLimitIMin2 = 0;
	} else {
		if (lineAtIIntersect(0, dimI, dimJ, region)) {
			lineLimitIMin2 = 0;
		} else {
			lineLimitIMin2 = lineLimitI2(lineLimitIMin, 0, dimI, dimJ, region);
		}
	}
	// test I max direction
	if (lineLimitIMax == dimI) {
		lineLimitIMax2 = dimI;
	} else {
		if (lineAtIIntersect(dimI, dimI, dimJ, region)) {
			lineLimitIMax2 = dimI;
		} else {
			lineLimitIMax2 = lineLimitI2(lineLimitIMax, dimI, dimI, dimJ, region);
		}
	}

	// test J min direction
	if (lineLimitJMin == 0) {
		lineLimitJMin2 = 0;
	} else {
		if (lineAtJIntersect(0, dimI, dimJ, region)) {
			lineLimitJMin2 = 0;
		} else {
			lineLimitJMin2 = lineLimitJ2(lineLimitJMin, 0, dimI, dimJ, region);
		}
	}
	// test J max direction
	if (lineLimitJMax == dimJ) {
		lineLimitJMax2 = dimJ;
	} else {
		if (lineAtJIntersect(dimJ, dimI, dimJ, region)) {
			lineLimitJMax2 = dimJ;
		} else {
			lineLimitJMax2 = lineLimitI2(lineLimitJMax, dimJ, dimI, dimJ, region);
		}
	}

	vtkSmartPointer<vtkExtractGrid> exGrid = vtkSmartPointer<vtkExtractGrid>::New();
	exGrid->SetVOI(lineLimitIMin2, lineLimitIMax2, lineLimitJMin2, lineLimitJMax2, 0, 0);
	exGrid->SetInputData(m_grid);
	exGrid->Update();
	m_simplifiedGrid = exGrid->GetOutput();
	int exRate = 1;
	while (m_simplifiedGrid->GetNumberOfCells() > MAX_DRAWCELLCOUNT) {
		exRate *= 2;
		exGrid->SetSampleRate(exRate, exRate, 1);
		exGrid->Update();
		m_simplifiedGrid = exGrid->GetOutput();
		m_isMasked = true;
	}
	m_threshold->SetInputData(m_simplifiedGrid);
	m_threshold->Modified();
}

void GeoDataNetcdf::getIJIndex(vtkIdType id, unsigned int* i, unsigned int* j) const
{
	int dimI = 1;
	if (m_coordinateSystemType == XY) {
		dimI = static_cast<int> (m_xValues.size()) + 1;
	} else if (m_coordinateSystemType == LonLat) {
		dimI = static_cast<int> (m_lonValues.size()) + 1;
	}

	*i = id % dimI;
	*j = id / dimI;
}

unsigned int GeoDataNetcdf::vertexIndex(unsigned int i, unsigned int j) const
{
	int dimI = 1;
	if (m_coordinateSystemType == XY) {
		dimI = static_cast<int> (m_xValues.size()) + 1;
	} else if (m_coordinateSystemType == LonLat) {
		dimI = static_cast<int> (m_lonValues.size()) + 1;
	}
	return dimI * j + i;
}

int GeoDataNetcdf::lineLimitI(int j, int iIn, int iOut, int dimI, int dimJ, const RectRegion& region) const
{
	if (qAbs(iOut - iIn) == 1) {
		return iIn;
	}
	int i = (iIn + iOut) / 2;
	double tmpv[3];
	m_grid->GetPoint(vertexIndex(i, j), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		return lineLimitI(j, i, iOut, dimI, dimJ, region);
	} else {
		return lineLimitI(j, iIn, i, dimI, dimJ, region);
	}
}

int GeoDataNetcdf::lineLimitJ(int i, int jIn, int jOut, int dimI, int dimJ, const RectRegion& region) const
{
	if (qAbs(jOut - jIn) == 1) {
		return jIn;
	}
	int j = (jIn + jOut) / 2;
	double tmpv[3];
	m_grid->GetPoint(vertexIndex(i, j), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])) {
		return lineLimitJ(i, j, jOut, dimI, dimJ, region);
	} else {
		return lineLimitJ(i, jIn, j, dimI, dimJ, region);
	}
}

int GeoDataNetcdf::lineLimitI2(int iIn, int iOut, int dimI, int dimJ, const RectRegion& region) const
{
	if (qAbs(iOut - iIn) == 1) {
		return iIn;
	}
	int i = (iIn + iOut) / 2;
	if (lineAtIIntersect(i, dimI, dimJ, region)) {
		return lineLimitI2(i, iOut, dimI, dimJ, region);
	} else {
		return lineLimitI2(iIn, i, dimI, dimJ, region);
	}
}

int GeoDataNetcdf::lineLimitJ2(int jIn, int jOut, int dimI, int dimJ, const RectRegion& region) const
{
	if (qAbs(jOut - jIn) == 1) {
		return jIn;
	}
	int j = (jIn + jOut) / 2;
	if (lineAtJIntersect(j, dimI, dimJ, region)) {
		return lineLimitJ2(j, jOut, dimI, dimJ, region);
	} else {
		return lineLimitJ2(jIn, j, dimI, dimJ, region);
	}
}

bool GeoDataNetcdf::lineAtIIntersect(int i, int /*dimI*/, int dimJ, const RectRegion& region) const
{
	QPointF p1, p2;
	double tmpv[3];
	m_grid->GetPoint(vertexIndex(i, 0), tmpv);
	p1 = QPointF(tmpv[0], tmpv[1]);
	for (int j = 1; j < dimJ; ++j) {
		m_grid->GetPoint(vertexIndex(i, j), tmpv);
		p2 = QPointF(tmpv[0], tmpv[1]);
		QLineF line(p1, p2);
		if (region.intersect(line)) {return true;}
	}
	return false;
}

bool GeoDataNetcdf::lineAtJIntersect(int j, int dimI, int /*dimJ*/, const RectRegion& region) const
{
	QPointF p1, p2;
	double tmpv[3];
	m_grid->GetPoint(vertexIndex(0, j), tmpv);
	p1 = QPointF(tmpv[0], tmpv[1]);
	for (int i = 1; i < dimI; ++i) {
		m_grid->GetPoint(vertexIndex(i, j), tmpv);
		p2 = QPointF(tmpv[0], tmpv[1]);
		QLineF line(p1, p2);
		if (region.intersect(line)) {return true;}
	}
	return false;
}

void GeoDataNetcdf::updateRegionPolyData()
{
	double bounds[6];
	m_grid->GetBounds(bounds);
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->Allocate(4);
	points->InsertNextPoint(bounds[0], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[3], 0);
	points->InsertNextPoint(bounds[0], bounds[3], 0);
	m_regionPolyData->SetPoints(points);

	vtkIdType pts[4] = {0, 1, 2, 3};
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(4, pts);
	m_regionPolyData->SetPolys(cells);
	m_regionPolyData->Modified();
	actorCollection()->RemoveItem(m_regionActor);
	actorCollection()->AddItem(m_regionActor);
	updateVisibilityWithoutRendering();
}

int GeoDataNetcdf::xSize() const
{
	if (m_coordinateSystemType == XY) {
		return static_cast<int> (m_xValues.size());
	} else if (m_coordinateSystemType == LonLat) {
		return static_cast<int> (m_lonValues.size());
	}
	return 0;
}
int GeoDataNetcdf::ySize() const
{
	if (m_coordinateSystemType == XY) {
		return static_cast<int> (m_yValues.size());
	} else if (m_coordinateSystemType == LonLat) {
		return static_cast<int> (m_lonValues.size());
	}
	return 0;
}
