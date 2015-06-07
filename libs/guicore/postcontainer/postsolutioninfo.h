#ifndef POSTSOLUTIONINFO_H
#define POSTSOLUTIONINFO_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"
#include "../solverdef/solverdefinition.h"
#include "postexportsetting.h"

#include <QList>
#include <QMap>

class PostIterationSteps;
class PostTimeSteps;
class PostDataContainer;
class PostZoneDataContainer;

class GUICOREDLL_EXPORT PostSolutionInfo : public ProjectDataItem
{
	Q_OBJECT

public:
	enum Dimension {dim1D, dim2D, dim3D};
	enum ExportFormat {efVTK, efCSV};
	/// Constructor
	PostSolutionInfo(ProjectDataItem* parent);
	~PostSolutionInfo();
	SolverDefinition::IterationType iterationType() const {return m_iterationType;}
	void setIterationType(SolverDefinition::IterationType type);
	PostIterationSteps* iterationSteps() const {return m_iterationSteps;}
	PostTimeSteps* timeSteps() const {return m_timeSteps;}
	int currentStep() const {return m_currentStep;}
	double currentTimeStep();
	/// Returns true if the current CGNS file has results.
	bool hasResults();
	/// Emit signal cgnsStepsUpdated().
	void informCgnsSteps();
	void loadFromCgnsFile(const int fn) override;
	void closeCgnsFile() override;
	const QList<PostZoneDataContainer*>& zoneContainers1D() {return m_zoneContainers1D;}
	const QList<PostZoneDataContainer*>& zoneContainers2D() {return m_zoneContainers2D;}
	const QList<PostZoneDataContainer*>& zoneContainers3D() {return m_zoneContainers3D;}
	const QList<PostZoneDataContainer*>& zoneContainers(Dimension dim);
	PostZoneDataContainer* zoneContainer1D(const QString& zonename) {
		return m_zoneContainerNameMap1D.value(zonename, 0);
	}
	PostZoneDataContainer* zoneContainer2D(const QString& zonename) {
		return m_zoneContainerNameMap2D.value(zonename, 0);
	}
	PostZoneDataContainer* zoneContainer3D(const QString& zonename) {
		return m_zoneContainerNameMap3D.value(zonename, 0);
	}
	PostZoneDataContainer* zoneContainer(Dimension dim, const QString& zoneName);
	bool isDataAvailable();
	bool isDataAvailable1D();
	bool isDataAvailable2D();
	bool isDataAvailable3D();
	bool isDataAvailableBase();
	static int toIntDimension(Dimension dim);
	static Dimension fromIntDimension(int dim);
	bool open();
	void close();

	const PostExportSetting& exportSetting() const {return m_exportSetting;}
	const QString& particleExportPrefix() const {return m_particleExportPrefix;}
	void setExportSetting(const PostExportSetting& setting) {m_exportSetting = setting;}
	void setParticleExportPrefix(const QString& prefix) {m_particleExportPrefix = prefix;}

	/// File ID that can be used with cgnslib functions.
	int fileId() const {return m_fileId;}

protected:
	void timerEvent(QTimerEvent*) override;
	bool innerSetupZoneDataContainers(int fn, int dimiension, QStringList& zonenames, QList<PostZoneDataContainer*>& containers, QMap<QString, PostZoneDataContainer*>& containerNameMap);
	bool innerSetupDummy3DZoneDataContainers(int fn, QStringList& zonenames, QList<PostZoneDataContainer*>& containers, QMap<QString, PostZoneDataContainer*>& containerNameMap);
	virtual void doLoadFromProjectMainFile(const QDomNode& node) override;
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void informStepsUpdated();
	static void clearContainers(QList<PostZoneDataContainer*>& conts);

public slots:
	void informSolverStart();
	void informSolverFinish();
	bool setCurrentStep(unsigned int step, int fn = 0);
	void checkCgnsStepsUpdate();
	void exportCalculationResult();

protected slots:
	void handleIterationStepsUpdate(const QList<int>& steps);
	void handleTimeStepsUpdate(const QList<double>& steps);

signals:
	void currentStepUpdated();
	void updated();
	void allPostProcessorsUpdated();
	void cgnsStepsUpdated(const QList<QString>& steps);
	void cgnsStepsUpdated(int fn);
	void zoneList1DUpdated();
	void zoneList2DUpdated();
	void zoneList3DUpdated();

private:
	bool stepsExist() const;
	void setupZoneDataContainers(int fn);
	void checkBaseIterativeDataExist(int fn);
	static const int TIMERINTERVAL = 500;
	SolverDefinition::IterationType m_iterationType;
	PostIterationSteps* m_iterationSteps;
	PostTimeSteps* m_timeSteps;
	int m_currentStep;
	QList<PostZoneDataContainer*> m_zoneContainers1D;
	QList<PostZoneDataContainer*> m_zoneContainers2D;
	QList<PostZoneDataContainer*> m_zoneContainers3D;
	bool m_baseIterativeDataExists;
	QList<PostDataContainer*> m_otherContainers;
	int m_timerId;
	int m_fileId;

	QStringList m_zoneNames1D;
	QStringList m_zoneNames2D;
	QStringList m_zoneNames3D;
	QMap<QString, PostZoneDataContainer*> m_zoneContainerNameMap1D;
	QMap<QString, PostZoneDataContainer*> m_zoneContainerNameMap2D;
	QMap<QString, PostZoneDataContainer*> m_zoneContainerNameMap3D;

	ExportFormat m_exportFormat;

	PostExportSetting m_exportSetting;
	QString m_particleExportPrefix;
};

#endif // POSTSOLUTIONINFO_H
