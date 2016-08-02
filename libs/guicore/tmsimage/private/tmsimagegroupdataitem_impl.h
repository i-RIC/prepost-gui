#ifndef TMSIMAGEGROUPDATAITEM_IMPL_H
#define TMSIMAGEGROUPDATAITEM_IMPL_H

#include "../tmsimagegroupdataitem.h"

#include <tmsloader/tmsloader.h>

#include <vtkActor.h>
#include <vtkPlaneSource.h>
#include <vtkQImageToImageSource.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

#include <QImage>
#include <QPointF>

class TmsImageGroupDataItem::Impl
{
public:
	Impl(TmsImageGroupDataItem* parent);

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPlaneSource> m_plane;
	vtkSmartPointer<vtkTexture> m_texture;
	vtkSmartPointer<vtkQImageToImageSource> m_imgToImg;

	std::string m_target;
	tmsloader::TmsLoader m_tmsLoader;
	QImage m_image;
	int m_tmsRequestId;

	QPointF m_imageLowerLeft;
	double m_imageScale;

	TmsImageGroupDataItem* m_parent;
};

#endif // TMSIMAGEGROUPDATAITEM_IMPL_H
