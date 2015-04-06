#ifndef SOLVERDEFINITION_H
#define SOLVERDEFINITION_H

#include "../guicore_global.h"
#include "solverdefinitionabstract.h"

#include <QObject>
#include <QString>
#include <QDomElement>
#include <QList>
#include <QMap>
#include <QDomDocument>

class SolverDefinitionTranslator;
class SolverDefinitionGridType;

/// SolverDefinition class contain information stored in "definition.xml" and
/// translation files.
/**
 * This class delegates some functions to SolverDefinitionAbstract .
 */
class GUICOREDLL_EXPORT SolverDefinition : public QObject
{
	Q_OBJECT

public:
	static const QString FILENAME;
	static const QString LICENSE;
	static const QString README;
	enum IterationType {
		NoIteration,
		ConvergenceIteration,
		TimeIteration,
	};
	/// Constructor
	SolverDefinition(const QString& solverfolder, const QLocale& locale)
		: m_abstract(solverfolder, locale, 0)
	{
		load(locale);
	}
	/// Destructor
	~SolverDefinition();
	/// Solver name in adequate locale
	const QString& name() const {return m_abstract.name();}
	/// Solver caption in adequate locale
	const QString& caption() const {return m_abstract.caption();}
	/// Solver version
	const VersionNumber& version() const {return m_abstract.version();}
	/// Description of solver
	const QString description() const {return m_abstract.description();}
	/// License of solver
	const QString license() const {return m_abstract.license();}
	/// Copyright
	const QString& copyright() const {return m_abstract.copyright();}
	/// Iteration type
	IterationType iterationType() const {return m_iterationType;}
	/// Executable filename in full path ex. "/usr/local/iRIC/solvers/nays2d/nays2d"
	const QString& executableFilename() const {return m_executableFilename;}
	/// The SolverDefinitionAbstract instance that is contained in this instance.
	const SolverDefinitionAbstract& abstract() const {return m_abstract;}
	/// The folder that contains files related to solver.
	const QDir& folder() const {return m_abstract.folder();}
	const QList<SolverDefinitionGridType*> gridTypes() const {return m_gridTypes;}
    SolverDefinitionGridType* dummyGridType() const {return m_dummyGridType;}
	SolverDefinitionGridType* gridType(const QString& name) const {return m_gridTypeNameMap.value(name);}
	const QDomDocument& document() const {return m_document;}

protected:
	/// Load information from solver defintion file and translation files.
	void load(const QLocale& locale);

private:
	void setIterationType(const QDomElement& elem);
	void setupGridTypes(const QDomNode& node, const SolverDefinitionTranslator& translator);
	SolverDefinitionGridType* setupGridType(const QDomNode& node, const SolverDefinitionTranslator& translator, bool isPrimary);

protected:
	SolverDefinitionAbstract m_abstract;
	IterationType m_iterationType;
	QString m_executableFilename;
	QList<SolverDefinitionGridType*> m_gridTypes;
    SolverDefinitionGridType* m_dummyGridType;
	QMap<QString, SolverDefinitionGridType*> m_gridTypeNameMap;
	QDomDocument m_document;
};

#endif // SOLVERDEFINITION_H
