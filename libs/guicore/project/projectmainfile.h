#ifndef PROJECTMAINFILE_H
#define PROJECTMAINFILE_H

#include "../guicore_global.h"
#include "projectdataitem.h"
#include "projectdata.h"
#include <misc/versionnumber.h>

#include <QModelIndex>
#include <QVector2D>

class QDomDocument;

class ProjectData;
class iRICMainWindowInterface;
class SolverDefinitionAbstract;
class CgnsFileList;
class PostSolutionInfo;
class ProjectPostProcessors;
class BackgroundImageInfo;
class MeasuredData;
class CoordinateSystem;

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
	/// Create first Cgns file.
	void createDefaultCgnsFile();
	/// Initializes for the specified solver definition.
	void initForSolverDefinition();
	/// Load solver information from file
	void loadSolverInformation();
	/// Load data from file
	void load();
	/// Load only CGNS file list from a project file.
	void loadCgnsList();
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
	const QString& solverName() const {return m_solverName;}
	/// Set solver name
	void setSolverName(const QString& name) {m_solverName = name;}
	/// Solver version number
	const VersionNumber& solverVersion() const {return m_solverVersion;}
	/// Set solver version number
	void setSolverVersion(const VersionNumber& version) {m_solverVersion = version;}
	/// iRIC version number
	const VersionNumber& iRICVersion() const {return m_iRICVersion;}
	void setIRICVersion(const VersionNumber& v) {
		m_iRICVersion = v;
	}
	/// Cgns file list
	CgnsFileList* cgnsFileList() const {return m_cgnsFileList;}
	/// Solution information
	PostSolutionInfo* postSolutionInfo() const {return m_postSolutionInfo;}
	bool hasResults();
	QStringList containedFiles();
	const QString currentCgnsFileName() const;
	/// Clear the results stored in the current CGNS file.
	void clearResults();
	/// Save current cgns file.
	bool saveCgnsFile();
	/// Return true when the work data is modified.
	bool isModified() const {return m_isModified;}
	/// PostProcessors.
	ProjectPostProcessors* postProcessors() const {return m_postProcessors;}
	void loadFromCgnsFile(const int fn);
	void saveToCgnsFile(const int fn);
	void toggleGridEditFlag();
	void closeCgnsFile();
	/// Background images
	const QList<BackgroundImageInfo*> backgroundImages() const {return m_backgroundImages;}
	const QList<MeasuredData*>& measuredDatas() const {return m_measuredDatas;}
	/// Renderers for background images
	const QList<vtkRenderer*>& renderers() const {return m_renderers;}

	void updateActorVisibility(int idx, bool vis);
	void importCgnsFile(const QString& filename);
	bool importCgnsFile(const QString& filename, const QString& newname);
	/// Import Measured data from CSV files.
	void addMeasuredData();
	virtual void setModified();

	bool importVisGraphSetting(const QString filename);
	bool exportVisGraphSetting(const QString filename);
	CoordinateSystem* coordinateSystem() const {return m_coordinateSystem;}
	void setCoordinateSystem(CoordinateSystem* system);
	const QVector2D offset() const {return m_offset;}
	void setOffset(double x, double y);
	bool mkdirBGDIR();
	/// Create background image
	void addBackgroundImage(BackgroundImageInfo* image);
	int showCoordinateSystemDialog();

public slots:
	/// Import Cgns file
	void importCgnsFile();
	/// Export Cgns file
	void exportCurrentCgnsFile();
	/// Switch between cgns files in cgnsFileList().
	bool switchCgnsFile(const QString& name);
	/// Create background image
	void addBackgroundImage();
	/// Delete background image
	void deleteImage(const QModelIndex& index);
	/// Move up background image
	void moveUpImage(const QModelIndex& index);
	/// Move down background image
	void moveDownImage(const QModelIndex& index);

	/// Delete measured data
	void deleteMeasuredData(const QModelIndex& index);
	/// Move up measured data
	void moveUpMeasuredData(const QModelIndex& index);
	/// Move down measured data
	void moveDownMeasuredData(const QModelIndex& index);

	/// Remove renderer
	void removeRenderer(vtkRenderer* ren);
	void addRenderer(vtkRenderer* ren);
	void clearModified() {m_isModified = false;}
	void setupOffset();

protected:
	/// Load information to restore data from ProjectMainFile.
	void doLoadFromProjectMainFile(const QDomNode& node);
	/// Write into projectmainfile.
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	ProjectData* projectData() const {return m_projectData;}
	QString relativeSubPath() const {return "";}

private:
	bool clearResultsIfGridIsEdited();

	/// Solver name
	QString m_solverName;
	/// Solver version
	VersionNumber m_solverVersion;
	/// iRIC version
	VersionNumber m_iRICVersion;
	/// Cgns file list
	CgnsFileList* m_cgnsFileList;
	/// Container that has solution information.
	PostSolutionInfo* m_postSolutionInfo;
	/// PostProcessor windows.
	ProjectPostProcessors* m_postProcessors;
	/// ProjectData
	ProjectData* m_projectData;
	/// Background images
	QList<BackgroundImageInfo*> m_backgroundImages;
	/// Measured datas
	QList<MeasuredData*> m_measuredDatas;
	/// Renderers for background images
	QList<vtkRenderer*> m_renderers;
	/// Load data from CGNS file
	bool loadCgnsFile(const QString& name);
	/// Soad data from CGNS file
	bool saveCgnsFile(const QString& name);
	/// Load backgrounds
	void loadBackgrounds(const QDomNode& node);
	/// save backgrounds
	void saveBackgrounds(QXmlStreamWriter& writer);
	/// Background images' file name.
	QStringList backgroundImageFiles();
	/// Load Measured datas
	void loadMeasuredDatas(const QDomNode& node);
	/// Save Measured datas
	void saveMeasuredDatas(QXmlStreamWriter& writer);
	void checkVersionCompatibility();
	/// Load DOM document
	void loadDom(QDomDocument& doc);

	CoordinateSystem* m_coordinateSystem;
	bool m_isWaitigHttpResponse;
	QVector2D m_offset;

	bool m_isModified;
signals:
	void cgnsFileSwitched();
	void backgroundImageAdded();
	void backgroundImageDeleted(int i);
	void backgroundImageMovedUp(int i);
	void backgroundImageMovedDown(int i);
	void measuredDataAdded();
	void measuredDataDeleted(int i);
	void measuredDataMovedUp(int i);
	void measuredDataMovedDown(int i);

	void backgroundActorVisibilityChanged(int idx, bool vis);
};

#endif // PROJECTMAINFILE_H
