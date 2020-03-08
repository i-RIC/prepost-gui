#ifndef POSTZONEDATACONTAINER_H
#define POSTZONEDATACONTAINER_H

#include "../guicore_global.h"
#include "postdatacontainer.h"
#include "../pre/grid/grid.h"

#include <QString>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <cgnslib.h>

#include <map>
#include <string>

class RectRegion;
class SolverDefinitionGridType;
class PostCalculatedResult;
class PostExportSetting;

class GUICOREDLL_EXPORT PostZoneDataContainer : public PostDataContainer
{

public:
	const static QString labelName;
	const static QString IBC;
	const static double IBCLimit;

	PostZoneDataContainer(const std::string& baseName, const std::string& zoneName, SolverDefinitionGridType* gridtype, PostSolutionInfo* parent);
	~PostZoneDataContainer();

	SolverDefinitionGridType* gridType() const;
	vtkPointSet* data() const;
	vtkPointSet* edgeidata() const;
	vtkPointSet* edgejdata() const;
	vtkPointSet* data(GridLocation_t gridLocation) const;
	vtkPointSet* ifacedata() const;
	vtkPointSet* jfacedata() const;
	vtkPointSet* labelData() const;
	vtkPolyData* particleData() const;
	const std::map<std::string, vtkSmartPointer<vtkPolyData> >& particleGroupMap() const;
	const std::map<std::string, vtkSmartPointer<vtkPolyData> >& polyDataMap() const;
	vtkPolyData* particleGroup(const std::string& name) const;
	vtkPolyData* polyData(const std::string& name) const;
	const std::vector<int>& polyDataCellIds(const std::string& name) const;

	vtkPolyData* filteredData(double xmin, double xmax, double ymin, double ymax, bool& masked) const;
	vtkPolyData* filteredEdgeIData(double xmin, double xmax, double ymin, double ymax, bool& masked) const;
	vtkPolyData* filteredEdgeJData(double xmin, double xmax, double ymin, double ymax, bool& masked) const;

	int baseId() const;
	int zoneId() const;
	const std::string& zoneName() const;
	/// Caption is the region name in pre-processor.
	/// Currently, zone name is used instead, temporally.
	QString caption() const;

	bool handleCurrentStepUpdate(const int fn, const int timeStep) override;
	bool handleCurrentStepUpdate(const int fn, const int timeStep, bool disableCalculatedResult);
	void loadFromCgnsFile(const int fn) override;
	void loadFromCgnsFile(const int fn, const int timeStep, bool disableCalculatedResult = false);

	bool cellScalarValueExists() const;
	bool edgeIScalarValueExists() const;
	bool edgeJScalarValueExists() const;
	bool scalarValueExists() const;
	bool vectorValueExists() const;

	std::vector<PostCalculatedResult*>& calculatedResults();
	const std::vector<PostCalculatedResult*>& calculatedResults() const;

	int nodeIndex(int i, int j, int k) const;
	void getNodeIJKIndex(int index, int* i, int* j, int* k) const;
	int cellIndex(int i, int j, int k) const;
	void getCellIJKIndex(int index, int* i, int* j, int* k) const;
	int ifaceIndex(int i, int j, int k) const;
	void getifaceIJKIndex(int index, int* i, int* j, int* k) const;
	int jfaceIndex(int i, int j, int k) const;
	void getjfaceIJKIndex(int index, int* i, int* j, int* k) const;

	void loadIfEmpty(const int fn);
	bool IBCCellExists() const;
	bool IBCEdgeIExists() const;
	bool IBCEdgeJExists() const;
	bool IBCExists() const;
	bool IBCExists(GridLocation_t gridLocation) const;

	QString elevationName() const;

	void applyOffset(double x, double y);

	std::vector<PostCalculatedResult*> detachCalculatedResult();
	void attachCalculatedResult(std::vector<PostCalculatedResult*> results);

	static std::string inputDataPrefix();
	static std::string addInputDataPrefix(const std::string& name);
	static std::string removeInputDataPrefix(const std::string& name);
	static bool hasInputDataPrefix(const std::string& name);

	static int index(int dim[3], int i, int j, int k);
	static void getIJKIndex(int dim[3], int idx, int* i, int* j, int* k);

private:
	bool setBaseId(const int fn);
	bool setZoneId(const int fn);
	bool loadZoneSize(const int fn);
	virtual bool loadStructuredGrid(const int fn, const int currentStep);
	bool loadUnstructuredGrid(const int fn, const int currentStep);
	bool findSolutionId(const int fn, const int currentStep, int* solId, const char* arrayName);
	bool getCellSolutionId(const int fn, const int currentStep, int* solId);
	bool getEdgeISolutionId(const int fn, const int currentStep, int* solId);
	bool getEdgeJSolutionId(const int fn, const int currentStep, int* solId);
	bool getSolutionId(const int fn, const int currentStep, int* solId);
	virtual bool loadGridScalarData(const int fn, const int solid);
	virtual bool loadGridVectorData(const int fn, const int solid);

	static bool loadScalarData(vtkDataSetAttributes* atts, int firstAtt = 1);
	static bool loadEdgeIScalarData(vtkDataSetAttributes* atts, int firstAtt = 1);
	static bool loadEdgeJScalarData(vtkDataSetAttributes* atts, int firstAtt = 1);
	static bool loadVectorData(vtkDataSetAttributes* atts, int firstAtt = 1);

	bool loadCellFlagData(const int fn);
	void loadStringResultData();
	bool setupIndexData();

	void addCalculatedResultArrays();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doApplyOffset(double x_diff, double y_diff) override;
	void doApplyOffset(vtkPointSet* ps, double x_diff, double y_diff);

	vtkPolyData* filteredDataStructured(vtkSmartPointer<vtkPointSet> data, double xmin, double xmax, double ymin, double ymax, bool& masked) const;
	vtkPolyData* filteredDataUnstructured(double xmin, double xmax, double ymin, double ymax, bool& masked) const;
	int lineLimitI(int j, int iIn, int iOut, const RectRegion& region) const;
	int lineLimitJ(int i, int jIn, int jOut, const RectRegion& region) const;
	int lineLimitI2(int iIn, int iOut, const RectRegion& region) const;
	int lineLimitJ2(int jIn, int jOut, const RectRegion& region) const;
	bool lineAtIIntersect(int i, const RectRegion& region) const;
	bool lineAtJIntersect(int j, const RectRegion& region) const;

private:
	SolverDefinitionGridType* m_gridType;
	vtkSmartPointer<vtkPointSet> m_data;
	vtkSmartPointer<vtkPointSet> m_edgeidata;		// IFaceCenter grid [Ni,Nj-1] interpolated to Vertex grid [Ni,Nj] valid for structured grids with CellDim >= 2
	vtkSmartPointer<vtkPointSet> m_edgejdata;		// IFaceCenter grid [Ni,Nj-1] interpolated to Vertex grid [Ni,Nj] valid for structured grids with CellDim >= 2
	vtkSmartPointer<vtkPointSet> m_ifacedata;		// IFaceCenter grid [Ni,Nj-1] valid for structured grids with CellDim >= 2
	vtkSmartPointer<vtkPointSet> m_jfacedata;		// JFaceCenter grid [Ni-1,Nj] valid for structured grids with CellDim >= 2
	vtkSmartPointer<vtkPointSet> m_labelData;
	vtkSmartPointer<vtkPolyData> m_particleData;
	std::vector<PostCalculatedResult*> m_calculatedResults;
	std::map<std::string, vtkSmartPointer<vtkPolyData> > m_particleGroupMap;
	std::map<std::string, vtkSmartPointer<vtkPolyData> > m_polyDataMap;
	std::map<std::string, std::vector<int> > m_polyDataCellIdsMap;
	std::string m_baseName;
	std::string m_zoneName;
	int m_baseId;
	int m_zoneId;
	int m_cellDim;
	cgsize_t m_sizes[9];

	bool m_loadOK;
	bool m_loadedOnce;

	class ParticleLoader;
	class ParticleGroupLoader;
	class PolyDataLoader;
};

#endif // POSTZONEDATACONTAINER_H
