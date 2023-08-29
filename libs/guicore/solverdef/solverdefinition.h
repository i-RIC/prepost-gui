#ifndef SOLVERDEFINITION_H
#define SOLVERDEFINITION_H

#include "../guicore_global.h"
#include <QObject>

class QDir;
class QDomDocument;
class QString;
class SolverDefinitionGridType;
class SolverDefinitionAbstract;
class SolverDefinitionTranslator;
class VersionNumber;

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
	static const QString README;
	static const QString LICENSE;
	enum IterationType {
		NoIteration,
		ConvergenceIteration,
		TimeIteration,
	};
	SolverDefinition(const QString& solverfolder, const QLocale& locale);
	~SolverDefinition();
	/// Solver name in adequate locale
	const std::string& name() const;
	/// Solver caption in adequate locale
	const QString& caption() const;
	/// Solver version
	const VersionNumber& version() const;
	/// Description of solver
	QString description() const;
	/// License of solver
	QString license() const;
	/// Copyright
	const QString& copyright() const;
	/// Iteration type
	IterationType iterationType() const;
	/// Executable filename in full path ex. "/usr/local/iRIC/solvers/nays2d/nays2d"
	const QString& executableFilename() const;
	/// The SolverDefinitionAbstract instance that is contained in this instance.
	const SolverDefinitionAbstract& abstract() const;
	/// The folder that contains files related to solver.
	const QDir& folder() const;
	/// The list of grid types
	const QList<SolverDefinitionGridType*>& gridTypes() const;
	std::vector<SolverDefinitionGridType*> preGridTypes() const;

	SolverDefinitionGridType* dummyGridType() const;
	/// Get grid type from name
	SolverDefinitionGridType* gridType(const std::string& name) const;
	/// XML Document
	const QDomDocument& document() const;

	SolverDefinitionTranslator buildTranslator() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/solverdefinition_impl.h"
#endif // _DEBUG

#endif // SOLVERDEFINITION_H
