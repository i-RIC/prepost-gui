#ifndef RAWDATAPOLYGONPROXY_H
#define RAWDATAPOLYGONPROXY_H

#include "rawdatapolygon.h"
#include <guicore/pre/rawdata/rawdataproxy.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>

class RawDataPolygonProxy : public RawDataProxy
{
	Q_OBJECT
public:
	RawDataPolygonProxy(RawDataPolygon* rawdata) : RawDataProxy(rawdata) {}
	void setupActors() override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
//	virtual void setupMenu(){}
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

#endif // RAWDATAPOLYGONPROXY_H
