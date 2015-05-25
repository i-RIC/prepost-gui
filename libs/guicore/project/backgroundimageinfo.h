#ifndef BACKGROUNDIMAGEINFO_H
#define BACKGROUNDIMAGEINFO_H

#include "../guicore_global.h"
#include "projectdataitem.h"

#include <vtkSmartPointer.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>

#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>

#include <QPixmap>
#include <QCursor>

class QMouseEvent;
class QAction;
class VTKGraphicsWindowDataItem;
class VTKGraphicsView;

class GUICOREDLL_EXPORT BackgroundImageInfo : public ProjectDataItem
{
	Q_OBJECT
public:
	const static int MAXWIDTH;
	/// Constructor
	BackgroundImageInfo(const QString& filename, const QString& origFilename, ProjectDataItem* d);
	~BackgroundImageInfo();

	const QString& name() const {return m_name;}
	void setName(const QString& name) {
		m_name = name;
	}
	const QString& caption() const {return m_caption;}
	void setCaption(const QString& cap) {m_caption = cap;}

	const QString& fileName() const {return m_filename;}

	void mouseMoveEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);
	int imageWidth() const {return m_imageWidth;}
	int imageHeight() const {return m_imageHeight;}

	vtkActor* refActor();
	QAction* fixAction() const {return m_fixAction;}
	QAction* fixActionWithIcon() const {return m_fixActionWithIcon;}
	double aspectRatio() const {return m_aspectRatio;}
	void setupActor(vtkActor* actor);
	bool visible() const {return m_visible;}
	void setVisible(bool visible) {m_visible = visible;}
	void applySettingToActor(vtkActor* actor);

	double translateX() const {return m_translateX;}
	double translateY() const {return m_translateY;}
	double scale() const {return m_scale;}
	double angle() const {return m_angle;}
	void setPreProcessorActor(vtkActor* actor) {
		m_preProcessorActor = actor;
	}
	void setTranslateX(double x) {m_translateX = x;}
	void setTranslateY(double y) {m_translateY = y;}
	void setScale(double scale) {m_scale = scale;}
	QDialog* propertyDialog();
	void handlePropertyDialogAccepted(QDialog* dialog);
	void applyOffset(double x, double y);

protected slots:
	void editName() {}
	void toggleFixState();

signals:
	void isChanged();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void updateFixActionIcon();
	void readImageData(const QString& filename, const QString& origFilename);
	void readWorldFile(const QString& name);
	void fitImageToData();
	void initializePosition();
	void informChange();
	QString static getThumbnailFileName(const QString& origname);

	QAction* m_editNameAction;
	QAction* m_fixAction;
	QAction* m_fixActionWithIcon;
	QString m_name;   // full path
	QString m_filename;  // basename
	QString m_caption;
	vtkSmartPointer<vtkTexture> m_texture;
	vtkSmartPointer<vtkPlaneSource> m_plane;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;
	vtkSmartPointer<vtkActor> m_refActor;
	vtkSmartPointer<vtkActor> m_preProcessorActor;

	bool m_isRotating;
	bool m_isZooming;
	bool m_isTranslating;

	double lastX;
	double lastY;

	// for world file
	QString m_worldFilename;
	bool m_hasWorldFile;
	double m_scale;
	double m_translateX;
	double m_translateY;
	double m_angle;
	bool m_visible;

	// image size
	int m_imageWidth;
	int m_imageHeight;
	double m_resizeScale;

	// actor property
	double m_oldTheta;
	double m_oldTranslateX;
	double m_oldTranslateY;
	double m_oldScale;

	bool m_isMoving;
	double m_aspectRatio;   // height / width

	bool m_fixed;

	QPixmap m_movePixmap;
	QPixmap m_rotatePixmap;
	QPixmap m_zoomPixmap;
	QCursor m_moveCursor;
	QCursor m_rotateCursor;
	QCursor m_zoomCursor;
public:
	friend class BackgroundImageInfoActorPropertySetting;
	friend class BackgroundImageInfoDialog;
};

#endif // BACKGROUNDIMAGEINFO_H
