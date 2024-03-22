#include "post2dwindowparticleimagedataitem_valuesizepair.h"

Post2dWindowParticleImageDataItem::ValueSizePair::ValueSizePair() :
	ValueSizePair {0, 0}
{}

Post2dWindowParticleImageDataItem::ValueSizePair::ValueSizePair(const ValueSizePair& pair) :
	ValueSizePair {}
{
	copyValue(pair);
}

Post2dWindowParticleImageDataItem::ValueSizePair::ValueSizePair(double v, double s) :
	CompositeContainer ({&value, &size}),
	value {"value", v},
	size {"size", s}
{}

Post2dWindowParticleImageDataItem::ValueSizePair& Post2dWindowParticleImageDataItem::ValueSizePair::operator=(const ValueSizePair& c)
{
	copyValue(c);
	return *this;

}

XmlAttributeContainer& Post2dWindowParticleImageDataItem::ValueSizePair::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ValueSizePair&> (c));
}
