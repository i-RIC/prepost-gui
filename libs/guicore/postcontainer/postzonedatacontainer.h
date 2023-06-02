#ifndef POSTZONEDATACONTAINER_H
#define POSTZONEDATACONTAINER_H

#include "../guicore_global.h"
#include "postdatacontainer.h"
#include "../misc/vtkpointsetandvaluerangesett.h"

#include <QString>

#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <h5cgnszone.h>

#include <map>
#include <string>

class RectRegion;
class SolverDefinitionGridType;
class PostCalculatedResult;
class PostExportSetting;

class vtkAbstractPointLocator;
class vtkStructuredGrid;

namespace iRICLib {
	class H5CgnsFlowSolution;
} // namespace iRICLib

class GUICOREDLL_EXPORT PostZoneDataContainer : public PostDataContainer
{
public:
	const static QString labelName;
	const static QString IBC;
	const static double IBCLimit;

	PostZoneDataContainer(const std::string& zoneName, SolverDefinitionGridType* gridtype, PostSolutionInfo* parent);
	~PostZoneDataContainer();

	const std::string& zoneName() const;
	SolverDefinitionGridType* gridType() const;
	vtkPointSetAndValueRangeSet* data() const;
	vtkPointSetAndValueRangeSet* data(iRICLib::H5CgnsZone::SolutionPosition position) const;
	vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* edgeIData() const;
	vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* edgeJData() const;
	vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* iFaceData() const;
	vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* jFaceData() const;
	vtkPointSet* labelData() const;
	vtkPointSetAndValueRangeSetT<vtkPolyData>* particleData() const;
	const std::map<std::string, vtkPointSetAndValueRangeSetT<vtkPolyData>*>& particleGroupMap() const;
	const std::map<std::string, vtkPointSetAndValueRangeSetT<vtkPolyData>*>& polyDataMap() const;
	vtkPointSetAndValueRangeSetT<vtkPolyData>* particleGroup(const std::string& name) const;
	vtkPointSetAndValueRangeSetT<vtkPolyData>* polyData(const std::string& name) const;
	const std::vector<int>& polyDataCellIds(const std::string& name) const;

	vtkPolyData* filteredData(double xmin, double xmax, double ymin, double ymax, bool* masked) const;
	vtkPolyData* filteredEdgeIData(double xmin, double xmax, double ymin, double ymax, bool* masked) const;
	vtkPolyData* filteredEdgeJData(double xmin, double xmax, double ymin, double ymax, bool* masked) const;

	vtkAbstractPointLocator* pointLocator() const;

	/// Currently, zone name is used instead, temporally.
	QString caption() const;

	bool handleCurrentStepUpdate(iRICLib::H5CgnsZone* zone, bool disableCalculatedResult);

	void loadFromCgnsFile(iRICLib::H5CgnsZone* zone);
	void loadFromCgnsFile(iRICLib::H5CgnsZone* zone, bool disableCalculatedResult);

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

	void loadIfEmpty(iRICLib::H5CgnsZone* zone);
	bool IBCCellExists() const;
	bool IBCEdgeIExists() const;
	bool IBCEdgeJExists() const;
	bool IBCExists() const;
	bool IBCExists(iRICLib::H5CgnsZone::SolutionPosition position) const;

	QString elevationName() const;
	QString cellElevationName() const;

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
	bool loadStructuredGrid(iRICLib::H5CgnsZone* zone);
	bool loadUnstructuredGrid(iRICLib::H5CgnsZone* zone);
	bool loadUnstructuredGrid(const int fn, const int currentStep);

	bool loadCellFlagData(iRICLib::H5CgnsZone* zone);
	void loadStringResultData();
	bool setupIndexData();

	void addCalculatedResultArrays();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doApplyOffset(double x_diff, double y_diff) override;
	void doApplyOffset(vtkPointSet* ps, double x_diff, double y_diff);

	static vtkPolyData* filteredDataStructured(vtkStructuredGrid* data, vtkAbstractPointLocator* pointLocator, double xmin, double xmax, double ymin, double ymax, bool* masked);
	static vtkPolyData* filteredDataUnstructured(vtkPointSet* data, double xmin, double xmax, double ymin, double ymax, bool* masked);

private:
	SolverDefinitionGridType* m_gridType;
	vtkPointSetAndValueRangeSet* m_data;
	vtkSmartPointer<vtkPointSet> m_origData;
	vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* m_edgeIData;		// IFaceCenter grid [Ni,Nj-1] interpolated to Vertex grid [Ni,Nj] valid for structured grids with CellDim >= 2
	vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* m_edgeJData;		// IFaceCenter grid [Ni,Nj-1] interpolated to Vertex grid [Ni,Nj] valid for structured grids with CellDim >= 2
	vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* m_iFaceData;		// IFaceCenter grid [Ni,Nj-1] valid for structured grids with CellDim >= 2
	vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* m_jFaceData;		// JFaceCenter grid [Ni-1,Nj] valid for structured grids with CellDim >= 2
	vtkSmartPointer<vtkPointSet> m_labelData;
	vtkPointSetAndValueRangeSetT<vtkPolyData>* m_particleData;
	std::map<std::string, vtkPointSetAndValueRangeSetT<vtkPolyData>*> m_particleGroupMap;
	std::map<std::string, vtkPointSetAndValueRangeSetT<vtkPolyData>*> m_polyDataMap;
	std::map<std::string, std::vector<int> > m_polyDataCellIdsMap;
	std::vector<PostCalculatedResult*> m_calculatedResults;
	vtkAbstractPointLocator* m_pointLocator;

	std::string m_zoneName;
	std::vector<int> m_size;

	bool m_loadOK;
	bool m_loadedOnce;

	class ParticleLoader;
	class ParticleGroupLoader;
	class PolyDataLoader;
};

#endif // POSTZONEDATACONTAINER_H
