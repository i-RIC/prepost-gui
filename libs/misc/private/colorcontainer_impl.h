#ifndef COLORCONTAINER_IMPL_H
#define COLORCONTAINER_IMPL_H

#include "../colorcontainer.h"
#include "../simplevaluecontainert.h"

#include <QColor>

class ColorContainer::Impl : public SimpleValueContainerT<QColor>
{
public:
	Impl(const QString& name);
	Impl(const QString& name, const QColor& defaultVal);
	Impl(const Impl& i);
	~Impl();

	void load(const QDomNode& node);
	void save(QXmlStreamWriter& writer) const;
};

#endif // COLORCONTAINER_IMPL_H
