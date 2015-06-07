#ifndef SOLVERDEFINITIONABSTRACT_H
#define SOLVERDEFINITIONABSTRACT_H

#include "../guicore_global.h"

#include <QObject>
#include <QDir>
#include <QString>
#include <QLocale>
#include <QDate>

#include <misc/versionnumber.h>

class QDomDocument;

/// SolverDefinitionAbstract class stores abstract information of a solver
/**
 * This class loads only those information necessary to show on
 * solver selection menu, from "definition.xml"
 */
class GUICOREDLL_EXPORT SolverDefinitionAbstract : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	/**
	 * @param solverfolder Full path of the folder that stores
	 * solver definition file and translation files of that, and of course,
	 * the solver executable. ex. "/usr/local/iRIC/solvers/nays2d"
	 *
	 * @param locale Locale to specify which translation should be used.
	 * if the corresponding translation file is not found, ignored and
	 * the strings in "definition.xml" is used.
	 */
	SolverDefinitionAbstract(const QString& solverfolder, const QLocale& locale, QObject* parent)
		: QObject(parent) {
		load(solverfolder, locale);
	}
	/// The name of the folder which contains files related to the solver
	/// (ex. definition.xml, translation_ja_JP.xml, solver.exe)
	const QString& folderName() {return m_folderName;}
	/// Solver name
	const QString& name() const {return m_name;}
	/// Solver caption in adequate locale
	const QString& caption() const {return m_caption;}
	/// Solver version
	const VersionNumber& version() const {return m_version;}
	/// Description of solver
	const QString description() const;
	/// Url of the homepage of the solver
	const QString& url() const {return m_url;}
	/// Copyright
	const QString& copyright() const {return m_copyright;}
	/// Release date
	const QDate& release() const {return m_release;}
	/// License information
	const QString license() const;
	/// Folder object
	const QDir& folder() const {return m_folder;}
	void setupDomDocument(QDomDocument* doc) const;

protected:
	/// Load information from solver defintion file and translation files.
	void load(const QString& solverfolder, const QLocale& locale);

private:
	QString m_folderName;
	QString m_name;
	QString m_caption;
	VersionNumber m_version;
	QString m_copyright;
	QString m_url;
	QDate m_release;
	QDir m_folder;
	QLocale m_locale;
};

#endif // SOLVERDEFINITIONABSTRACT_H
