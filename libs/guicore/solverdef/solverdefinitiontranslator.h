#ifndef SOLVERDEFINITIONTRANSLATOR_H
#define SOLVERDEFINITIONTRANSLATOR_H

#include "../guicore_global.h"

class QLocale;
class QString;

/// This class represents translator for Solver Definition files
class GUICOREDLL_EXPORT SolverDefinitionTranslator
{

public:
	/// Constructor
	SolverDefinitionTranslator(const QString& solverfolder, const QLocale& locale);
	/// Destructor
	~SolverDefinitionTranslator();
	/// Returns translated string corresponsing to the source string
	/**
	 * @param src Source string to translate
	 * @return Translated string or source string (if the corresponsing translation
	 * was not found)
	 */
	QString translate(const QString& src) const;
	/// Returns translation file name (like "translation_ja_JP.xml") from locale
	/**
	 * @param locale Locale object
	 * @return Translation file name
	 */
	static QString filenameFromLocale(const QLocale& locale);

private:
	class Impl;
	Impl* m_impl;
};

#ifdef _DEBUG
	#include "private/solverdefinitiontranslator_impl.h"
#endif _DEBUG

#endif // SOLVERDEFINITIONTRANSLATOR_H
