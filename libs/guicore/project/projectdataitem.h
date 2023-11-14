#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include "../guicore_global.h"

#include <QObject>
#include <QDataStream>

#include <memory>

class iRICMainWindowI;
class ProjectData;

class QColor;
class QDomNode;
class QPointF;
class QXmlStreamWriter;

class vtkCamera;

/// Abstract class that store data in project.xml.
class GUICOREDLL_EXPORT ProjectDataItem : public QObject
{
	Q_OBJECT

protected:
	static const QDataStream::Version dataStreamVersion;

public:
	/// @name Constructors and destructors
	//@{
	/// Constructor
	explicit ProjectDataItem(ProjectDataItem* d);
	/// Constructor
	ProjectDataItem(const QString& filename, ProjectDataItem* d);
	/// Destructor
	virtual ~ProjectDataItem();
	//@}

	/// @name I/O interface against project.xml
	//@{
	/// Load data from project main file
	virtual void loadFromProjectMainFile(const QDomNode& node);
	/// Save data into project main file
	virtual void saveToProjectMainFile(QXmlStreamWriter& writer);
	//@}

	/// @name I/O interface against CGNS file
	//@{
	/// Currently opened CGNS file name
	virtual QString currentCgnsFileName() const;
	/// Load data from CGNS file
	virtual int loadFromCgnsFile();
	/// Save data into CGNS file
	virtual int saveToCgnsFile();
	/// Update data in CGNS file other than grids
	virtual int updateCgnsFileOtherThanGrids();
	/// Discard data loaded from CGNS file.
	virtual void closeCgnsFile();
	//@}

	/// @name I/O interface against external files
	//@{
	/// Returns the filename in an absolute path.
	QString filename() const;
	/// Returns the filename in relative path against workDirectory.
	QString relativeFilename() const;
	/// Set the filename. It does not include path information.
	void setFilename(const QString& fname);
	/// External file names that are used to store data below this node
	virtual QStringList containedFiles() const;
	//@}

	/// @name Utility functions
	//@{
	/// Project data
	virtual ProjectData* projectData() const;
	/// Parent item
	ProjectDataItem* parent() const;
	/// iRIC main window
	iRICMainWindowI* iricMainWindow() const;
	//@}

protected:
	/// @name Internal Utility functions
	//@{
	/// Set this project modified
	virtual void setModified(bool modified = true);
	/// Load filename from project.xml
	void loadFilename(const QDomNode& node);
	/// Save filename into project.xml
	void saveFilename(QXmlStreamWriter& writer) const;
	/// Load external data.
	void loadExternalData();
	/// Relative subpath
	virtual QString relativeSubPath() const;
	/// Sub path of the external file
	QString subPath() const;
	/// Set sub path
	void setSubPath(const QString& subPath);
	/// Offset applied to the data
	QPointF offset() const;
	/// Apply offset the current data.
	virtual void doApplyOffset(double x, double y);
	//@}

private:
	/// @name Virtual functions to implement project.xml I/O function
	//@{
	/// Load information to restore data from ProjectMainFile.
	virtual void doLoadFromProjectMainFile(const QDomNode& node) = 0;
	/// Write into projectmainfile.
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& writer) = 0;
	//@}

	/// @name Vertual functions to implement I/O functions against external files
	//@{
	/// Load external data from the file specified by m_filename.
	virtual void loadExternalData(const QString& filename);
	/// Save external data into the file specified by m_filename.
	virtual void saveExternalData(const QString& filename);
	//@}

public:
	/// @name Utility functions for project.xml I/O. Caution! These functions are deprecated and will be removed!
	//@{
	/// Load color attribute from project.xml.
	static QColor loadColorAttribute(const QString& name, const QDomNode&, const QColor& defaultColor);
	/// Save color attribute to project.xml.
	static void writeColorAttribute(const QString& name, const QColor& col, QXmlStreamWriter& writer);
	/// Load background color from project.xml.
	static QColor loadBackgroundColor(const QDomNode& node, const QColor& defaultColor);
	/// Save background color to project.xml.
	static void writeBackgroundColor(QColor c, QXmlStreamWriter& writer);
	/// Load camera setting from project.xml.
	static void loadCamera(vtkCamera* camera, const QDomNode& node);
	/// Save camera setting to project.xml.
	static void saveCamera(vtkCamera* camera, QXmlStreamWriter& writer);
	//@}

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/projectdataitem_impl.h"
#endif // _DEBUG

#endif // PROJECTDATAITEM_H
