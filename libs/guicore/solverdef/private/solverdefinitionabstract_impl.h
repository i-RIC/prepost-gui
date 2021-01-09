#ifndef SOLVERDEFINITIONABSTRACT_IMPL_H
#define SOLVERDEFINITIONABSTRACT_IMPL_H

#include "../solverdefinitionabstract.h"

#include <misc/versionnumber.h>

#include <QDate>
#include <QDir>
#include <QLocale>
#include <QString>

#include <string>

class SolverDefinitionAbstract::Impl
{
public:
	/// Constructor
	Impl(SolverDefinitionAbstract* parent);
	/// Destructor
	~Impl() {}
	void load(const QString& solverfolder, const QLocale& locale);

	QString m_folderName;
	QString m_absoluteFolderName;
	std::string m_name;
	QString m_caption;
	VersionNumber m_version;
	QString m_copyright;
	QString m_url;
	QDate m_release;
	QDir m_folder;
	QLocale m_locale;

	QString loadFile(const QString& filename);

private:
	SolverDefinitionAbstract* m_parent;
};

#endif // SOLVERDEFINITIONABSTRACT_IMPL_H
