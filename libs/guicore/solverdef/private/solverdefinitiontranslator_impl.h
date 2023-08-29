#ifndef SOLVERDEFINITIONTRANSLATOR_IMPL_H
#define SOLVERDEFINITIONTRANSLATOR_IMPL_H

#include "../solverdefinitiontranslator.h"

#include <QHash>
#include <QLocale>

class SolverDefinitionTranslator::Impl
{
public:
	Impl();
	Impl(const QString& solverfolder, const QLocale& locale);
	void load(const QString& solverfolder, const QLocale& locale);

	QHash<QString, QString> m_dictionary;
	QLocale m_locale;
};

#endif // SOLVERDEFINITIONTRANSLATOR_IMPL_H
