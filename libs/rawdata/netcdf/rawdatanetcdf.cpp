#include "rawdatanetcdf.h"

#include <guicore/pre/gridcond/base/gridrelatedconditiondimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensioncontainert.h>
#include <guicore/solverdef/solverdefinitiongridrelatedconditiont.h>
#include <guicore/solverdef/solverdefinitiongridrelatedconditiondimension.h>
#include <guicore/solverdef/solverdefinitiongridrelatedconditiondimensiont.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guibase/coordinatesystem.h>

#include <misc/stringtool.h>

#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkExtractGrid.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellArray.h>

#include <netcdf.h>

#define VALUE "value"
#define X "x"
#define Y "y"

#define LON "lon"
#define LAT "lat"

const int RawDataNetcdf::MAX_DRAWCELLCOUNT = 200000;

RawDataNetcdf::RawDataNetcdf(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
	: RawData(d, creator, condition)
{
	m_opacityPercent = 50;

	m_grid = vtkSmartPointer<vtkStructuredGrid>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	m_grid->SetPoints(points);

	m_isMasked = false;

	//	setupActions();
}

RawDataNetcdf::~RawDataNetcdf()
{
	actorCollection()->RemoveItem(m_regionActor);
	actorCollection()->RemoveItem(m_actor);
	renderer()->RemoveActor(m_regionActor);
	renderer()->RemoveActor(m_actor);
}

void RawDataNetcdf::setupActors()
{
	m_threshold = vtkSmartPointer<vtkThreshold>::New();
	m_threshold->SetInputData(m_grid);
	double tVal = thresholdValue();
	if (tVal < 0){
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

void RawDataNetcdf::loadExternalData(const QString& filename)
{
	std::string fname = iRIC::toStr(filename);
	int ncid, ret;
	ret = nc_open(fname.c_str(), NC_NOWRITE, &ncid);

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

	if (xyexist){
		// the coordinate system type is XY.
		m_coordinateSystemType = XY;

		// load X, Y values
		int xDimId, yDimId;
		ret = nc_inq_dimid(ncid, X, &xDimId);
		ret = nc_inq_dimid(ncid, Y, &yDimId);

		size_t xSize, ySize;
		ret = nc_inq_dimlen(ncid, xDimId, &xSize);
		ret = nc_inq_dimlen(ncid, yDimId, &ySize);

		double *xs = new double[xSize];
		double *ys = new double[ySize];

		ret = nc_get_var_double(ncid, xVarId, xs);
		ret = nc_get_var_double(ncid, yVarId, ys);

		m_xValues.clear();
		m_xValues.reserve(static_cast<int>(xSize));
		for (int i = 0; i < xSize; ++i){
			m_xValues.append(*(xs + i));
		}
		m_yValues.clear();
		m_yValues.reserve(static_cast<int>(ySize));
		for (int i = 0; i < ySize; ++i){
			m_yValues.append(*(ys + i));
		}
		delete xs;
		delete ys;

		// load Lon Lat Values
		ret = nc_inq_varid(ncid, LAT, &latVarId);
		ret = nc_inq_varid(ncid, LON, &lonVarId);

		// load lon, lat values
		int lonDimId, latDimId;
		ret = nc_inq_dimid(ncid, LAT, &latDimId);
		ret = nc_inq_dimid(ncid, LON, &lonDimId);

		size_t lonSize, latSize;
		ret = nc_inq_dimlen(ncid, latDimId, &latSize);
		ret = nc_inq_dimlen(ncid, lonDimId, &lonSize);

		double *lons = new double[lonSize];
		double *lats = new double[latSize];

		ret = nc_get_var_double(ncid, lonVarId, lons);
		ret = nc_get_var_double(ncid, latVarId, lats);

		m_lonValues.clear();
		m_lonValues.reserve(static_cast<int>(lonSize));
		for (int i = 0; i < lonSize; ++i){
			m_lonValues.append(*(lons + i));
		}
		m_latValues.clear();
		m_latValues.reserve(static_cast<int>(latSize));
		for (int i = 0; i < latSize; ++i){
			m_latValues.append(*(lats + i));
		}

		delete lons;
		delete lats;

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

		double *lons = new double[lonSize];
		double *lats = new double[latSize];

		ret = nc_get_var_double(ncid, lonVarId, lons);
		ret = nc_get_var_double(ncid, latVarId, lats);

		m_lonValues.clear();
		m_lonValues.reserve(static_cast<int>(lonSize));
		for (int i = 0; i < lonSize; ++i){
			m_lonValues.append(*(lons + i));
		}
		m_latValues.clear();
		m_latValues.reserve(static_cast<int>(latSize));
		for (int i = 0; i < latSize; ++i){
			m_latValues.append(*(lats + i));
		}

		delete lons;
		delete lats;
	}

	// ------------------------
	// Load dimension values
	// Only for checking.
	// ------------------------

	GridRelatedConditionDimensionsContainer* dims = dimensions();
	for (int i = 0; i < dims->containers().size(); ++i){
		GridRelatedConditionDimensionContainer* c = dims->containers().at(i);
		int dDimId, dVarId;
		ret = nc_inq_varid(ncid, iRIC::toStr(c->name()).c_str(), &dVarId);
		ret = nc_inq_dimid(ncid, iRIC::toStr(c->name()).c_str(), &dDimId);
		size_t dimSize;
		ret = nc_inq_dimlen(ncid, dDimId, &dimSize);

		if (dynamic_cast<GridRelatedConditionDimensionIntegerContainer*>(c) != 0){
			int* vals = new int[c->count()];
			ret = nc_get_var_int(ncid, dVarId, vals);
			QList<QVariant> vals1;
			for (int j = 0; j < c->count(); ++j){
				QVariant v(*(vals + j));
				vals1.append(v);
			}
			delete vals;

			QList<QVariant> vals2 = c->variantValues();
			if (vals1 != vals2){
				// invalid!
				// @todo add error handling!
			}
		} else if (dynamic_cast<GridRelatedConditionDimensionRealContainer*>(c) != 0){
			double* vals = new double[c->count()];
			ret = nc_get_var_double(ncid, dVarId, vals);
			QList<QVariant> vals1;
			for (int j = 0; j < c->count(); ++j){
				QVariant v(*(vals + j));
				vals1.append(v);
			}
			delete vals;

			QList<QVariant> vals2 = c->variantValues();
			if (vals1 != vals2){
				// invalid!
				// @todo add error handling!
			}
		}
	}
	nc_close(ncid);
}

void RawDataNetcdf::saveExternalData(const QString& filename)
{

}

void RawDataNetcdf::updateFilename()
{
	m_filename = m_name;
	m_filename.append(".dat");
}

void RawDataNetcdf::doLoadFromProjectMainFile(const QDomNode& node){
	RawData::doLoadFromProjectMainFile(node);
	m_opacityPercent = loadOpacityPercent(node);
}

void RawDataNetcdf::doSaveToProjectMainFile(QXmlStreamWriter& writer){
	RawData::doSaveToProjectMainFile(writer);
	writeOpacityPercent(m_opacityPercent, writer);
}

void RawDataNetcdf::doApplyOffset(double x, double y)
{

}

void RawDataNetcdf::updateShapeData()
{
	CoordinateSystem* cs = projectData()->mainfile()->coordinateSystem();

	vtkPoints* points = m_grid->GetPoints();
	if (m_coordinateSystemType == XY){
		m_grid->SetDimensions(m_xValues.size() + 1, m_yValues.size() + 1, 1);
		points->Initialize();
		points->Allocate((m_xValues.size() + 1) * (m_yValues.size() + 1));
		double *longitudes = new double[(m_xValues.size() + 1) * (m_yValues.size() + 1)];
		double* latitudes = new double[(m_xValues.size() + 1) * (m_yValues.size() + 1)];
		for (int j = 0; j < m_yValues.size() + 1; ++j){
			for (int i = 0; i < m_xValues.size() + 1; ++i){
				*(longitudes + i + (m_xValues.size() + 1) * j) = 0;
				*(latitudes  + i + (m_xValues.size() + 1) * j) = 0;
			}
		}

		// middle points
		for (int j = 1; j < m_yValues.size(); ++j){
			for (int i = 1; i < m_xValues.size(); ++i){
				double longitude = 0;
				double latitude = 0;

				longitude += m_lonValues[i - 1 + (m_xValues.size() * (j - 1))];
				latitude  += m_latValues[i - 1 + (m_xValues.size() * (j - 1))];

				longitude += m_lonValues[i     + (m_xValues.size() * (j - 1))];
				latitude  += m_latValues[i     + (m_xValues.size() * (j - 1))];

				longitude += m_lonValues[i - 1 + (m_xValues.size() * (j    ))];
				latitude  += m_latValues[i - 1 + (m_xValues.size() * (j    ))];

				longitude += m_lonValues[i     + (m_xValues.size() * (j    ))];
				latitude  += m_latValues[i     + (m_xValues.size() * (j    ))];

				longitude /= 4;
				latitude  /= 4;

				*(longitudes + i + (m_xValues.size() + 1) * j) = longitude;
				*(latitudes  + i + (m_xValues.size() + 1) * j) = latitude;
			}
		}
		// i = 0
		for (int j = 1; j < m_yValues.size(); ++j){
			double longitude = *(longitudes + 1 + (m_xValues.size() + 1) * j) * 2;
			double latitude  = *(latitudes  + 1 + (m_xValues.size() + 1) * j) * 2;
			longitude -= *(longitudes + 2 + (m_xValues.size() + 1) * j);
			latitude  -= *(latitudes  + 2 + (m_xValues.size() + 1) * j);
			*(longitudes + 0 + (m_xValues.size() + 1) * j) = longitude;
			*(latitudes  + 0 + (m_xValues.size() + 1) * j) = latitude;
		}
		// i = m_xValues.size()
		for (int j = 1; j < m_yValues.size(); ++j){
			double longitude = *(longitudes + (m_xValues.size() - 1) + (m_xValues.size() + 1) * j) * 2;
			double latitude  = *(latitudes  + (m_xValues.size() - 1) + (m_xValues.size() + 1) * j) * 2;
			longitude -= *(longitudes + (m_xValues.size() - 2) + (m_xValues.size() + 1) * j);
			latitude  -= *(latitudes  + (m_xValues.size() - 2) + (m_xValues.size() + 1) * j);
			*(longitudes + m_xValues.size() + (m_xValues.size() + 1) * j) = longitude;
			*(latitudes  + m_xValues.size() + (m_xValues.size() + 1) * j) = latitude;
		}
		// j = 0
		for (int i = 1; i < m_xValues.size(); ++i){
			double longitude = *(longitudes + i + (m_xValues.size() + 1) * 1) * 2;
			double latitude  = *(latitudes  + i + (m_xValues.size() + 1) * 1) * 2;
			longitude -= *(longitudes + i + (m_xValues.size() + 1) * 2);
			latitude  -= *(latitudes  + i + (m_xValues.size() + 1) * 2);
			*(longitudes + i + (m_xValues.size() + 1) * 0) = longitude;
			*(latitudes  + i + (m_xValues.size() + 1) * 0) = latitude;
		}
		// j = m_yValues.size()
		for (int i = 1; i < m_xValues.size(); ++i){
			double longitude = *(longitudes + i + (m_xValues.size() + 1) * (m_yValues.size() - 1)) * 2;
			double latitude  = *(latitudes  + i + (m_xValues.size() + 1) * (m_yValues.size() - 1)) * 2;
			longitude -= *(longitudes + i + (m_xValues.size() + 1) * (m_yValues.size() - 2));
			latitude  -= *(latitudes  + i + (m_xValues.size() + 1) * (m_yValues.size() - 2));
			*(longitudes + i + (m_xValues.size() + 1) * m_yValues.size()) = longitude;
			*(latitudes  + i + (m_xValues.size() + 1) * m_yValues.size()) = latitude;
		}
		// bottom-left
		double longitude = *(longitudes + 1 + (m_xValues.size() + 1) * 1) * 2;
		double latitude  = *(latitudes  + 1 + (m_xValues.size() + 1) * 1) * 2;
		longitude -= *(longitudes + 2 + (m_xValues.size() + 1) * 2);
		latitude  -= *(latitudes  + 2 + (m_xValues.size() + 1) * 2);
		*(longitudes + 0) = longitude;
		*(latitudes  + 0) = latitude;

		// bottom-right
		longitude = *(longitudes + (m_xValues.size() - 1) + (m_xValues.size() + 1) * 1) * 2;
		latitude  = *(latitudes  + (m_xValues.size() - 1) + (m_xValues.size() + 1) * 1) * 2;
		longitude -= *(longitudes + (m_xValues.size() - 2) + (m_xValues.size() + 1) * 2);
		latitude  -= *(latitudes  + (m_xValues.size() - 2) + (m_xValues.size() + 1) * 2);
		*(longitudes + m_xValues.size()) = longitude;
		*(latitudes  + m_xValues.size()) = latitude;

		// top-left
		longitude = *(longitudes + 1 + (m_xValues.size() + 1) * (m_yValues.size() - 1)) * 2;
		latitude  = *(latitudes  + 1 + (m_xValues.size() + 1) * (m_yValues.size() - 1)) * 2;
		longitude -= *(longitudes + 2 + (m_xValues.size() + 1) * (m_yValues.size() - 2));
		latitude  -= *(latitudes  + 2 + (m_xValues.size() + 1) * (m_yValues.size() - 2));
		*(longitudes + (m_xValues.size() + 1) * m_yValues.size()) = longitude;
		*(latitudes  + (m_xValues.size() + 1) * m_yValues.size()) = latitude;

		// top-right
		longitude = *(longitudes + (m_xValues.size() - 1) + (m_xValues.size() + 1) * (m_yValues.size() - 1)) * 2;
		latitude  = *(latitudes  + (m_xValues.size() - 1) + (m_xValues.size() + 1) * (m_yValues.size() - 1)) * 2;
		longitude -= *(longitudes + (m_xValues.size() - 2) + (m_xValues.size() + 1) * (m_yValues.size() - 2));
		latitude  -= *(latitudes  + (m_xValues.size() - 2) + (m_xValues.size() + 1) * (m_yValues.size() - 2));
		*(longitudes + m_xValues.size() + (m_xValues.size() + 1) * m_yValues.size()) = longitude;
		*(latitudes  + m_xValues.size() + (m_xValues.size() + 1) * m_yValues.size()) = latitude;
		for (int j = 0; j < m_yValues.size() + 1; ++j){
			for (int i = 0; i < m_xValues.size() + 1; ++i){
				double longitude = *(longitudes + i + (m_xValues.size() + 1) * j);
				double latitude  = *(latitudes  + i + (m_xValues.size() + 1) * j);

				double x, y;
				cs->mapGeoToGrid(longitude, latitude, &x, &y);
				points->InsertNextPoint(x, y, 0);
			}
		}

		delete latitudes;
		delete longitudes;
	} else if (m_coordinateSystemType == LonLat){
		m_grid->SetDimensions(m_lonValues.size() + 1, m_latValues.size() + 1, 1);
		points->Initialize();
		points->Allocate((m_lonValues.size() + 1) * (m_latValues.size() + 1));
		for (int j = 0; j < m_latValues.size() + 1; ++j){
			double latitude;
			if (j == 0){
				double cellsize = m_latValues.at(1) - m_latValues.at(0);
				latitude = m_latValues.at(0) - cellsize * 0.5;
			}
			else if (j == m_latValues.size()){
				double cellsize = m_latValues.at(m_latValues.size() - 1) - m_latValues.at(m_latValues.size() - 2);
				latitude = m_latValues.at(m_latValues.size() - 1) + cellsize * 0.5;
			}
			else {
				latitude = (m_latValues.at(j - 1) + m_latValues.at(j)) * 0.5;
			}
			for (int i = 0; i < m_lonValues.size() + 1; ++i){
				double longitude;
				if (i == 0){
					double cellsize = m_lonValues.at(1) - m_lonValues.at(0);
					longitude = m_lonValues.at(0) - cellsize * 0.5;
				}
				else if (i == m_lonValues.size()){
					double cellsize = m_lonValues.at(m_lonValues.size() - 1) - m_lonValues.at(m_lonValues.size() - 2);
					longitude = m_lonValues.at(m_lonValues.size() - 1) + cellsize * 0.5;
				}
				else {
					longitude = (m_lonValues.at(i - 1) + m_lonValues.at(i)) * 0.5;
				}
				double x, y;
				cs->mapGeoToGrid(longitude, latitude, &x, &y);
				points->InsertNextPoint(x, y, 0);
			}
		}
	}

	points->Modified();
	updateRegionPolyData();
}

void RawDataNetcdf::updateActorSettings()
{
	m_actor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
}

nc_type RawDataNetcdf::getNcType(SolverDefinitionGridRelatedCondition* cond)
{
	if (dynamic_cast<SolverDefinitionGridRelatedIntegerCondition*>(cond) != 0){
		return NC_INT;
	} else if (dynamic_cast<SolverDefinitionGridRelatedRealCondition*>(cond) != 0){
		return NC_DOUBLE;
	}
	return NC_NAT;
}

nc_type RawDataNetcdf::getNcType(SolverDefinitionGridRelatedConditionDimension* dim)
{
	if (dynamic_cast<SolverDefinitionGridRelatedConditionIntegerDimension*>(dim) != 0){
		return NC_INT;
	} else if (dynamic_cast<SolverDefinitionGridRelatedConditionRealDimension*>(dim) != 0){
		return NC_DOUBLE;
	}
	return NC_NAT;
}

int RawDataNetcdf::defineCoords(int ncid, int* xDimId, int* yDimId, int* lonDimId, int* latDimId, int* xVarId, int* yVarId, int* lonVarId, int* latVarId)
{
	int ret;
	QString tmp;

	if (m_coordinateSystemType == XY){
		// define x, y dimensions
		ret = nc_def_dim(ncid, X, static_cast<size_t>(m_xValues.size()), xDimId);
		if (ret != NC_NOERR){return ret;}
		ret = nc_def_dim(ncid, Y, static_cast<size_t>(m_yValues.size()), yDimId);
		if (ret != NC_NOERR){return ret;}

		// define x, y variables
		ret = nc_def_var(ncid, X, NC_DOUBLE, 1, xDimId, xVarId);
		if (ret != NC_NOERR){return ret;}
		ret = nc_def_var(ncid, Y, NC_DOUBLE, 1, yDimId, yVarId);
		if (ret != NC_NOERR){return ret;}

		tmp = "x-coordinate in Cartesian system";
		ret = nc_put_att_text(ncid, *xVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "m";
		ret = nc_put_att_text(ncid, *xVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "X";
		ret = nc_put_att_text(ncid, *xVarId, "axis", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}

		tmp = "y-coordinate in Cartesian system";
		ret = nc_put_att_text(ncid, *yVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "m";
		ret = nc_put_att_text(ncid, *yVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "Y";
		ret = nc_put_att_text(ncid, *yVarId, "axis", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}

		// define lon, lat variables

		int dims[2];
		dims[0] = *yDimId;
		dims[1] = *xDimId;

		ret = nc_def_var(ncid, LON, NC_DOUBLE, 2, &(dims[0]), lonVarId);
		if (ret != NC_NOERR){return ret;}
		ret = nc_def_var(ncid, LAT, NC_DOUBLE, 2, &(dims[0]), latVarId);
		if (ret != NC_NOERR){return ret;}

		tmp = "Longitude";
		ret = nc_put_att_text(ncid, *lonVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "degrees_east";
		ret = nc_put_att_text(ncid, *lonVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "Latitude";
		ret = nc_put_att_text(ncid, *latVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "degrees_north";
		ret = nc_put_att_text(ncid, *latVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		return NC_NOERR;

	} else if (m_coordinateSystemType == LonLat){
		// define lon, lat dimensions
		ret = nc_def_dim(ncid, LON, static_cast<size_t>(m_lonValues.size()), lonDimId);
		if (ret != NC_NOERR){return ret;}
		ret = nc_def_dim(ncid, LAT, static_cast<size_t>(m_latValues.size()), latDimId);
		if (ret != NC_NOERR){return ret;}

		// define lon, lat variables
		ret = nc_def_var(ncid, LON, NC_DOUBLE, 1, lonDimId, lonVarId);
		if (ret != NC_NOERR){return ret;}
		ret = nc_def_var(ncid, LAT, NC_DOUBLE, 1, latDimId, latVarId);
		if (ret != NC_NOERR){return ret;}

		tmp = "Longitude";
		ret = nc_put_att_text(ncid, *lonVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "degrees_east";
		ret = nc_put_att_text(ncid, *lonVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "Latitude";
		ret = nc_put_att_text(ncid, *latVarId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		tmp = "degrees_north";
		ret = nc_put_att_text(ncid, *latVarId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
		if (ret != NC_NOERR){return ret;}
		return NC_NOERR;
	}
	return NC_NOERR;
}

int RawDataNetcdf::defineDimensions(int ncid, QList<int> &dimIds, QList<int>& varIds)
{
	const GridRelatedConditionDimensionsContainer* dims = dimensions();
	int ret;
	for (int i = 0; i < dims->containers().size(); ++i){
		int dimId;
		GridRelatedConditionDimensionContainer* c = dims->containers().at(i);
		size_t len = c->count();
		// time is the special dimension: it is defined as unlimited.
		if (c->definition()->name() == "time"){len = NC_UNLIMITED;}
		ret = nc_def_dim(ncid, iRIC::toStr(c->definition()->name()).c_str(), len, &dimId);
		if (ret != NC_NOERR){return ret;}
		dimIds.append(dimId);
	}
	for (int i = 0; i < dims->containers().size(); ++i){
		int varId;
		GridRelatedConditionDimensionContainer* c = dims->containers().at(i);
		nc_type ncType = getNcType(c->definition());
		int dimId = dimIds.at(i);
		ret = nc_def_var(ncid, iRIC::toStr(c->definition()->name()).c_str(), ncType, 1, &dimId, &varId);
		if (ret != NC_NOERR){return ret;}
		varIds.append(varId);

		// for special dimensions
		QString tmp;
		if (c->definition()->name() == "time"){
			// time is normalized to seconds since 1970-01-01 00:00:00
			tmp = "time of measurement";
			ret = nc_put_att_text(ncid, varId, "long_name", tmp.length(), iRIC::toStr(tmp).c_str());
			if (ret != NC_NOERR){return ret;}
			tmp = "seconds since 1970-01-01 00:00:00";
			ret = nc_put_att_text(ncid, varId, "units", tmp.length(), iRIC::toStr(tmp).c_str());
			if (ret != NC_NOERR){return ret;}
		}
	}
	return NC_NOERR;
}

int RawDataNetcdf::defineValue(int ncid, int xId, int yId, const QList<int>& dimIds, int* varId)
{
	int ret;
	int ndims = dimensions()->containers().size() + 2;
	int* dimids = new int[ndims];
	size_t* chunksizes = new size_t[ndims];

	// reverse order!
	for (int i = 0; i < ndims - 2; ++i){
		*(dimids + ndims - 3 - i) = dimIds.at(i);
		*(chunksizes + ndims - 3 - i) = 1;
	}
	*(dimids + ndims - 2) = yId;
	*(dimids + ndims - 1) = xId;
	if (m_coordinateSystemType == XY){
		*(chunksizes + ndims - 2) = m_yValues.size();
		*(chunksizes + ndims - 1) = m_xValues.size();
	} else if (m_coordinateSystemType == LonLat){
		*(chunksizes + ndims - 2) = m_latValues.size();
		*(chunksizes + ndims - 1) = m_lonValues.size();
	}
	ret = nc_def_var(ncid, VALUE, getNcType(gridRelatedCondition()), ndims, dimids, varId);
	ret = nc_def_var_deflate(ncid, *varId, 0, 1, 2);
	ret = nc_def_var_chunking(ncid, *varId, NC_CHUNKED, chunksizes);

	delete chunksizes;
	delete dimids;
	if (ret != NC_NOERR){return ret;}

	return NC_NOERR;
}

int RawDataNetcdf::outputCoords(int ncid, int xId, int yId, int lonId, int latId)
{
	if (m_coordinateSystemType == XY){
		double* xs = new double[m_xValues.size()];
		double* ys = new double[m_yValues.size()];
		int ret;

		for (int i = 0; i < m_xValues.size(); ++i){
			*(xs + i) = m_xValues.at(i);
		}
		for (int i = 0; i < m_yValues.size(); ++i){
			*(ys + i) = m_yValues.at(i);
		}
		ret = nc_put_var_double(ncid, xId, xs);
		if (ret != NC_NOERR){return ret;}
		ret = nc_put_var_double(ncid, yId, ys);
		if (ret != NC_NOERR){ return ret; }

		delete xs;
		delete ys;
	}

	if (m_coordinateSystemType == XY || m_coordinateSystemType == LonLat){
		double* lons = new double[m_lonValues.size()];
		double* lats = new double[m_latValues.size()];
		int ret;

		for (int i = 0; i < m_lonValues.size(); ++i){
			*(lons + i) = m_lonValues.at(i);
		}
		for (int i = 0; i < m_latValues.size(); ++i){
			*(lats + i) = m_latValues.at(i);
		}
		ret = nc_put_var_double(ncid, lonId, lons);
		if (ret != NC_NOERR){return ret;}
		ret = nc_put_var_double(ncid, latId, lats);
		if (ret != NC_NOERR){ return ret; }

		delete lons;
		delete lats;
	}
	return NC_NOERR;
}

int RawDataNetcdf::outputDimensions(int ncid, const QList<int>& varIds)
{
	const GridRelatedConditionDimensionsContainer* dims = dimensions();
	for (int i = 0; i < dims->containers().size(); ++i){
		int ret;
		int varId = varIds.at(i);
		GridRelatedConditionDimensionContainer* c = dims->containers().at(i);
		if (dynamic_cast<GridRelatedConditionDimensionIntegerContainer*>(c) != 0){
			GridRelatedConditionDimensionIntegerContainer* c2 =
					dynamic_cast<GridRelatedConditionDimensionIntegerContainer*>(c);
			int* vals = new int[c->count()];
			const QList<int> listVals = c2->values();
			for (int j = 0; j < listVals.size(); ++j){
				*(vals + j) = listVals.at(j);
			}
			size_t start = 0;
			size_t len = listVals.size();
			ret = nc_put_vara_int(ncid, varId, &start, &len, vals);
			delete vals;
		} else if (dynamic_cast<GridRelatedConditionDimensionRealContainer*>(c) != 0){
			GridRelatedConditionDimensionRealContainer* c2 =
					dynamic_cast<GridRelatedConditionDimensionRealContainer*>(c);
			double* vals = new double[c->count()];
			const QList<double> listVals = c2->values();
			for (int j = 0; j < listVals.size(); ++j){
				*(vals + j) = listVals.at(j);
			}
			size_t start = 0;
			size_t len = listVals.size();
			ret = nc_put_vara_double(ncid, varId, &start, &len, vals);
			delete vals;
		}
		if (ret != NC_NOERR){return ret;}
	}
	return NC_NOERR;
}


void RawDataNetcdf::handleDimensionCurrentIndexChange(int oldIndex, int newIndex)
{
	doHandleDimensionCurrentIndexChange(oldIndex, newIndex);
}

void RawDataNetcdf::handleDimensionValuesChange(const QList<QVariant>& before, const QList<QVariant>& after)
{
	GridRelatedConditionDimensionContainer* dim =
			dynamic_cast<GridRelatedConditionDimensionContainer*> (sender());
	doHandleDimensionValuesChange(dim, before, after);
}

int RawDataNetcdf::getValueVarId(int ncid, int* varId)
{
	return nc_inq_varid(ncid, VALUE, varId);
}

void RawDataNetcdf::viewOperationEndedGlobal(PreProcessorGraphicsViewInterface* v)
{
	double xmin, xmax, ymin, ymax;
	v->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
	updateSimpifiedGrid(xmin, xmax, ymin, ymax);
}

void RawDataNetcdf::updateSimpifiedGrid(double xmin, double xmax, double ymin, double ymax)
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
	double* cv = m_grid->GetPoint(vid);
	if (*cv < xmin || *cv > xmax || *(cv + 1) < ymin || *(cv + 1) > ymax){
		m_simplifiedGrid = vtkSmartPointer<vtkStructuredGrid>::New();
		m_threshold->SetInputData(m_simplifiedGrid);
		m_threshold->Modified();
		return;
	}
	int dimI, dimJ;

	if (m_coordinateSystemType == RawDataNetcdf::XY){
		dimI = m_xValues.size();
		dimJ = m_yValues.size();
	} else if (m_coordinateSystemType == LonLat){
		dimI = m_lonValues.size();
		dimJ = m_latValues.size();
	}

	RectRegion region(xmin, xmax, ymin, ymax);
	unsigned int centerI, centerJ;
	getIJIndex(vid, &centerI, &centerJ);

	int lineLimitIMin, lineLimitIMax, lineLimitJMin, lineLimitJMax;
	double tmpv[3];

	// test I = 0
	m_grid->GetPoint(vertexIndex(0, centerJ), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])){
		lineLimitIMin = 0;
	} else {
		lineLimitIMin = lineLimitI(centerJ, centerI, 0, dimI, dimJ, region);
	}
	// test I = imax
	m_grid->GetPoint(vertexIndex(dimI - 1, centerJ), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])){
		lineLimitIMax = dimI - 1;
	} else {
		lineLimitIMax = lineLimitI(centerJ, centerI, dimI - 1, dimI, dimJ, region);
	}
	// test J = 0
	m_grid->GetPoint(vertexIndex(centerI, 0), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])){
		lineLimitJMin = 0;
	} else {
		lineLimitJMin = lineLimitJ(centerI, centerJ, 0, dimI, dimJ, region);
	}
	// test J = jmax
	m_grid->GetPoint(vertexIndex(centerI, dimJ - 1), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])){
		lineLimitJMax = dimJ - 1;
	} else {
		lineLimitJMax = lineLimitJ(centerI, centerJ, dimJ - 1, dimI, dimJ, region);
	}

	int lineLimitIMin2, lineLimitIMax2, lineLimitJMin2, lineLimitJMax2;

	// test I min direction
	if (lineLimitIMin == 0){
		lineLimitIMin2 = 0;
	} else {
		if (lineAtIIntersect(0, dimI, dimJ, region)){
			lineLimitIMin2 = 0;
		} else {
			lineLimitIMin2 = lineLimitI2(lineLimitIMin, 0, dimI, dimJ, region);
		}
	}
	// test I max direction
	if (lineLimitIMax == dimI - 1){
		lineLimitIMax2 = dimI - 1;
	} else {
		if (lineAtIIntersect(dimI - 1, dimI, dimJ, region)){
			lineLimitIMax2 = dimI - 1;
		} else {
			lineLimitIMax2 = lineLimitI2(lineLimitIMax, dimI - 1, dimI, dimJ, region);
		}
	}

	// test J min direction
	if (lineLimitJMin == 0){
		lineLimitJMin2 = 0;
	} else {
		if (lineAtJIntersect(0, dimI, dimJ, region)){
			lineLimitJMin2 = 0;
		} else {
			lineLimitJMin2 = lineLimitJ2(lineLimitJMin, 0, dimI, dimJ, region);
		}
	}
	// test J max direction
	if (lineLimitJMax == dimJ - 1){
		lineLimitJMax2 = dimJ - 1;
	} else {
		if (lineAtJIntersect(dimJ - 1, dimI, dimJ, region)){
			lineLimitJMax2 = dimJ - 1;
		} else {
			lineLimitJMax2 = lineLimitI2(lineLimitJMax, dimJ - 1, dimI, dimJ, region);
		}
	}

	vtkSmartPointer<vtkExtractGrid> exGrid = vtkSmartPointer<vtkExtractGrid>::New();
	exGrid->SetVOI(lineLimitIMin2, lineLimitIMax2, lineLimitJMin2, lineLimitJMax2, 0, 0);
	exGrid->SetInputData(m_grid);
	exGrid->Update();
	m_simplifiedGrid = exGrid->GetOutput();
	int exRate = 1;
	while (m_simplifiedGrid->GetNumberOfCells() > MAX_DRAWCELLCOUNT){
		exRate *= 2;
		exGrid->SetSampleRate(exRate, exRate, 1);
		exGrid->Update();
		m_simplifiedGrid = exGrid->GetOutput();
		m_isMasked = true;
	}
	m_threshold->SetInputData(m_simplifiedGrid);
	m_threshold->Modified();
}

void RawDataNetcdf::getIJIndex(vtkIdType id, unsigned int* i, unsigned int* j) const
{
	int dimI;
	if (m_coordinateSystemType == XY){
		dimI = m_xValues.size() + 1;
	} else if (m_coordinateSystemType == LonLat){
		dimI = m_lonValues.size() + 1;
	}

	*i = id % dimI;
	*j = id / dimI;
}

unsigned int RawDataNetcdf::vertexIndex(unsigned int i, unsigned int j) const
{
	int dimI;
	if (m_coordinateSystemType == XY){
		dimI = m_xValues.size() + 1;
	} else if (m_coordinateSystemType == LonLat){
		dimI = m_lonValues.size() + 1;
	}
	return dimI * j + i;
}

int RawDataNetcdf::lineLimitI(int j, int iIn, int iOut, int dimI, int dimJ, const RectRegion& region) const
{
	if (qAbs(iOut - iIn) == 1){
		return iIn;
	}
	int i = (iIn + iOut) / 2;
	double tmpv[3];
	m_grid->GetPoint(vertexIndex(i, j), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])){
		return lineLimitI(j, i, iOut, dimI, dimJ, region);
	} else {
		return lineLimitI(j, iIn, i, dimI, dimJ, region);
	}
}

int RawDataNetcdf::lineLimitJ(int i, int jIn, int jOut, int dimI, int dimJ, const RectRegion& region) const
{
	if (qAbs(jOut - jIn) == 1){
		return jIn;
	}
	int j = (jIn + jOut) / 2;
	double tmpv[3];
	m_grid->GetPoint(vertexIndex(i, j), tmpv);
	if (region.pointIsInside(tmpv[0], tmpv[1])){
		return lineLimitJ(i, j, jOut, dimI, dimJ, region);
	} else {
		return lineLimitJ(i, jIn, j, dimI, dimJ, region);
	}
}

int RawDataNetcdf::lineLimitI2(int iIn, int iOut, int dimI, int dimJ, const RectRegion& region) const
{
	if (qAbs(iOut - iIn) == 1){
		return iIn;
	}
	int i = (iIn + iOut) / 2;
	if (lineAtIIntersect(i, dimI, dimJ, region)){
		return lineLimitI2(i, iOut, dimI, dimJ, region);
	} else {
		return lineLimitI2(iIn, i, dimI, dimJ, region);
	}
}

int RawDataNetcdf::lineLimitJ2(int jIn, int jOut, int dimI, int dimJ, const RectRegion& region) const
{
	if (qAbs(jOut - jIn) == 1){
		return jIn;
	}
	int j = (jIn + jOut) / 2;
	if (lineAtJIntersect(j, dimI, dimJ, region)){
		return lineLimitJ2(j, jOut, dimI, dimJ, region);
	} else {
		return lineLimitJ2(jIn, j, dimI, dimJ, region);
	}
}

bool RawDataNetcdf::lineAtIIntersect(int i, int dimI, int dimJ, const RectRegion& region) const
{
	QPointF p1, p2;
	double tmpv[3];
	m_grid->GetPoint(vertexIndex(i, 0), tmpv);
	p1 = QPointF(tmpv[0], tmpv[1]);
	for (int j = 1; j < dimJ; ++j){
		m_grid->GetPoint(vertexIndex(i, j), tmpv);
		p2 = QPointF(tmpv[0], tmpv[1]);
		QLineF line(p1, p2);
		if (region.intersect(line)){return true;}
	}
	return false;
}

bool RawDataNetcdf::lineAtJIntersect(int j, int dimI, int dimJ, const RectRegion& region) const
{
	QPointF p1, p2;
	double tmpv[3];
	m_grid->GetPoint(vertexIndex(0, j), tmpv);
	p1 = QPointF(tmpv[0], tmpv[1]);
	for (int i = 1; i < dimI; ++i){
		m_grid->GetPoint(vertexIndex(i, j), tmpv);
		p2 = QPointF(tmpv[0], tmpv[1]);
		QLineF line(p1, p2);
		if (region.intersect(line)){return true;}
	}
	return false;
}

void RawDataNetcdf::updateRegionPolyData()
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

int RawDataNetcdf::xSize() const
{
	if (m_coordinateSystemType == XY){
		return m_xValues.size();
	} else if (m_coordinateSystemType == LonLat){
		return m_lonValues.size();
	}
	return 0;
}
int RawDataNetcdf::ySize() const
{
	if (m_coordinateSystemType == XY){
		return m_yValues.size();
	} else if (m_coordinateSystemType == LonLat){
		return m_lonValues.size();
	}
	return 0;
}
