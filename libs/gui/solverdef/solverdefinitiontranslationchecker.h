#ifndef SOLVERDEFINITIONTRANSLATIONCHECKER_H
#define SOLVERDEFINITIONTRANSLATIONCHECKER_H

#include <QMap>
#include <QSet>

class QDomNode;
class QLocale;
class QXmlStreamWriter;

/// This class implements the checking logics for solver definition translation files.
/**
 * SolverDefinitionTranslationChecker class loads a solver definition file and
 * a translation file that is bundled with it, and check the followings:
 * - Whether there're lacking translations in the translation file
 * - Whether there're translations those are not used any more.
 *
 * Then, it overwrites the translation file, with the following modifications:
 *
 * - The words that lacks translations are added.
 * - The words that are not used any more, are flagged so.
 */
class SolverDefinitionTranslationChecker
{

public:
	/// This class is a container class that contains the translation messages.
	/**
	 * The main contents of this class are :
	 * - Source string in the original language (English)
	 * - Translated message (in Japanese, French etc.)
	 */
	class TranslationMessage
	{

	public:
		/// An enumeration that defines the types of messages
		enum MessageType {
			TypeNormal,          ///< The messages already translated and actually used.
			TypeUnfinished,      ///< The messages not yet translated, or not yet checked.
			TypeObsolete,        ///< THe messages not used anymore.
		};
		/// Constructor (without translation)
		TranslationMessage(const QString& src) {
			m_source = src;
			m_messageType = TypeUnfinished;
		}
		/// Constructor (with translation)
		TranslationMessage(const QString& src, const QString& trans) {
			m_source = src;
			m_translation = trans;
			m_messageType = TypeNormal;
			if (m_translation == "") {m_messageType = TypeUnfinished;}
		}
		/// Constructor (with translation and type)
		TranslationMessage(const QString& src, const QString& trans, MessageType type) {
			m_source = src;
			m_translation = trans;
			m_messageType = type;
		}
		/// Save the information contained in this instance into translation file.
		void save(QXmlStreamWriter& writer) const;
		/// The source message.
		const QString& source() const {return m_source;}
		/// The translated message.
		const QString& translation() const {return m_translation;}
		/// The type of message.
		MessageType messageType() const {return m_messageType;}

	private:
		MessageType m_messageType;
		QString m_source;
		QString m_translation;
	};
	/// Constructor
	SolverDefinitionTranslationChecker(const QString& solverfolder, const QLocale& locale) {
		load(solverfolder, locale);
		save(solverfolder, locale);
	}
	/// Overwrites the translation file with modifications to reflect the check result.

private:
	/// Load a solver definition file and a translation file.
	void load(const QString& solverfolder, const QLocale& locale);
	/// Save the updated translation file.
	void save(const QString& solverfolder, const QLocale& locale);
	/// Scan a DOM node in solver definition file recursively and
	/// find strings to translate.
	void scanDefinition(const QDomNode& node);
	/// The dictionary in translation file.
//	QHash<QString, QString> m_dictionary;
	/// The words those has no correnponding translation in translation file.
//	QSet<QString> m_lackingTranslations;
	/// The words those appear in the translation file, but not in the solver definition file.
	QMap<QString, QString> m_uselessTranslations;

	QSet<QString> m_dic;
	QList<TranslationMessage> m_messages;
};

#endif // SOLVERDEFINITIONTRANSLATIONCHECKER_H
