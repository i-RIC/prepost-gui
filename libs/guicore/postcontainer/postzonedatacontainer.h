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

	PostZoneDataContainer(const std::string& baseName, const std::string& zoneName, SolverDefinitionGridType* gridtype, ProjectDataItem* parent);
	~PostZoneDataContainer();

	SolverDefinitionGridType* gridType() const;
	vtkPointSet* data() const;
	vtkPointSet* labelData() const;
	vtkPolyData* particleData() const;
	vtkPolyData* filteredData(double xmin, double xmax, double ymin, double ymax, bool& masked) const;

	int baseId() const;
	int zoneId() const;
	const std::string& zoneName() const;
	/// Caption is the region name in pre-processor.
	/// Currently, zone name is used instead, temporally.
	QString caption() const;

	bool handleCurrentStepUpdate(const int fn) override;
	bool handleCurrentStepUpdate(const int fn, bool disableCalculatedResult);
	void loadFromCgnsFile(const int fn) override;
	void loadFromCgnsFile(const int fn, bool disableCalculatedResult);

	bool cellScalarValueExists() const;
	bool scalarValueExists() const;
	bool vectorValueExists() const;

	std::vector<PostCalculatedResult*>& calculatedResults();
	const std::vector<PostCalculatedResult*>& calculatedResults() const;

	int nodeIndex(int i, int j, int k) const;
	void getNodeIJKIndex(int index, int* i, int* j, int* k) const;
	int cellIndex(int i, int j, int k) const;
	void getCellIJKIndex(int index, int* i, int* j, int* k) const;

	void loadIfEmpty(const int fn);
	bool IBCCellExists() const;
	bool IBCExists() const;
	QString elevationName() const;

	void applyOffset(double x, double y);

	std::vector<PostCalculatedResult*> detachCalculatedResult();
	void attachCalculatedResult(std::vector<PostCalculatedResult*> results);

protected:
	void doApplyOffset(double x, double y) override;

private:
	bool setBaseId(const int fn);
	bool setZoneId(const int fn);
	bool loadZoneSize(const int fn);
	virtual bool loadStructuredGrid(const int fn, const int currentStep);
	bool loadUnstructuredGrid(const int fn, const int currentStep);
	bool loadParticle(const int fn, const int currentStep);
	bool findSolutionId(const int fn, const int currentStep, int* solId, const char* arrayName);
	bool getCellSolutionId(const int fn, const int currentStep, int* solId);
	bool getSolutionId(const int fn, const int currentStep, int* solId);
	virtual bool loadGridScalarData(const int fn, const int solid);
	virtual bool loadGridVectorData(const int fn, const int solid);

	bool loadScalarData(vtkDataSetAttributes* atts, int firstAtt = 1);
	bool loadVectorData(vtkDataSetAttributes* atts, int firstAtt = 1);

	bool loadCellFlagData(const int fn);
	bool setupIndexData();

	void addCalculatedResultArrays();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkPolyData* filteredDataStructured(double xmin, double xmax, double ymin, double ymax, bool& masked) const;
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
	vtkSmartPointer<vtkPointSet> m_labelData;
	vtkSmartPointer<vtkPolyData> m_particleData;
	std::vector<PostCalculatedResult*> m_calculatedResults;
	std::string m_baseName;
	std::string m_zoneName;
	int m_baseId;
	int m_zoneId;
	int m_cellDim;
	cgsize_t m_sizes[9];

	bool m_loadOK;
	bool m_loadedOnce;
};

#endif // POSTZONEDATACONTAINER_H
