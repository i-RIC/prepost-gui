#ifndef QFONTCONTAINER_H
#define QFONTCONTAINER_H

#include "misc_global.h"
#include "simplevaluecontainert.h"

#include <QFont>

class MISCDLL_EXPORT QFontContainer : public SimpleValueContainerT<QFont>
{
public:
	QFontContainer(const QString& name);
	QFontContainer(const QString& name, const QFont& defaultVal);
	QFontContainer(const QFontContainer& c);
	~QFontContainer();

	/// @name XML I/O functions
	//@{
	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;
	//@}

	/// @name Operators
	//@{
	QFontContainer& operator=(const QFont& val);
	bool operator!=(const QFont& val);
	//@}

private:
	void copyValue(const XmlAttributeContainer& c) override;
};

#endif // QFONTCONTAINER_H
