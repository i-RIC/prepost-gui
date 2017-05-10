#ifndef SOLVERDEFINITION_IMPL_H
#define SOLVERDEFINITION_IMPL_H

#include "../solverdefinition.h"
#include "../solverdefinitionabstract.h"

#include <QDomDocument>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QString>

#include <string>

class SolverDefinitionTranslator;
class QDomNode;

class SolverDefinition::Impl
{
public:
	Impl(const QString& solverfolder, const QLocale& locale, SolverDefinition* p);
	~Impl();

	/// Load information from solver defintion file and translation files.
	void load();
	void setupIterationType(const QDomElement& elem);
	void setupGridTypes(const QDomNode& node, const SolverDefinitionTranslator& translator);
	SolverDefinitionGridType* setupGridType(const QDomNode& node, bool isPrimary);
	SolverDefinitionTranslator buildTranslator() const;

	IterationType m_iterationType;
	QString m_executableFilename {};
	QList<SolverDefinitionGridType*> m_gridTypes {};
	SolverDefinitionGridType* m_dummyGridType {nullptr};
	QMap<std::string, SolverDefinitionGridType*> m_gridTypeNameMap {};
	QDomDocument m_document {};
	QLocale m_locale;

	SolverDefinitionAbstract m_abstract;

private:
	SolverDefinition* m_parent;
};

#endif // SOLVERDEFINITION_IMPL_H
