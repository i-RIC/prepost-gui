#ifndef GEODATANETCDFPROXY_H
#define GEODATANETCDFPROXY_H

#include <guicore/pre/geodata/geodataproxy.h>

class GeoDataNetcdf;

class GeoDataNetcdfProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataNetcdfProxy(GeoDataNetcdf* geodata);
	~GeoDataNetcdfProxy();

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

#endif // GEODATANETCDFPROXY_H
