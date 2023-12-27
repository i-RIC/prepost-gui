#ifndef POSTSOLUTIONINFO_H
#define POSTSOLUTIONINFO_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"
#include "../solverdef/solverdefinition.h"
#include "postdataexportdialog.h"
#include "postexportsetting.h"

#include <QList>
#include <QMap>
#include <QStringList>

#include <map>
#include <vector>

namespace iRICLib {
	class H5CgnsFile;
} // namespace iRICLib

class PostBaseIterativeNumericalDataContainer;
class PostBaseIterativeStringDataContainer;
class PostBaseIterativeValuesContainer;
class PostCalculatedResult;
class PostIterationSteps;
class PostTimeSteps;
class PostZoneDataContainer;
class v4PostCalculatedResult;
class v4PostZoneDataContainer;

class QDomElement;

class GUICOREDLL_EXPORT PostSolutionInfo : public ProjectDataItem
{
	Q_OBJECT

public:
	enum Dimension {dim1D, dim2D, dim3D};

	PostSolutionInfo(ProjectDataItem* parent);
	~PostSolutionInfo();

	SolverDefinition::IterationType iterationType() const;
	void setIterationType(SolverDefinition::IterationType type);

	QStringList containedFiles() const override;

	PostIterationSteps* iterationSteps() const;
	PostTimeSteps* timeSteps() const;
	int stepCount() const;
	int currentStep() const;
	double currentTimeStep();
	/// Returns true if the current CGNS file has results.
	bool hasResults();
	/// Emit signal cgnsStepsUpdated().
	void informCgnsSteps();
	int loadFromCgnsFile() override;
	void closeCgnsFile() override;

	const std::vector<v4PostZoneDataContainer*>& v4ZoneContainers1D() const;
	const std::vector<v4PostZoneDataContainer*>& v4ZoneContainers2D() const;
	const std::vector<v4PostZoneDataContainer*>& v4ZoneContainers3D() const;
	const std::vector<v4PostZoneDataContainer*>& v4ZoneContainers(Dimension dim) const;
	v4PostZoneDataContainer* v4ZoneContainer1D(const std::string& zoneName) const;
	v4PostZoneDataContainer* v4ZoneContainer2D(const std::string& zoneName) const;
	v4PostZoneDataContainer* v4ZoneContainer3D(const std::string& zoneName) const;
	v4PostZoneDataContainer* v4ZoneContainer(Dimension dim, const std::string& zoneName) const;

	const QList<PostZoneDataContainer*>& zoneContainers1D() const;
	const QList<PostZoneDataContainer*>& zoneContainers2D() const;
	const QList<PostZoneDataContainer*>& zoneContainers3D() const;
	const QList<PostZoneDataContainer*>& zoneContainers(Dimension dim) const;
	PostZoneDataContainer* zoneContainer1D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer2D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer3D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer(Dimension dim, const std::string& zoneName) const;
	PostZoneDataContainer* firstZoneContainer() const;
	PostBaseIterativeValuesContainer* baseIterativeValuesContainer() const;

	const std::map<std::string, PostBaseIterativeStringDataContainer*>& baseIterativeStringResults() const;
	const std::map<std::string, PostBaseIterativeNumericalDataContainer*>& baseIterativeNumericalResults() const;
	PostBaseIterativeStringDataContainer* baseIterativeStringResult(const std::string& name) const;
	PostBaseIterativeNumericalDataContainer* baseIterativeNumericalResult(const std::string& name) const;

	bool isDataAvailable() const;
	bool isDataAvailable1D() const;
	bool isDataAvailable2D() const;
	bool isDataAvailable3D() const;
	bool isDataAvailableBase() const;

	static int toIntDimension(Dimension dim);
	static Dimension fromIntDimension(int dim);
	int open();
	void close();

	const PostExportSetting& exportSetting() const;
	const QString& particleExportPrefix() const;
	void setExportSetting(const PostExportSetting& setting);
	void setParticleExportPrefix(const QString& prefix);
	bool separateResultExists() const;

	/// File ID that can be used with cgnslib functions.
	int fileId() const;
	iRICLib::H5CgnsFile* cgnsFile() const;
	void setCalculatedResultDisabled(bool disabled);

	void exportCalculationResult(const std::string& folder, const std::string& prefix, const std::vector<int> steps, PostDataExportDialog::Format format);

	void applyOffset(double x_diff, double y_diff);

public slots:
	void informSolverStart();
	void informSolverFinish();
	int setCurrentStep(unsigned int step);
	void checkCgnsStepsUpdate();
	void exportCalculationResult();

private slots:
	void informStepsUpdated();

signals:
	void currentStepUpdated();
	void updated();
	void allPostProcessorsUpdated();
	void cgnsTimeStepsUpdated(const QList<double>& steps);
	void cgnsIterationStepsUpdated(const QList<int>& steps);
	void cgnsStepsUpdated();
	void zoneList1DUpdated();
	void zoneList2DUpdated();
	void zoneList3DUpdated();

private:
	bool stepsExist() const;
	void setupZoneDataContainers();
	ProjectMainFile* mainFile() const;
	void loadDividedBaseIterativeData();
	void loadCalculatedResult();
	void clearCalculatedResults(QMap<std::string, std::vector<PostCalculatedResult*> >* results);
	bool innerSetupZoneDataContainers(int dimension, QList<PostZoneDataContainer*>* containers, QMap<std::string, PostZoneDataContainer*>* containerNameMap, QMap<std::string, std::vector<PostCalculatedResult*> > *results);
	bool innerSetupZoneDataContainers(int dimension, std::vector<v4PostZoneDataContainer*>* containers, std::map<std::string, v4PostZoneDataContainer*>* containerNameMap, std::map<std::string, std::vector<v4PostCalculatedResult*> > *calculatedResults);

	bool setupBaseIterativeResults();
	void clearBaseIterativeResults();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void timerEvent(QTimerEvent*) override;

	static void clearContainers(QList<PostZoneDataContainer*>* conts);
	static void clearContainers(std::vector<v4PostZoneDataContainer*>* conts);

	static const int TIMERINTERVAL = 500;
	SolverDefinition::IterationType m_iterationType;

	PostIterationSteps* m_iterationSteps;
	PostTimeSteps* m_timeSteps;
	int m_currentStep;

	std::vector<v4PostZoneDataContainer*> m_v4ZoneContainers1D;
	std::vector<v4PostZoneDataContainer*> m_v4ZoneContainers2D;
	std::vector<v4PostZoneDataContainer*> m_v4ZoneContainers3D;
	std::map<std::string, v4PostZoneDataContainer*> m_v4ZoneContainerNameMap1D;
	std::map<std::string, v4PostZoneDataContainer*> m_v4ZoneContainerNameMap2D;
	std::map<std::string, v4PostZoneDataContainer*> m_v4ZoneContainerNameMap3D;

	QList<PostZoneDataContainer*> m_zoneContainers1D;
	QList<PostZoneDataContainer*> m_zoneContainers2D;
	QList<PostZoneDataContainer*> m_zoneContainers3D;
	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap1D;
	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap2D;
	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap3D;

	int m_timerId;
	iRICLib::H5CgnsFile* m_cgnsFile;
	PostBaseIterativeValuesContainer* m_baseIterativeValuesContainer;

	QMap<std::string, std::vector<PostCalculatedResult*> > m_calculatedResults1D;
	QMap<std::string, std::vector<PostCalculatedResult*> > m_calculatedResults2D;
	QMap<std::string, std::vector<PostCalculatedResult*> > m_calculatedResults3D;
	std::map<std::string, std::vector<v4PostCalculatedResult*> > m_v4CalculatedResults1D;
	std::map<std::string, std::vector<v4PostCalculatedResult*> > m_v4CalculatedResults2D;
	std::map<std::string, std::vector<v4PostCalculatedResult*> > m_v4CalculatedResults3D;

	std::map<std::string, PostBaseIterativeStringDataContainer*> m_baseIterativeStringResults;
	std::map<std::string, PostBaseIterativeNumericalDataContainer*> m_baseIterativeNumericalResults;

	PostDataExportDialog::Format m_exportFormat;
	bool m_disableCalculatedResult;

	PostExportSetting m_exportSetting;
	QString m_particleExportPrefix;

	QDomElement* m_loadedElement;

	class UpdateIfNeededThread;
};

#endif // POSTSOLUTIONINFO_H
