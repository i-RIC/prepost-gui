#ifndef ENUMLOADER_H
#define ENUMLOADER_H

#include <QMap>
#include <misc/xmlsupport.h>
#include "solverdefinitiontranslator.h"

class QDomElement;

template <class V>
class EnumLoader {
public:
	const QMap<V, QString>& enumerations() const {return m_enumerations;}
	const QMap<V, QString>& englishEnumerations() const {return m_englishEnumerations;}
protected:
	virtual void loadEnumeration(const QDomElement& node, const SolverDefinitionTranslator& translator)
	{
		QDomNode def = iRIC::getChildNode(node, "Definition");
		if (def.isNull()){return;}
		QDomNode enums = iRIC::getChildNode(def, "Enumerations");
		QDomNodeList children;
		if (! enums.isNull()){
			children = enums.childNodes();
		} else {
			children = def.childNodes();
		}
		for (int i = 0; i < children.count(); ++i){
			QDomNode child = children.at(i);
			if (child.nodeName() == "Enumeration"){
				QDomElement elem = child.toElement();
				V key = fromStr(elem.attribute("value"));
				QString caption = translator.translate(elem.attribute("caption"));
				m_enumerations.insert(key, caption);
				m_englishEnumerations.insert(key, elem.attribute("caption"));
			}
		}
	}

	virtual V fromStr(const QString& str) = 0;
	QMap<V, QString> m_enumerations;
	QMap<V, QString> m_englishEnumerations;
};

class IntegerEnumLoader : public EnumLoader<int> {
protected:
	int fromStr(const QString& str){return str.toInt();}
};

class RealEnumLoader : public EnumLoader<double> {
protected:
	double fromStr(const QString& str){return str.toDouble();}
};

#endif // ENUMLOADER_H
