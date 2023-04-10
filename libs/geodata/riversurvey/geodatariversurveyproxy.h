#ifndef GEODATARIVERSURVEYPROXY_H
#define GEODATARIVERSURVEYPROXY_H

#include "geodatariversurveydisplaysetting.h"

#include <guicore/pre/geodata/geodataproxy.h>

class GeoDataRiverSurvey;

class GeoDataRiverSurveyProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataRiverSurveyProxy(GeoDataRiverSurvey* geodata);
	~GeoDataRiverSurveyProxy();

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

#endif // GEODATARIVERSURVEYPROXY_H
