#ifndef TMSIMAGESETTING_IMPL_H
#define TMSIMAGESETTING_IMPL_H

#include "../tmsimagesetting.h"

#include <QString>

#include <map>

class TmsImageSetting::Impl
{
public:
	std::map<std::string, QString> m_settings;
};

#endif // TMSIMAGESETTING_IMPL_H

