#ifndef STRINGCONTAINER_H
#define STRINGCONTAINER_H

#include "misc_global.h"
#include "simplevaluecontainert.h"

#include <QString>

#include <string>

class MISCDLL_EXPORT StringContainer : public SimpleValueContainerT<QString>
{
public:
	/// @name Constructors and Destructor
	//@{
	StringContainer(const QString& name);
	StringContainer(const QString& name, const QString& defaultVal);
	StringContainer(const StringContainer& c);
	virtual ~StringContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	//@{
	StringContainer& operator=(const QString& val);
	bool operator!=(const QString& val);
	operator std::string() const;
	//@}

private:
	void copyValue(const XmlAttributeContainer& c) override;
};

#endif // STRINGCONTAINER_H
