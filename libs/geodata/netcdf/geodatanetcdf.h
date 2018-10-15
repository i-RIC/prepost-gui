#ifndef GEODATANETCDF_H
#define GEODATANETCDF_H

#include <guicore/pre/geodata/geodata.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkThreshold.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

#include <netcdf.h>

class QAction;
class QPolygonF;
class GeoDataNetcdfImporter;
class GeoDataNetcdfXbandImporter;
class GeoDataNetcdfGdalImporter;
class GridAttributeDimensionContainer;

class GeoDataNetcdf : public GeoData
{
	Q_OBJECT

private:
	class RectRegion
	{

	public:
		double xMin;
		double xMax;
		double yMin;
		double yMax;

		RectRegion(double xmin, double xmax, double ymin, double ymax) :
			xMin {xmin},
			xMax {xmax},
			yMin {ymin},
			yMax {ymax}
		{}
		bool pointIsInside(double x, double y) const;
		bool intersect(const QLineF& line) const;
	};

public:
	static const int MAX_DRAWCELLCOUNT;

	enum CoordinateSystemType {
		LonLat,
		XY
	};

	/// Constructor
	GeoDataNetcdf(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att);
	virtual ~GeoDataNetcdf();

	const std::vector<double> lonValues() const {return m_lonValues;}
	const std::vector<double> latValues() const {return m_latValues;}
	std::vector<double> lonValues() {return m_lonValues;}
	std::vector<double> latValues() {return m_latValues;}

	const std::vector<double> xValues() const {return m_xValues;}
	const std::vector<double> yValues() const {return m_yValues;}
	std::vector<double> xValues() {return m_xValues;}
	std::vector<double> yValues() {return m_yValues;}

	int xSize() const;
	int ySize() const;

	CoordinateSystemType coordinateSystemType() const {return m_coordinateSystemType;}
	QString coordinateSystemName() const {return m_coordinateSystemName;}

	int defineCoords(int ncid, int* xDimId, int* yDimId, int* lonDimId, int* latDimId, int* xVarId, int* yVarId, int* lonVarId, int* latVarId);
	int defineDimensions(int ncid, std::vector<int>* dimIds, std::vector<int>* varIds);
	int defineValue(int ncid, int xId, int yId, const std::vector<int>& dimIds, int* varId);
	int outputCoords(int ncid, int xId, int yId, int lonId, int latId);
	int outputDimensions(int ncid, const std::vector<int>& varIds);
	bool requestCoordinateSystem() const override {return true;}
	vtkStructuredGrid* grid() const {return m_grid;}
	void viewOperationEndedGlobal(PreProcessorGraphicsViewInterface* v) override;
	virtual double thresholdValue() const = 0;

	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

public slots:
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex) override;
	void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after) override;

private:
	void setupActors() override;
	void updateShapeData();
	void updateActorSettings();
	void updateSimpifiedGrid(double xmin, double xmax, double ymin, double ymax);
	void updateRegionPolyData();

	static nc_type getNcType(SolverDefinitionGridAttributeDimension* dim);
	static nc_type getNcType(SolverDefinitionGridAttribute* cond);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateFilename() override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void doApplyOffset(double x, double y) override;
	int getValueVarId(int ncid, int* varId);

	void getIJIndex(vtkIdType id, unsigned int* i, unsigned int* j) const;
	unsigned int vertexIndex(unsigned int i, unsigned int j) const;

	int lineLimitI(int j, int iIn, int iOut, int dimI, int dimJ, const RectRegion& region) const;
	int lineLimitJ(int i, int jIn, int jOut, int dimI, int dimJ, const RectRegion& region) const;
	int lineLimitI2(int iIn, int iOut, int dimI, int dimJ, const RectRegion& region) const;
	int lineLimitJ2(int jIn, int jOut, int dimI, int dimJ, const RectRegion& region) const;
	bool lineAtIIntersect(int i, int dimI, int dimJ, const RectRegion& region) const;
	bool lineAtJIntersect(int j, int dimI, int dimJ, const RectRegion& region) const;

	virtual void doHandleDimensionCurrentIndexChange(int oldIndex, int newIndex) = 0;
	virtual void doHandleDimensionValuesChange(GridAttributeDimensionContainer* cont, const std::vector<QVariant>& before, const std::vector<QVariant>& after) = 0;

	vtkSmartPointer<vtkStructuredGrid> m_grid;
	vtkSmartPointer<vtkStructuredGrid> m_simplifiedGrid;
	int m_opacityPercent;

	vtkSmartPointer<vtkThreshold> m_threshold;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkActor> m_actor;

	vtkSmartPointer<vtkPolyData> m_regionPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_regionMapper;
	vtkSmartPointer<vtkActor> m_regionActor;

	std::vector<double> m_lonValues;
	std::vector<double> m_latValues;

	std::vector<double> m_xValues;
	std::vector<double> m_yValues;

	CoordinateSystemType m_coordinateSystemType;
	QString m_coordinateSystemName;

	bool m_isMasked;

public:
	friend class GeoDataNetcdfImporter;
	friend class GeoDataNetcdfXbandImporter;
	friend class GeoDataNetcdfGdalImporter;
};

#endif // GEODATANETCDF_H
