#ifndef TMSIMAGESETTING_IMPL_H
#define TMSIMAGESETTING_IMPL_H

#include "../tmsimagesetting.h"

#include <QString>

class TmsImageSetting::Impl
{
public:
	Impl(const std::string& setting);

	std::string m_setting;
	QString m_caption;
	bool m_isActive;
};

#endif // TMSIMAGESETTING_IMPL_H

