#ifndef POST2DWINDOWPARTICLEIMAGEDATAITEM_VALUESIZEPAIR_H
#define POST2DWINDOWPARTICLEIMAGEDATAITEM_VALUESIZEPAIR_H

#include "../post2dwindowparticleimagedataitem.h"

#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>

class Post2dWindowParticleImageDataItem::ValueSizePair : public CompositeContainer
{
public:
	ValueSizePair();
	ValueSizePair(const ValueSizePair& pair);
	ValueSizePair(double value, double size);

	ValueSizePair& operator=(const ValueSizePair& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	DoubleContainer value;
	DoubleContainer size;
};

#endif // POST2DWINDOWPARTICLEIMAGEDATAITEM_VALUESIZEPAIR_H
