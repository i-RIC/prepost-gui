#ifndef GEODATAPOLYGONGROUPPROXY_H
#define GEODATAPOLYGONGROUPPROXY_H

#include "geodatapolygongroupcolorsettingdialog.h"

#include <guicore/pre/geodata/geodataproxy.h>

class GeoDataPolygonGroup;

class GeoDataPolygonGroupProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataPolygonGroupProxy(GeoDataPolygonGroup* geodata);
	~GeoDataPolygonGroupProxy();

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

#endif // GEODATAPOLYGONGROUPPROXY_H
