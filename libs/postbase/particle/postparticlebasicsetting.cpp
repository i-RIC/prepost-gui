#include "postparticlebasicsetting.h"

#include <QColor>
#include <QSettings>

PostParticleBasicSetting::PostParticleBasicSetting() :
	CompositeContainer ({&color, &size}),
	color {"color"},
	size {"size"}
{
	QSettings settings;
	color = settings.value("graphics/particlecolor", QColor(Qt::black)).value<QColor>();
	size = settings.value("graphics/particlesize", 3).toInt();
}

PostParticleBasicSetting::PostParticleBasicSetting(const PostParticleBasicSetting& s) :
	PostParticleBasicSetting()
{
	CompositeContainer::copyValue(s);
}

XmlAttributeContainer& PostParticleBasicSetting::operator=(const XmlAttributeContainer& c)
{
	CompositeContainer::copyValue(dynamic_cast<const PostParticleBasicSetting&> (c));
	return *this;
}
