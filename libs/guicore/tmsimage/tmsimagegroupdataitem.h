#ifndef TMSIMAGEGROUPDATAITEM_H
#define TMSIMAGEGROUPDATAITEM_H

#include "../guicore_global.h"
#include "../datamodel/graphicswindowdataitem.h"
#include "../misc/targeted/targeteditemi.h"

#include <tmsloader/tmsloader.h>

#include <vtkActor.h>
#include <vtkPlaneSource.h>
#include <vtkQImageToImageSource.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

#include <QImage>
#include <QPointF>

class NamedGraphicWindowDataItem;

class GUICOREDLL_EXPORT TmsImageGroupDataItem : public GraphicsWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	TmsImageGroupDataItem(GraphicsWindowDataItem* parent);
	~TmsImageGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string &target) override;

	void rebuildChildItems();

	void viewOperationEndedGlobal(VTKGraphicsView* v) override;

	void updateZDepthRangeItemCount() override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private slots:
	void handleImageUpdate(int requestId);

private:
	void requestImage();

	void assignActorZValues(const ZDepthRange& range) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

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
};

#endif // TMSIMAGEGROUPDATAITEM_H
