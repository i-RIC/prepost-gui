#ifndef GEODATAPOLYGONPROXY_H
#define GEODATAPOLYGONPROXY_H

#include "geodatapolygon.h"

#include <guicore/pre/geodata/geodataproxy.h>

class GeoDataPolygonProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataPolygonProxy(GeoDataPolygon* geodata);
	~GeoDataPolygonProxy();

	void setupActors() override;

	void updateZDepthRangeItemCount(ZDepthRange& range) override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

public slots:
	void updateActorSetting() override;

private:
	void assignActorZValues(const ZDepthRange& range) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class DisplaySetting;
	class DisplaySettingWidget;

	class Impl;
	Impl* impl;
};

#endif // GEODATAPOLYGONPROXY_H
