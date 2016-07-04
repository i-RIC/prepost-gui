%module iric
%include "std_string.i"
%include "std_vector.i"

namespace std {
	%template(IntVector) vector<int>;
}

%{
#include "application.h"
#include "calculationresult.h"
#include "geodata.h"
#include "geodatagroup.h"
#include "geodataimporter.h"
#include "gridtype.h"
#include "gridzone.h"
#include "pre.h"
#include "solver.h"
%}

namespace iRICPython {

class Application;
class CalculationResult;
class GeoData;
class GeoDataGroup;
class GeoDataImporter;
class GridType;
class GridZone;
class Pre;
class Solver;

class Application
{
public:
	static void init(const char* path);

	Application();
	~Application();

	bool openProject(const char* filename);
	bool saveProject();
	bool saveProjectAs(const char* filename);
	bool closeProject();

	Pre* pre() const;
	Solver* solver() const;
	CalculationResult* calcResult() const;
};

class CalculationResult
{
public:
	enum class ExportType {
		ESRIShape,
		CSV,
		VtkBinary,
		vtkAscii
	};

	CalculationResult(const Application* app);

	int length() const;
	void exportToFolder(const std::string& folderName, const std::string& prefix, const std::vector<int>& steps, ExportType type);
};

class GeoData
{
public:
	GeoData(PreProcessorGeoDataDataItemInterface* dataItem);

	PreProcessorGeoDataDataItemInterface* geoDataItem() const;
};

class GeoDataGroup
{
public:
	GeoDataGroup(PreProcessorGeoDataGroupDataItem* item);
	~GeoDataGroup();

	std::string name() const;

	GeoDataImporter* importer(const std::string& name) const;
	void add(GeoData* data);
};

class GeoDataImporter
{
public:
	GeoDataImporter(::GeoDataImporter* importer, SolverDefinitionGridAttribute* att, PreProcessorGeoDataGroupDataItemInterface *groupItem);

	GeoData* importGeoData(const std::string& filename);
};

class GridType
{
public:
	GridType(const PreProcessorGridTypeDataItem* gtItem);
	~GridType();

	std::string name() const;
	bool isPrimary() const;

	GeoDataGroup* geoDataGroup(const char* name) const;
	std::vector<GeoDataGroup*> geoDataGroups() const;

	void mapAllGeoData();
	void mapGeoData(const char* name);

	std::vector<GridZone*> gridZones() const;
};

class GridZone
{
public:
	GridZone(PreProcessorGridAndGridCreatingConditionDataItem* item);
	~GridZone();

	void mapAllGeoData();
	void mapGeoData(const char* name);
};

class Pre
{
public:
	Pre(const Application* app);
	~Pre();

	GridType* gridType(const char* name) const;
	std::vector<GridType*> gridTypes() const;

	GeoDataGroup* geoDataGroup(const char* name) const;
	std::vector<GeoDataGroup*> geoDataGroups() const;

	void mapAllGeoData();
	void mapGeoData(const char* name);

	void clearGridTypes();
};

class Solver
{
public:
	Solver(const Application* app);
	~Solver();

	void start();
	void stop();

	void startAndWaitForFinish();
};

} // iRICPython
