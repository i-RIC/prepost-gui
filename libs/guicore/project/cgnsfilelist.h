#ifndef CGNSFILELIST_H
#define CGNSFILELIST_H

#include "../guicore_global.h"
#include "projectdataitem.h"
#include <QList>
#include <QMap>
#include <QDir>

/// This class handles the list of CGNS files in the project.
class GUICOREDLL_EXPORT CgnsFileList : public ProjectDataItem
{
	Q_OBJECT
public:
	/// This class handles the information about a CGNS file in the project.
	class CgnsFileEntry : public ProjectDataItem
	{
	public:
		/// Constructor
		CgnsFileEntry(ProjectDataItem* parent) : ProjectDataItem(parent) {}
		/// Constructor
		CgnsFileEntry(const QString& filename, ProjectDataItem* parent) : ProjectDataItem(parent) {
			m_filename = filename;
		}
		/// file name (".cgn" is not included. For example "calculation1".
		const QString& filename() {return m_filename;}
		/// Comment for this calculation
		const QString& comment() {return m_comment;}
	protected:
		void doLoadFromProjectMainFile(const QDomNode& node);
		void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	private:
		QString m_filename;
		QString m_comment;
	};
	/// Constructor
	CgnsFileList(ProjectDataItem* parent) : ProjectDataItem(parent) {
		m_current = nullptr;
	}
	~CgnsFileList();
	QList<CgnsFileEntry*> cgnsFiles() const;
	CgnsFileEntry* current() const {return m_current;}
	bool exists(const QString& name) const;
	CgnsFileEntry* setCurrent(const QString& name);
	/// Add new cgns file.
	void add(const QString& name);
	/// Remove existing cgns file.
	void remove(const QString& name);
	QStringList containedFiles() override;
	/// Propose a file name for next cgns to create.
	QString proposeFilename();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

signals:
	void cgnsFilesUpdated(QList<CgnsFileList::CgnsFileEntry*>);
	void cgnsFileSwitched(QString newname);
private:
	QMap<QString, CgnsFileEntry*> m_cgnsFiles;
	CgnsFileEntry* m_current;
};

#endif // CGNSFILELIST_H
