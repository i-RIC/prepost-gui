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

class PostIterationSteps;
class PostTimeSteps;
class PostDataContainer;
class PostZoneDataContainer;

class GUICOREDLL_EXPORT PostSolutionInfo : public ProjectDataItem
{
	Q_OBJECT

public:
	enum Dimension {dim1D, dim2D, dim3D};
	/// Constructor
	PostSolutionInfo(ProjectDataItem* parent);
	~PostSolutionInfo();
	SolverDefinition::IterationType iterationType() const;
	void setIterationType(SolverDefinition::IterationType type);
	PostIterationSteps* iterationSteps() const;
	PostTimeSteps* timeSteps() const;
	int currentStep() const;
	double currentTimeStep();
	/// Returns true if the current CGNS file has results.
	bool hasResults();
	/// Emit signal cgnsStepsUpdated().
	void informCgnsSteps();
	void loadFromCgnsFile(const int fn) override;
	void closeCgnsFile() override;
	const QList<PostZoneDataContainer*>& zoneContainers1D() const;
	const QList<PostZoneDataContainer*>& zoneContainers2D() const;
	const QList<PostZoneDataContainer*>& zoneContainers3D() const;
	const QList<PostZoneDataContainer*>& zoneContainers(Dimension dim) const;
	PostZoneDataContainer* zoneContainer1D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer2D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer3D(const std::string& zoneName) const;
	PostZoneDataContainer* zoneContainer(Dimension dim, const std::string& zoneName) const;
	bool isDataAvailable() const;
	bool isDataAvailable1D() const;
	bool isDataAvailable2D() const;
	bool isDataAvailable3D() const;
	bool isDataAvailableBase() const;
	static int toIntDimension(Dimension dim);
	static Dimension fromIntDimension(int dim);
	bool open();
	void close();

	const PostExportSetting& exportSetting() const;
	const QString& particleExportPrefix() const;
	void setExportSetting(const PostExportSetting& setting);
	void setParticleExportPrefix(const QString& prefix);

	/// File ID that can be used with cgnslib functions.
	int fileId() const;

protected:
	void timerEvent(QTimerEvent*) override;
	bool innerSetupZoneDataContainers(int fn, int dimiension, std::vector<std::string>* zoneNames, QList<PostZoneDataContainer*>* containers, QMap<std::string, PostZoneDataContainer*>* containerNameMap);
	bool innerSetupDummy3DZoneDataContainers(int fn, std::vector<std::string>* zoneNames, QList<PostZoneDataContainer*>* containers, QMap<std::string, PostZoneDataContainer*>* containerNameMap);
	virtual void doLoadFromProjectMainFile(const QDomNode& node) override;
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void informStepsUpdated();
	static void clearContainers(QList<PostZoneDataContainer*>* conts);

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

	std::vector<std::string> m_zoneNames1D;
	std::vector<std::string> m_zoneNames2D;
	std::vector<std::string> m_zoneNames3D;

	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap1D;
	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap2D;
	QMap<std::string, PostZoneDataContainer*> m_zoneContainerNameMap3D;

	PostDataExportDialog::Format m_exportFormat;

	PostExportSetting m_exportSetting;
	QString m_particleExportPrefix;
};

#endif // POSTSOLUTIONINFO_H
