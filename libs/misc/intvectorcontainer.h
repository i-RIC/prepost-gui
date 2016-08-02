#ifndef INTVECTORCONTAINER_H
#define INTVECTORCONTAINER_H

#include "misc_global.h"
#include "simplevaluecontainert.h"

#include <vector>

class MISCDLL_EXPORT IntVectorContainer : public SimpleValueContainerT<std::vector<int> >
{
public:
	/// @name Constructors and Destructor
	//@{
	IntVectorContainer(const QString& name);
	IntVectorContainer(const QString& name, const std::vector<int>& defaultVal);
	IntVectorContainer(const IntVectorContainer& c);
	~IntVectorContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;
	//@}

	/// @name Operators
	IntVectorContainer& operator=(const std::vector<int>& val);

private:
	void copyValue(const XmlAttributeContainer& c) override;
};

#endif // INTVECTORCONTAINER_H
