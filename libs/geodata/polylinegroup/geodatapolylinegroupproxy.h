#ifndef GEODATAPOLYLINEGROUPPROXY_H
#define GEODATAPOLYLINEGROUPPROXY_H

#include "geodatapolylinegroupcolorsettingdialog.h"

#include <guicore/pre/geodata/geodataproxy.h>

class GeoDataPolyLineGroup;

class GeoDataPolyLineGroupProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataPolyLineGroupProxy(GeoDataPolyLineGroup* geodata);
	~GeoDataPolyLineGroupProxy();

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

#endif // GEODATAPOLYLINEGROUPPROXY_H
