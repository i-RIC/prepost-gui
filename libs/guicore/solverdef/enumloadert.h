#ifndef ENUMLOADERT_H
#define ENUMLOADERT_H

#include "enumloader.h"

#include <misc/xmlsupport.h>

#include <QMap>
#include <QDomNodeList>
#include <QVariant>

#include <string>

class SolverDefinitionTranslator;
class QDomElement;

template <class V>
class EnumLoaderT : public EnumLoader
{
public:
	virtual ~EnumLoaderT();

	void addEnumeration(V value, const std::string& english, const QString& caption);

	const QMap<V, QString>& enumerations() const;
	const QMap<V, std::string>& englishEnumerations() const;
	QMap<QVariant, QString> variantEnumerations() const override;

protected:
	void loadEnumeration(const QDomElement& elem, const SolverDefinitionTranslator& translator);

private:
	virtual V fromStr(const QString& str) const = 0;

	QMap<V, QString> m_enumerations;
	QMap<V, std::string> m_englishEnumerations;
};

#include "private/enumloadert_detail.h"

#endif // ENUMLOADERT_H
