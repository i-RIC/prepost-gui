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
	void setupActors();
	void updateZDepthRangeItemCount(ZDepthRange& range);
//	virtual void setupMenu(){}
public slots:
	void updateGraphics();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void assignActorZValues(const ZDepthRange& range) override;
private:
	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkActor> m_linesActor;
};

#endif // RAWDATAPOLYGONPROXY_H
