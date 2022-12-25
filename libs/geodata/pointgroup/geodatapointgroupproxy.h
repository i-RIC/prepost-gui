#ifndef GEODATAPOINTGROUPPROXY_H
#define GEODATAPOINTGROUPPROXY_H

#include "geodatapointgroupcolorsettingdialog.h"

#include <guicore/pre/geodata/geodataproxy.h>

class GeoDataPointGroup;

class GeoDataPointGroupProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataPointGroupProxy(GeoDataPointGroup* geodata);
	~GeoDataPointGroupProxy();

	void setupActors() override;

	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

public slots:
	void updateGraphics() override;

private:
	void assignActorZValues(const ZDepthRange& range) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class SetSettingCommand;

	class Impl;
	Impl* impl;
};

#endif // GEODATAPOINTGROUPPROXY_H
