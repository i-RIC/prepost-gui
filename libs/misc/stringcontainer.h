#ifndef STRINGCONTAINER_H
#define STRINGCONTAINER_H

#include "misc_global.h"
#include "simplevaluecontainert.h"

#include <QString>

class MISCDLL_EXPORT StringContainer : public SimpleValueContainerT<QString>
{

public:
	/// @name Constructors and Destructor
	//@{
	/// Constructor
	StringContainer(const QString& name);
	/// Constructor (with default value)
	StringContainer(const QString& name, const QString& defaultVal);
	/// Constructor (copy)
	StringContainer(const StringContainer& c);
	/// Destructor
	virtual ~StringContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	/// Save data into XML node as attributes
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
