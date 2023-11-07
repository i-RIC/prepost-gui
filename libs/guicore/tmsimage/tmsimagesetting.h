#ifndef TMSIMAGESETTING_H
#define TMSIMAGESETTING_H

#include "../guicore_global.h"

#include <string>

class QString;

class GUICOREDLL_EXPORT TmsImageSetting
{
public:
	TmsImageSetting();
	TmsImageSetting(const TmsImageSetting& setting);

	~TmsImageSetting();

	TmsImageSetting& operator=(const TmsImageSetting& setting);

	QString setting() const;

	QString caption() const;
	bool isXYZ() const;
	QString url() const;
	int maxZoomLevel() const;

	bool isActive() const;
	void setIsActive(bool active);

	QString value(const std::string& key) const;
	void setValue(const std::string& key, const QString& value);

	static TmsImageSetting buildFromQuery(const QString& setting);
	static TmsImageSetting buildFromString(const QString& setting);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/tmsimagesetting_impl.h"
#endif // _DEBUG

#endif // TMSIMAGESETTING_H
