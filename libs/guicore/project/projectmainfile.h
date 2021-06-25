#ifndef PROJECTMAINFILE_H
#define PROJECTMAINFILE_H

#include "../guicore_global.h"
#include "projectdataitem.h"
#include "projectdata.h"

#include <guibase/timeformat/timeformat.h>

#include <QModelIndex>

#include <string>
#include <vector>

namespace iRICLib {
	class H5CgnsFile;
} // namespace iRICLib

class QDomDocument;
class QPointF;

class BackgroundImageInfo;
class CoordinateSystem;
class iRICMainWindowInterface;
class MeasuredData;
class PostSolutionInfo;
class ProjectData;
class ProjectPostProcessors;
class SolverDefinitionAbstract;
class VersionNumber;

class vtkRenderer;

/// ProjectMainFile class represents the project main file ("project.xml") in project data.
class GUICOREDLL_EXPORT ProjectMainFile : public ProjectDataItem
{
	Q_OBJECT

public:
	const static QString FILENAME;
	const static QString BGDIR;
	/// Constructor
	ProjectMainFile(ProjectData* parent);
	virtual ~ProjectMainFile();
	/// Initializes for the specified solver definition.
	void initForSolverDefinition();
	/// Load solver information from file
	void loadSolverInformation();
	/// Load data from file
	void load();
	/// Load Post-processor settings from a project file.
	void openPostProcessors();
	/// Save data into file
	bool save();
	/// Save data contained in files except for CGNS files.
	bool saveExceptCGNS();
	/// Returns the filename of project main file as an absolute path.
	QString filename();
	/// Set information about the solver used for the calculation in this project
	void setSolverInformation(const SolverDefinitionAbstract&);
	/// Solver name
	const std::string& solverName() const;
	/// Set solver name
	void setSolverName(const std::string& name);

	/// Solver version number
	const VersionNumber& solverVersion() const;
	/// Set solver version number
	void setSolverVersion(const VersionNumber& version);

	/// iRIC version number
	const VersionNumber& iRICVersion() const;
	void setIRICVersion(const VersionNumber& v);

	bool separateResult() const;
	void setSeparateResult(bool separate);

	/// Solution information
	PostSolutionInfo* postSolutionInfo() const;
	bool hasResults();
	QStringList containedFiles() override;
	QString currentCgnsFileName() const override;
	/// Clear the results stored in the current CGNS file.
	void clearResults();

	int loadFromCgnsFile() override;
	int saveToCgnsFile() override;
	int updateCgnsFileOtherThanGrids() override;
	void closeCgnsFile() override;

	/// Return true when the work data is modified.
	bool isModified() const;
	/// PostProcessors.
	ProjectPostProcessors* postProcessors() const;
	void toggleGridEditFlag();
	/// Background images
	const std::vector<BackgroundImageInfo*>& backgroundImages() const;
	/// Measured data
	const std::vector<MeasuredData*>& measuredDatas() const;
	/// Renderers for background images
	const std::vector<vtkRenderer*>& renderers() const;

	void updateActorVisibility(int idx, bool vis);
	bool importCgnsFile(const QString& filename, const QString& newname);
	/// Import Measured data from CSV files.
	void addMeasuredData();
	virtual void setModified(bool modified = true) override;

	bool importVisGraphSetting(const QString filename);
	bool exportVisGraphSetting(const QString filename);

	CoordinateSystem* coordinateSystem() const;
	void setCoordinateSystem(CoordinateSystem* system);

	const QDateTime& zeroDateTime() const;
	void setZeroDateTime(const QDateTime& dt);

	TimeFormat timeFormat() const;
	void setTimeFormat(TimeFormat format);
	QString customTimeFormat() const;
	void setCustomTimeFormat(const QString& format);

	QPointF offset() const;
	void setOffset(double x, double y);

	iRICLib::H5CgnsFile* cgnsFile() const;

	bool mkdirBGDIR();
	/// Create background image
	void addBackgroundImage(BackgroundImageInfo* image);
	int showCoordinateSystemDialog(bool forceSelect = false);

public slots:
	void exportCurrentCgnsFile();

	void addBackgroundImage();
	void moveUpImage(const QModelIndex& index);
	void moveDownImage(const QModelIndex& index);
	void deleteImage(const QModelIndex& index);

	void moveUpMeasuredData(const QModelIndex& index);
	void moveDownMeasuredData(const QModelIndex& index);
	void deleteMeasuredData(const QModelIndex& index);

	void addRenderer(vtkRenderer* ren);
	void removeRenderer(vtkRenderer* ren);

	void clearModified();
	void setupOffset();

signals:
	void backgroundImageAdded();
	void backgroundImageDeleted(int i);
	void backgroundImageMovedUp(int i);
	void backgroundImageMovedDown(int i);
	void measuredDataAdded();
	void measuredDataDeleted(int i);
	void measuredDataMovedUp(int i);
	void measuredDataMovedDown(int i);

	void backgroundActorVisibilityChanged(int idx, bool vis);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	ProjectData* projectData() const override;
	QString relativeSubPath() const override;

	/// ProjectData
	ProjectData* m_projectData;
	/// Renderers for background images
	std::vector<vtkRenderer*> m_renderers;
	void checkVersionCompatibility();
	/// Load DOM document
	void loadDom(QDomDocument& doc);

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/projectmainfile_impl.h"
#endif // _DEBUG

#endif // PROJECTMAINFILE_H
