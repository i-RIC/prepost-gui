#ifndef GEODATAPOINTMAPPROXY_H
#define GEODATAPOINTMAPPROXY_H

#include <guicore/pre/geodata/geodataproxy.h>

class GeoDataPointmap;

class GeoDataPointmapProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataPointmapProxy(GeoDataPointmap* geodata);
	~GeoDataPointmapProxy();

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

#endif // GEODATAPOINTMAPPROXY_H
