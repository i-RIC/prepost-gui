#ifndef CGNSFILELIST_H
#define CGNSFILELIST_H

#include "../guicore_global.h"
#include "projectdataitem.h"
#include <QList>
#include <QMap>
#include <QDir>

class CgnsFileEntry;

/// This class handles the list of CGNS files in the project.
class GUICOREDLL_EXPORT CgnsFileList : public ProjectDataItem
{
	Q_OBJECT

public:
	CgnsFileList(ProjectDataItem* parent);
	~CgnsFileList();

	QList<CgnsFileEntry*> cgnsFiles() const;

	CgnsFileEntry* current() const;
	CgnsFileEntry* setCurrent(const QString& name);

	bool exists(const QString& name) const;

	/// Add new cgns file.
	void add(const QString& name);
	/// Remove existing cgns file.
	void remove(const QString& name);
	QStringList containedFiles() override;
	/// Propose a file name for next cgns to create.
	QString proposeFilename();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

signals:
	void cgnsFilesUpdated(QList<CgnsFileEntry*>);
	void cgnsFileSwitched(QString newname);

private:
	QMap<QString, CgnsFileEntry*> m_cgnsFiles;
	CgnsFileEntry* m_current;
};

#endif // CGNSFILELIST_H
