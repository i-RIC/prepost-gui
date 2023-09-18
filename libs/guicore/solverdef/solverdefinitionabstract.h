#ifndef SOLVERDEFINITIONABSTRACT_H
#define SOLVERDEFINITIONABSTRACT_H

#include "../guicore_global.h"
#include <QObject>

class QDomDocument;
class QDate;
class QDir;
class QString;
class VersionNumber;

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
	SolverDefinitionAbstract(const QString& solverfolder, const QLocale& locale, QObject* parent);
	~SolverDefinitionAbstract();

	/// The name of the folder which contains files related to the solver
	/// (ex. definition.xml, translation_ja_JP.xml, solver.exe)
	const QString& folderName() const;
	const QString& absoluteFolderName() const;
	/// Solver name
	const std::string& name() const;
	/// Solver caption in adequate locale
	const QString& caption() const;
	/// Solver version
	const VersionNumber& version() const;
	/// Description of solver
	QString description() const;
	/// Url of the homepage of the solver
	const QString& url() const;
	/// Copyright
	const QString& copyright() const;
	/// Release date
	const QDate& release() const;
	/// License information
	QString license() const;
	/// Folder object
	const QDir& folder() const;

	/// Setup dom document
	void setupDomDocument(QDomDocument* doc) const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/solverdefinitionabstract_impl.h"
#endif // _DEBUG

#endif // SOLVERDEFINITIONABSTRACT_H
