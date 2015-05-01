#ifndef SOLVERDEFINITIONTRANSLATOR_H
#define SOLVERDEFINITIONTRANSLATOR_H

#include "../guicore_global.h"
#include <QString>
#include <QLocale>
#include <QHash>

/// This class represents translator for Solver Definition files
class GUICOREDLL_EXPORT SolverDefinitionTranslator
{
public:
	/// Constructor
	SolverDefinitionTranslator(const QString& solverfolder, const QLocale& locale) {
		load(solverfolder, locale);
	}
	/// Returns translated string corresponsing to the source string
	/**
	 * @param src Source string to translate
	 * @return Translated string or source string (if the corresponsing translation
	 * was not found)
	 */
	const QString translate(const QString& src) const;
	/// Returns translation file name (like "translation_ja_JP.xml") from locale
	/**
	 * @param locale Locale object
	 * @return Translation file name
	 */
	static QString filenameFromLocale(const QLocale& locale);

private:
	/**
	 * @todo Implement this function!
	 *
	 * The file format of Solver definition translation files should be equal to
	 * that of Qt translation files, to make it possible to edit translation files
	 * with Qt Linguist.
	 */
	void load(const QString& solverfolder, const QLocale& locale);
	QHash<QString, QString> m_dictionary;
};

#endif // SOLVERDEFINITIONTRANSLATOR_H
