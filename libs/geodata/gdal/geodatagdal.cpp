#include "geodatagdal.h"
#include "geodatagdalcolorsettingdialog.h"
#include "geodatagdalproxy.h"
#include "private/geodatagdal_impl.h"
#include "public/geodatagdal_displaysettingwidget.h"

#include <cs/coordinatesystem.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorgridtypedataitemi.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/gridcond/base/gridattributedimensionintegercontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionrealcontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <guicore/solverdef/solverdefinitiongridattributeinteger.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegerdimension.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <guicore/solverdef/solverdefinitiongridattributerealdimension.h>
#include <misc/modifycommanddialog.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <misc/zdepthrange.h>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkExtractGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersionMacros.h>

#include <QDir>
#include <QDomElement>
#include <QDomNode>
#include <QLineF>
#include <QMessageBox>
#include <QXmlStreamWriter>

#include <gdal_priv.h>
#include <gdal_utils.h>
#include <ogr_spatialref.h>
#include <netcdf.h>

#include <vector>

#define VALUE "value"
#define X "x"
#define Y "y"

#define LON "lon"
#define LAT "lat"

const int GeoDataGdal::MAX_DRAWCELLCOUNT = 200000;

namespace {

	int calcIndex(int i, int j, int isize)
	{
		return i + j * isize;
	}

} // namespace

bool GeoDataGdal::RectRegion::pointIsInside(double x, double y) const
{
	if (x < xMin) {return false;}
	if (x > xMax) {return false;}
	if (y < yMin) {return false;}
	if (y > yMax) {return false;}
	return true;
}

bool GeoDataGdal::RectRegion::intersect(const QLineF& line) const
{
	bool isInside1 = pointIsInside(line.x1(), line.y1());
	bool isInside2 = pointIsInside(line.x2(), line.y2());
	return (isInside1 != isInside2);
}

GeoDataGdal::GeoDataGdal(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoDataWithSingleMapper {d, creator, att},
	impl {new Impl {}}
{
	m_grid = vtkSmartPointer<vtkStructuredGrid>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	m_grid->SetPoints(points);
}

GeoDataGdal::~GeoDataGdal()
{
	if (creator()->isReadOnly()) {
		QFile f(filename());
		if (f.exists()) {
			f.remove();
		}
	}

	actorCollection()->RemoveItem(m_regionActor);
	actorCollection()->RemoveItem(m_actor);
	renderer()->RemoveActor(m_regionActor);
	renderer()->RemoveActor(m_actor);

	delete impl;
}

const std::vector<double>& GeoDataGdal::xValues() const
{
	return impl->m_xValues;
}

const std::vector<double>& GeoDataGdal::yValues() const
{
	return impl->m_yValues;
}

std::vector<double> GeoDataGdal::xValues()
{
	return impl->m_xValues;
}

std::vector<double> GeoDataGdal::yValues()
{
	return impl->m_yValues;
}

bool GeoDataGdal::geoTransformExists() const
{
	return impl->m_geoTransformExists;
}

double* GeoDataGdal::geoTransform()
{
	return &(impl->m_geoTransform[0]);
}

void GeoDataGdal::setGeoTransform(double* t)
{
	impl->m_geoTransformExists = true;

	for (int i = 0; i < 6; ++i) {
		impl->m_geoTransform[i] = *(t + i);
	}
}

bool GeoDataGdal::baseAndResolutionExists() const
{
	return impl->m_baseAndResolutionExists;
}

void GeoDataGdal::setBaseAndResolution(double base, double resolution)
{
	impl->m_baseAndResolutionExists = true;
	impl->m_base = base;
	impl->m_resolution = resolution;
}

double GeoDataGdal::base() const
{
	return impl->m_base;
}

double GeoDataGdal::resolution() const
{
	return impl->m_resolution;
}


vtkStructuredGrid* GeoDataGdal::grid() const
{
	return m_grid;
}

void GeoDataGdal::setupActors()
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

	m_actor = vtkSmartPointer<vtkActor>::New();
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

	updateActorSetting();
}

void GeoDataGdal::loadExternalData(const QString& filename)
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

	int xVarId, yVarId;

	// try to find x, y
	ret = nc_inq_varid(ncid, X, &xVarId);
	ret = nc_inq_varid(ncid, Y, &yVarId);

	// load X, Y values
	int xDimId, yDimId;
	ret = nc_inq_dimid(ncid, X, &xDimId);
	ret = nc_inq_dimid(ncid, Y, &yDimId);

	size_t xSize, ySize;
	ret = nc_inq_dimlen(ncid, xDimId, &xSize);
	ret = nc_inq_dimlen(ncid, yDimId, &ySize);

	impl->m_xValues.assign(xSize, 0);
	impl->m_yValues.assign(ySize, 0);

	ret = nc_get_var_double(ncid, xVarId, impl->m_xValues.data());
	ret = nc_get_var_double(ncid, yVarId, impl->m_yValues.data());

	// ------------------------
	// Load dimension values
	// Only for checking.
	// ------------------------

	GridAttributeDimensionsContainer* dims = dimensions();
	for (int i = 0; i < static_cast<int> (dims->containers().size()); ++i) {
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
	loadRasterData(0);
}

void GeoDataGdal::saveExternalData(const QString& /*filename*/)
{

}

void GeoDataGdal::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataGdal::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_displaySetting.load(node);
	loadGeoTransform(node);
	loadBaseAndResolution(node);
}

void GeoDataGdal::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_displaySetting.save(writer);
	saveGeoTransform(writer);
	saveBaseAndResolution(writer);
}

void GeoDataGdal::doApplyOffset(double x, double y)
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

	if (m_simplifiedGrid != nullptr) {
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
	}

	updateRegionPolyData();
}

void GeoDataGdal::updateShapeData()
{
	auto offset = projectData()->mainfile()->offset();

	vtkPoints* points = m_grid->GetPoints();
	m_grid->SetDimensions(static_cast<int> (impl->m_xValues.size()) + 1, static_cast<int> (impl->m_yValues.size()) + 1, 1);
	points->Initialize();
	points->Allocate((impl->m_xValues.size() + 1) * (impl->m_yValues.size() + 1));

	double dx = impl->m_xValues.at(1) - impl->m_xValues.at(0);
	double dy = impl->m_yValues.at(1) - impl->m_yValues.at(0);
	double x0 = impl->m_xValues.at(0) - dx * 0.5;
	double y0 = impl->m_yValues.at(0) - dy * 0.5;

	for (int j = 0; j < static_cast<int> (impl->m_yValues.size()) + 1; ++j) {
		double y = y0 + dy * j;
		for (int i = 0; i < static_cast<int> (impl->m_xValues.size()) + 1; ++i) {
			double x = x0 + dx * i;
			points->InsertNextPoint(x - offset.x(), y - offset.y(), 0);
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

nc_type GeoDataGdal::getNcType(SolverDefinitionGridAttribute* cond)
{
	if (dynamic_cast<SolverDefinitionGridAttributeInteger*>(cond) != 0) {
		return NC_INT;
	} else if (dynamic_cast<SolverDefinitionGridAttributeReal*>(cond) != 0) {
		return NC_DOUBLE;
	}
	return NC_NAT;
}

nc_type GeoDataGdal::getNcType(SolverDefinitionGridAttributeDimension* dim)
{
	if (dynamic_cast<SolverDefinitionGridAttributeIntegerDimension*>(dim) != 0) {
		return NC_INT;
	} else if (dynamic_cast<SolverDefinitionGridAttributeRealDimension*>(dim) != 0) {
		return NC_DOUBLE;
	}
	return NC_NAT;
}

int GeoDataGdal::defineCoords(int ncid, int* xDimId, int* yDimId, int* xVarId, int* yVarId)
{
	int ret;
	QString tmp;

	// define x, y dimensions
	ret = nc_def_dim(ncid, X, static_cast<size_t>(impl->m_xValues.size()), xDimId);
	if (ret != NC_NOERR) {return ret;}
	ret = nc_def_dim(ncid, Y, static_cast<size_t>(impl->m_yValues.size()), yDimId);
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

	return NC_NOERR;
}

int GeoDataGdal::defineDimensions(int ncid, std::vector<int>* dimIds, std::vector<int>* varIds)
{
	const GridAttributeDimensionsContainer* dims = dimensions();
	int ret;
	for (int i = 0; i < static_cast<int> (dims->containers().size()); ++i) {
		int dimId;
		GridAttributeDimensionContainer* c = dims->containers().at(i);
		size_t len = c->count();
		// time is the special dimension: it is defined as unlimited.
		if (c->definition()->name() == "Time") {len = NC_UNLIMITED;}
		ret = nc_def_dim(ncid, c->definition()->name().c_str(), len, &dimId);
		if (ret != NC_NOERR) {return ret;}
		dimIds->push_back(dimId);
	}
	for (int i = 0; i < static_cast<int> (dims->containers().size()); ++i) {
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

int GeoDataGdal::defineValue(int ncid, int xId, int yId, const std::vector<int> &dimIds, int* varId)
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
	chunksizes[ndims - 2] = impl->m_yValues.size();
	chunksizes[ndims - 1] = impl->m_xValues.size();
	ret = nc_def_var(ncid, VALUE, getNcType(gridAttribute()), ndims, dimids.data(), varId);
	ret = nc_def_var_deflate(ncid, *varId, 0, 1, 2);
	ret = nc_def_var_chunking(ncid, *varId, NC_CHUNKED, chunksizes.data());

	if (ret != NC_NOERR) {return ret;}

	return NC_NOERR;
}

int GeoDataGdal::outputCoords(int ncid, int xId, int yId)
{
	std::vector<double> xs(impl->m_xValues.size());
	std::vector<double> ys(impl->m_yValues.size());
	int ret = NC_NOERR;

	for (int i = 0; i < static_cast<int> (impl->m_xValues.size()); ++i) {
		xs[i] = impl->m_xValues.at(i);
	}
	for (int i = 0; i < static_cast<int> (impl->m_yValues.size()); ++i) {
		ys[i] = impl->m_yValues.at(i);
	}
	ret = nc_put_var_double(ncid, xId, xs.data());
	if (ret != NC_NOERR) {return ret;}
	ret = nc_put_var_double(ncid, yId, ys.data());
	if (ret != NC_NOERR) {return ret;}

	return NC_NOERR;
}

int GeoDataGdal::outputDimensions(int ncid, const std::vector<int> &varIds)
{
	const GridAttributeDimensionsContainer* dims = dimensions();
	for (int i = 0; i < static_cast<int> (dims->containers().size()); ++i) {
		int ret = NC_NOERR;
		int varId = varIds.at(i);
		GridAttributeDimensionContainer* c = dims->containers().at(i);
		if (dynamic_cast<GridAttributeDimensionIntegerContainer*>(c) != nullptr) {
			GridAttributeDimensionIntegerContainer* c2 =
				dynamic_cast<GridAttributeDimensionIntegerContainer*>(c);
			std::vector<int> vals(c->count());
			const std::vector<int>& listVals = c2->values();
			for (int j = 0; j < static_cast<int> (listVals.size()); ++j) {
				vals[j] = listVals.at(j);
			}
			size_t start = 0;
			size_t len = listVals.size();
			ret = nc_put_vara_int(ncid, varId, &start, &len, vals.data());
		} else if (dynamic_cast<GridAttributeDimensionRealContainer*>(c) != nullptr) {
			GridAttributeDimensionRealContainer* c2 =
				dynamic_cast<GridAttributeDimensionRealContainer*>(c);
			std::vector<double> vals(c->count());
			const std::vector<double>& listVals = c2->values();
			for (int j = 0; j < static_cast<int> (listVals.size()); ++j) {
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

void GeoDataGdal::handleDimensionCurrentIndexChange(int oldIndex, int newIndex)
{
	doHandleDimensionCurrentIndexChange(oldIndex, newIndex);
}

void GeoDataGdal::handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after)
{
	GridAttributeDimensionContainer* dim =
		dynamic_cast<GridAttributeDimensionContainer*>(sender());
	doHandleDimensionValuesChange(dim, before, after);
}

int GeoDataGdal::getValueVarId(int ncid, int* varId)
{
	return nc_inq_varid(ncid, VALUE, varId);
}

void GeoDataGdal::doUpdateActorSetting()
{
	// color
	m_actor->GetProperty()->SetColor(impl->m_displaySetting.color);

	// opacity
	m_actor->GetProperty()->SetOpacity(impl->m_displaySetting.opacity);

	// mapping
	bool scalarVisibility = true;
	if (impl->m_displaySetting.mapping == DisplaySetting::Mapping::Arbitrary) {
		scalarVisibility = false;
	}

	if (scalarVisibility) {
		vtkMapper* mapper = nullptr;
		auto cs = colorMapSettingContainer();

		m_threshold->Update();
		mapper = cs->buildCellDataMapper(m_threshold->GetOutput(), false);
		m_actor->SetMapper(mapper);
		mapper->Delete();

	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto geometry = vtkSmartPointer<vtkGeometryFilter>::New();
		geometry->SetInputConnection(m_threshold->GetOutputPort());
		mapper->SetInputConnection(geometry->GetOutputPort());
		m_actor->SetMapper(mapper);
		mapper->Delete();
	}

	emit updateActorSettingExecuted();
}

void GeoDataGdal::viewOperationEndedGlobal(PreProcessorGraphicsViewI* v)
{
	double xmin, xmax, ymin, ymax;
	v->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
	updateSimpifiedGrid(xmin, xmax, ymin, ymax);
}

void GeoDataGdal::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataGdal::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
	m_regionActor->SetPosition(0, 0, range.min());
}

bool GeoDataGdal::getValueAt(double x, double y, double* value)
{
	vtkIdType cellid;
	double pcoords[4];
	double weights[4];
	int subid;

	double point[3] = {x, y, 0};

	cellid = grid()->FindCell(point, 0, 0, 1e-4, subid, pcoords, weights);
	if (cellid < 0) {return false;}

	vtkDataArray* da = m_grid->GetCellData()->GetArray("values");
	*value = da->GetTuple1(cellid);

	return true;
}

void GeoDataGdal::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataGdal::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoDataGroupDataItem()->condition()->name(), parent);
	auto widget = new DisplaySettingWidget(dialog);

	if (geoDataGroupDataItem()->condition()->isReferenceInformation()) {
		widget->setIsReferenceInformation(true);
	} else {
		auto colorMapWidget = geoDataGroupDataItem()->condition()->createColorMapSettingEditWidget(widget);
		auto colormap = geoDataDataItem()->colorMapSettingContainer();
		colorMapWidget->setSetting(colormap);
		auto colorMapWidget2 = new ColorMapSettingEditWidgetWithImportExportButton(colorMapWidget, widget);

		widget->setColorMapWidget(colorMapWidget2);
	}
	widget->setSetting(&impl->m_displaySetting);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Raster Data Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataGdal::applyColorMapSetting()
{
	updateActorSetting();
}

GeoDataProxy* GeoDataGdal::getProxy()
{
	return new GeoDataGdalProxy(this);
}

void GeoDataGdal::buildWarpMatrix(int srcISize, int srcJSize, double* srcGeoTransform, CoordinateSystem* srcCs, CoordinateSystem* tgtCs,
																	int* tgtISize, int* tgtJSize, double* tgtGeoTransform, std::vector<int>* matrix)
{
	if (srcCs == tgtCs) {
		*tgtISize = srcISize;
		*tgtJSize = srcJSize;
		matrix->assign(srcISize * srcJSize, 0);
		for (int i = 0; i < matrix->size(); ++i) {
			(*matrix)[i] = i;
		}
		for (int i = 0; i < 6; ++i) {
			*(tgtGeoTransform + i) = *(srcGeoTransform + i);
		}

		return;
	}

	GDALAllRegister();

	char** papszOptions = nullptr;
	auto driverManager = GetGDALDriverManager();
	GDALDriver* memDriver = driverManager->GetDriverByName("MEM");
	auto dataset = memDriver->Create("dummy", srcISize, srcJSize, 1, GDT_UInt32, papszOptions);
	dataset->SetGeoTransform(srcGeoTransform);
	OGRSpatialReference srcSRC;
	srcSRC.importFromProj4(iRIC::toStr(srcCs->proj4PlaneStr()).c_str());
	char* srcWktStr;
	srcSRC.exportToWkt(&srcWktStr);
	dataset->SetProjection(srcWktStr);

	auto band = dataset->GetRasterBand(1);
	std::vector<int> indices;
	indices.assign(srcISize * srcJSize, 0);
	for (int i = 0; i < indices.size(); ++i) {
		indices[i] = i + 1;
	}
	GDALRasterIO(band, GF_Write, 0, 0, srcISize, srcJSize, indices.data(), srcISize, srcJSize, GDT_UInt32, 0, 0);

	std::vector<char*> args;
	char t_src1[] = "-t_srs";
	std::string t_src2 = iRIC::toStr(tgtCs->proj4PlaneStr());
	char of_1[] = "-of";
	char of_2[] = "GTiff";
	args.push_back(t_src1);
	args.push_back(const_cast<char*>(t_src2.data()));
	args.push_back(of_1);
	args.push_back(of_2);
	args.push_back(nullptr);
	int error;
	auto options = GDALWarpAppOptionsNew(args.data(), nullptr);
	auto newDataset = reinterpret_cast<GDALDataset*> (GDALWarp("tmp.tif", nullptr, 1, reinterpret_cast<GDALDatasetH*>(&dataset), options, &error));
	*tgtISize = newDataset->GetRasterXSize();
	*tgtJSize = newDataset->GetRasterYSize();
	newDataset->GetGeoTransform(tgtGeoTransform);

	auto newBand = newDataset->GetRasterBand(1);
	matrix->assign(*tgtISize * *tgtJSize, 0);
	GDALRasterIO(newBand, GF_Read, 0, 0, *tgtISize, *tgtJSize, matrix->data(), *tgtISize, *tgtJSize, GDT_UInt32, 0, 0);
	for (int i = 0; i < matrix->size(); ++i) {
		(*matrix)[i] -= 1;
	}

	GDALWarpAppOptionsFree(options);
	GDALClose(newDataset);
	GDALClose(dataset);

	QFile::remove("tmp.tif");
}

void GeoDataGdal::updateSimpifiedGrid(double xmin, double xmax, double ymin, double ymax)
{
	if (! dataLoaded()) {return;}

	double dx = impl->m_xValues.at(1) - impl->m_xValues.at(0);
	double dy = impl->m_yValues.at(1) - impl->m_yValues.at(0);
	double x0 = impl->m_xValues.at(0) - dx * 0.5;
	double y0 = impl->m_yValues.at(0) - dy * 0.5;
	double x1 = x0 + dx * impl->m_xValues.size();
	double y1 = y0 + dy * impl->m_yValues.size();

	if (x1 < xmin || x0 > xmax || y1 < ymin || y0 > ymax) {
		m_simplifiedGrid = vtkSmartPointer<vtkStructuredGrid>::New();
		m_threshold->SetInputData(m_simplifiedGrid);
		m_threshold->Modified();
		return;
	}

	int dimI = static_cast<int> (impl->m_xValues.size()) + 1;
	int dimJ = static_cast<int> (impl->m_yValues.size()) + 1;

	int iMin = std::max(static_cast<int> ((xmin - x0) / dx), 0);
	int iMax = std::min(static_cast<int> ((xmax - x0) / dx) + 1, dimI);
	int jMin = std::max(static_cast<int> ((ymin - y0) / dy), 0);
	int jMax = std::min(static_cast<int> ((ymax - y0) / dy) + 1, dimJ);

	auto exGrid = vtkSmartPointer<vtkExtractGrid>::New();
	exGrid->SetVOI(iMin, iMax, jMin, jMax, 0, 0);
	exGrid->SetInputData(m_grid);
	exGrid->Update();
	m_simplifiedGrid = exGrid->GetOutput();
	int exRate = 1;
	while (m_simplifiedGrid->GetNumberOfCells() > MAX_DRAWCELLCOUNT) {
		exRate *= 2;
		exGrid->SetSampleRate(exRate, exRate, 1);
		exGrid->Update();
		m_simplifiedGrid = exGrid->GetOutput();
	}
	m_threshold->SetInputData(m_simplifiedGrid);
	m_threshold->Modified();

	updateActorSetting();
}

void GeoDataGdal::getIJIndex(vtkIdType id, unsigned int* i, unsigned int* j) const
{
	int dimI = static_cast<int> (impl->m_xValues.size()) + 1;

	*i = id % dimI;
	*j = id / dimI;
}

unsigned int GeoDataGdal::vertexIndex(unsigned int i, unsigned int j) const
{
	int dimI = static_cast<int> (impl->m_xValues.size()) + 1;
	return dimI * j + i;
}

void GeoDataGdal::updateRegionPolyData()
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

int GeoDataGdal::xSize() const
{
	return static_cast<int> (impl->m_xValues.size());
}

int GeoDataGdal::ySize() const
{
	return static_cast<int> (impl->m_yValues.size());
}

void GeoDataGdal::loadGeoTransform(const QDomNode& node)
{
	impl->m_geoTransformExists = false;

	for (int i = 0; i < 6; ++i) {
		impl->m_geoTransform[i] = iRIC::getDoubleAttribute(node, QString("geotransform%1").arg(i + 1));
		impl->m_geoTransformExists = impl->m_geoTransformExists || (impl->m_geoTransform[i] != 0);
	}
}

void GeoDataGdal::saveGeoTransform(QXmlStreamWriter& writer)
{
	if (!impl->m_geoTransformExists) {return;}

	for (int i = 0; i < 6; ++i) {
		iRIC::setDoubleAttribute(writer, QString("geotransform%1").arg(i + 1), impl->m_geoTransform[i]);
	}
}

void GeoDataGdal::loadBaseAndResolution(const QDomNode& node)
{
	impl->m_base = iRIC::getDoubleAttribute(node, "base");
	impl->m_resolution = iRIC::getDoubleAttribute(node, "resolution");

	impl->m_baseAndResolutionExists = (impl->m_base != 0 || impl->m_resolution != 0);
}

void GeoDataGdal::saveBaseAndResolution(QXmlStreamWriter& writer)
{
	if (!impl->m_baseAndResolutionExists) {return;}

	iRIC::setDoubleAttribute(writer, "base", impl->m_base);
	iRIC::setDoubleAttribute(writer, "resolution", impl->m_resolution);
}
