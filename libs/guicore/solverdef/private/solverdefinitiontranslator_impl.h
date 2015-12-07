#ifndef SOLVERDEFINITIONTRANSLATOR_IMPL_H
#define SOLVERDEFINITIONTRANSLATOR_IMPL_H

#include "../solverdefinitiontranslator.h"

#include <QHash>

class SolverDefinitionTranslator::Impl
{
public:
	Impl(const QString& solverfolder, const QLocale& locale);
	void load(const QString& solverfolder, const QLocale& locale);

	QHash<QString, QString> m_dictionary;
};

#endif // SOLVERDEFINITIONTRANSLATOR_IMPL_H
