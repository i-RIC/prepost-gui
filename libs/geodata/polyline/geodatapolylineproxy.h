#ifndef GEODATAPOLYLINEPROXY_H
#define GEODATAPOLYLINEPROXY_H

#include "geodatapolyline.h"

#include <guicore/pre/geodata/geodataproxy.h>

class GeoDataPolyLineProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataPolyLineProxy(GeoDataPolyLine* geodata);
	~GeoDataPolyLineProxy();

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

#endif // GEODATAPOLYLINEPROXY_H
