#ifndef TMSIMAGESETTINGMANAGER_H
#define TMSIMAGESETTINGMANAGER_H

#include "../guicore_global.h"

#include <QObject>

#include <vector>

namespace tmsloader {
	class TmsRequest;
} // tmsloader

class TmsImageSetting;

class GUICOREDLL_EXPORT TmsImageSettingManager : public QObject
{
	Q_OBJECT

public:
	TmsImageSettingManager();
	~TmsImageSettingManager();

	std::vector<TmsImageSetting> settings() const;
	std::vector<TmsImageSetting> defaultSettings() const;
	void setSettings(const std::vector<TmsImageSetting>& settings);

	TmsImageSetting setupXYZSetting(const QString& name, const QString& url, int maxZoom);

	tmsloader::TmsRequest* buildRequest(const QPointF& centerLonLat, const QSize& size, double scale, const std::string& setting) const;
};

#endif // TMSIMAGESETTINGMANAGER_H
