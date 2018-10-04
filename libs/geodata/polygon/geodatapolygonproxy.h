#ifndef GEODATAPOLYGONPROXY_H
#define GEODATAPOLYGONPROXY_H

#include "geodatapolygon.h"

#include <guicore/pre/geodata/geodataproxy.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>

class GeoDataPolygonProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataPolygonProxy(GeoDataPolygon* geodata) : GeoDataProxy(geodata) {}
	void setupActors() override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;

public slots:
	void updateGraphics() override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;

private:
	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkActor> m_linesActor;
};

#endif // GEODATAPOLYGONPROXY_H
