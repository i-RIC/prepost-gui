#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include "../guicore_global.h"

#include <QObject>
#include <QStringList>
#include <QDataStream>
#include <QXmlStreamWriter>

class ProjectData;
class QDomNode;
class QXmlStreamWriter;
class QPoint;
class QColor;
class iRICMainWindowInterface;
class vtkCamera;
class QVector2D;

/// Abstract class that store data in project.xml.
class GUICOREDLL_EXPORT ProjectDataItem : public QObject
{
	Q_OBJECT
protected:
	static const QDataStream::Version dataStreamVersion = QDataStream::Qt_4_6;

public:
	/// Constructor
	explicit ProjectDataItem(ProjectDataItem *d) : QObject(d){
		m_subFolder = "";
		m_filename = "";
	}
	/// Constructor
	ProjectDataItem(const QString& filename, ProjectDataItem* d) : QObject(d){
		m_filename = filename;
	}
	/// Destructor
	virtual ~ProjectDataItem(){}
	/// Load data from project main file
	virtual void loadFromProjectMainFile(const QDomNode& node);
	/// Save data into project main file
	virtual void saveToProjectMainFile(QXmlStreamWriter& writer);
	/// Load data from CGNS file
	virtual void loadFromCgnsFile(const int /*fn*/){}
	/// Save data into CGNS file
	virtual void saveToCgnsFile(const int /*fn*/){}
	/// Discard data loaded from CGNS file.
	virtual void closeCgnsFile(){}
	virtual QStringList containedFiles();
	ProjectDataItem* parent() const {return dynamic_cast<ProjectDataItem*>(QObject::parent());}
	iRICMainWindowInterface* iricMainWindow();
	static bool isNear(const QPoint& p1, const QPoint& p2);
	virtual const QString currentCgnsFileName() const {return parent()->currentCgnsFileName();}
	void loadExternalData();

protected:
	/// Load information to restore data from ProjectMainFile.
	virtual void doLoadFromProjectMainFile(const QDomNode& node) = 0;
	/// Write into projectmainfile.
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& writer) = 0;
	virtual void setModified(){parent()->setModified();}
	/// Load additinal information from external data file.
	/**
	 * Load external data from the file specified by m_filename.
	 */
	virtual void loadExternalData(const QString& /*filename*/){}
	/// Save additinal information into external data file.
	/**
	 * Save external data into the file specified by m_filename.
	 */
	virtual void saveExternalData(const QString& /*filename*/){}
	virtual ProjectData* projectData() const {return parent()->projectData();}
	virtual QString relativeSubPath() const;
	QString subPath() const;
	/// Load file name from project.xml.
	void loadFilename(const QDomNode& node);
	/// Save file name to project.xml
	void saveFilename(QXmlStreamWriter& writer) const;
	virtual void doApplyOffset(double /*x*/, double /*y*/){}
	QVector2D offset();
	/// Load Window geometry from project.xml.
	static void loadWindowGeometry(QWidget* w, const QDomNode& node);
	/// Save window geometry to project.xml.
	static void writeWindowGeometry(QWidget* w, QXmlStreamWriter& writer);
	/// Load color attribute from project.xml.
	static QColor loadColorAttribute(const QString& name, const QDomNode&, const QColor& defaultColor);
	/// Save color attribute to project.xml.
	static void writeColorAttribute(const QString& name, const QColor& col, QXmlStreamWriter& writer);
	/// Load background color from project.xml.
	static QColor loadBackgroundColor(const QDomNode& node, const QColor& defaultColor);
	/// Save background color to project.xml.
	static void writeBackgroundColor(QColor c, QXmlStreamWriter& writer);
	/// Load opacity color from project.xml.
	static int loadOpacityPercent(const QDomNode& node);
	/// Save opacity color to project.xml.
	static void writeOpacityPercent(int opacity, QXmlStreamWriter& writer);
	/// Load camera setting from project.xml.
	static void loadCamera(vtkCamera* camera, const QDomNode& node);
	/// Save camera setting to project.xml.
	static void saveCamera(vtkCamera* camera, QXmlStreamWriter& writer);
	/// Returns the filename in an absolute path.
	const QString filename();
	/// Returns the filename in relative path against workDirectory.
	const QString relativeFilename();
	void setFilename(const QString& fname){m_filename = fname;}
protected:
	/// The name of file to store additional information about this class.
	/**
	 * m_filename = "" when it does not any file to store information beside project main file.
	 * The file name is stored as a relative path against the project workdirectory, that
	 * is available from ProjectData::workDirectory().
	 */
	QString m_filename;
	QString m_subFolder;
};

#endif // PROJECTDATAITEM_H
