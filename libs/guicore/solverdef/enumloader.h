#ifndef ENUMLOADER_H
#define ENUMLOADER_H

class EnumLoader
{
public:
	virtual ~EnumLoader() {}

	virtual QMap<QVariant, QString> variantEnumerations() const = 0;
};

#endif // ENUMLOADER_H
