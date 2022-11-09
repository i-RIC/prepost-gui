#ifndef BACKGROUNDIMAGEINFO_IMPL_H
#define BACKGROUNDIMAGEINFO_IMPL_H

#include "../backgroundimageinfo.h"
#include "backgroundimageinfo_setting.h"

#include <guibase/vtktool/vtklineactor.h>

#include <QCursor>
#include <QPixmap>

class QAction;

class vtkPlaneSource;
class vtkTexture;
class vtkTextureMapToPlane;

class BackgroundImageInfo::Impl
{
public:
	enum class MouseEventMode {
		Normal,
		Rotating,
		Zooming,
		Translating
	};

	Impl(BackgroundImageInfo* info);
	~Impl();

	void loadImageData(const QString& fileName);
	void updateFixActionIcon();
	void applyWorldFileSetting(const QString& fileName);
	void fitToRegion(double xmin, double xmax, double ymin, double ymax);

	static QString scaledImageFileName(const QString origName);

	// setting about positioning background image
	Setting m_setting;
	Setting m_settingBeforeMouseEventStarts;

	// image information
	int m_originalImageWidth;
	int m_originalImageHeight;
	double m_resizeScale;

	// vtk resources
	vtkTexture* m_texture;
	vtkPlaneSource* m_plane;
	vtkTextureMapToPlane* m_mapToPlane;

	vtkActor* m_preProcessorActor;

	// georeferencing related resources
	bool m_isGeoReferencing;
	bool m_isGeoReferenceSelectingPoint;

	vtkActor* m_geoReferenceActor;
	vtkLineActor m_geoReferencePointsActor;
	vtkLineActor m_geoReferenceSelectedPointsActor;
	VTKGraphicsView* m_geoReferenceGraphicsView;
	QWidget* m_geoReferenceParentWindow;

	// menu actions
	QAction* m_fixAction;
	QAction* m_fixActionWithIcon;

	// mouse cursor related resources
	QPixmap m_movePixmap;
	QPixmap m_rotatePixmap;
	QPixmap m_zoomPixmap;
	QCursor m_moveCursor;
	QCursor m_rotateCursor;
	QCursor m_zoomCursor;

	MouseEventMode m_mouseEventMode;

	QPoint m_previousMousePosition;

private:
	void setupGeoReferenceActors();

	BackgroundImageInfo* m_parent;
};

#endif // BACKGROUNDIMAGEINFO_IMPL_H
