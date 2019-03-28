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

PostParticleBasicSetting& PostParticleBasicSetting::operator=(const PostParticleBasicSetting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}
