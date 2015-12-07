#ifndef SOLVERDEFINITION_IMPL_H
#define SOLVERDEFINITION_IMPL_H

#include "../solverdefinition.h"
#include "../solverdefinitionabstract.h"

#include <QDomDocument>
#include <QList>
#include <QMap>
#include <QString>

#include <string>

class SolverDefinitionTranslator;
class QDomNode;

class SolverDefinition::Impl
{
public:
	/// Constructor
	Impl(const QString& solverfolder, const QLocale& locale, SolverDefinition* p);
	/// Destructor
	~Impl();

	/// Load information from solver defintion file and translation files.
	void load(const QLocale& locale);
	void setupIterationType(const QDomElement& elem);
	void setupGridTypes(const QDomNode& node, const SolverDefinitionTranslator& translator);
	SolverDefinitionGridType* setupGridType(const QDomNode& node, const SolverDefinitionTranslator& translator, bool isPrimary);

	IterationType m_iterationType;
	QString m_executableFilename {};
	QList<SolverDefinitionGridType*> m_gridTypes {};
	SolverDefinitionGridType* m_dummyGridType {nullptr};
	QMap<std::string, SolverDefinitionGridType*> m_gridTypeNameMap {};
	QDomDocument m_document {};

	SolverDefinitionAbstract m_abstract;

private:
	SolverDefinition* m_parent;
};

#endif // SOLVERDEFINITION_IMPL_H
