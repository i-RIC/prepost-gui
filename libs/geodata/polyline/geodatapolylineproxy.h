#ifndef GEODATAPOLYLINEPROXY_H
#define GEODATAPOLYLINEPROXY_H

#include "geodatapolyline.h"

#include <guicore/pre/geodata/geodataproxy.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>

class GeoDataPolyLineProxy : public GeoDataProxy
{
	Q_OBJECT

public:
	GeoDataPolyLineProxy(GeoDataPolyLine* geodata) : GeoDataProxy(geodata) {}
	void setupActors() override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;

public slots:
	void updateGraphics() override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;

private:
	vtkSmartPointer<vtkActor> m_linesActor;
};

#endif // GEODATAPOLYLINEPROXY_H
