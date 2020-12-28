#ifndef ENUMLOADERT_DETAIL_H
#define ENUMLOADERT_DETAIL_H

#include "../enumloadert.h"
#include "../solverdefinitiontranslator.h"

#include <misc/stringtool.h>

#include <QDomNode>
#include <QDomElement>

template <class V>
EnumLoaderT<V>::~EnumLoaderT()
{}

template <class V>
const QMap<V, QString>& EnumLoaderT<V>::enumerations() const
{
	return m_enumerations;
}

template <class V>
const QMap<V, std::string>& EnumLoaderT<V>::englishEnumerations() const
{
	return m_englishEnumerations;
}

template <class V>
QMap<QVariant, QString> EnumLoaderT<V>::variantEnumerations() const
{
	QMap<QVariant, QString> ret;
	for (auto it = m_enumerations.begin(); it != m_enumerations.end(); ++it) {
		ret.insert(QVariant(it.key()), it.value());
	}
	return ret;
}

template <class V>
void EnumLoaderT<V>::loadEnumeration(const QDomElement& elem, const SolverDefinitionTranslator& translator)
{
	QDomNode def = iRIC::getChildNode(elem, "Definition");
	if (def.isNull()) {return;}
	QDomNode enums = iRIC::getChildNode(def, "Enumerations");
	QDomNodeList children;
	if (! enums.isNull()) {
		children = enums.childNodes();
	} else {
		children = def.childNodes();
	}
	for (int i = 0; i < children.count(); ++i) {
		QDomNode child = children.at(i);
		if (child.nodeName() == "Enumeration") {
			QDomElement elem = child.toElement();
			V key = fromStr(elem.attribute("value"));
			QString caption = translator.translate(elem.attribute("caption"));
			m_enumerations.insert(key, caption);
			m_englishEnumerations.insert(key, iRIC::toStr(elem.attribute("caption")));
		}
	}
}

#endif // ENUMLOADERT_DETAIL_H
