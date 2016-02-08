#ifndef TMSIMAGESETTING_H
#define TMSIMAGESETTING_H

#include "../guicore_global.h"

#include <string>

class QString;

class GUICOREDLL_EXPORT TmsImageSetting
{
public:
	TmsImageSetting();
	TmsImageSetting(const std::string& setting);
	TmsImageSetting(const TmsImageSetting& setting);

	~TmsImageSetting();

	TmsImageSetting& operator=(const TmsImageSetting& setting);

	std::string setting() const;
	QString caption() const;

	bool isActive() const;
	void setIsActive(bool active);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/tmsimagesetting_impl.h"
#endif // _DEBUG

#endif // TMSIMAGESETTING_H
