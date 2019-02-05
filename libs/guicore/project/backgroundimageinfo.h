#ifndef BACKGROUNDIMAGEINFO_H
#define BACKGROUNDIMAGEINFO_H

#include "../guicore_global.h"
#include "projectdataitem.h"

#include <guibase/vtktool/vtklineactor.h>

#include <vtkSmartPointer.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>

#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>

#include <QDialog>
#include <QPixmap>
#include <QCursor>

#include <unordered_set>

class QMouseEvent;
class QAction;
class VTKGraphicsWindowDataItem;
class VTKGraphicsView;
class GcpTableModel;
class GcpTableRow;

class GUICOREDLL_EXPORT BackgroundImageInfo : public ProjectDataItem
{
	Q_OBJECT

public:
	const static int MAXWIDTH;
	/// Constructor
	BackgroundImageInfo(const QString& filename, const QString& origFilename, ProjectDataItem* d);
	~BackgroundImageInfo();

	const QString& name() const;
	void setName(const QString& name);

	const QString& caption() const;
	void setCaption(const QString& cap);

	const QString& fileName() const;

	void mouseMoveEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);
	int imageWidth() const {return m_imageWidth;}
	int imageHeight() const {return m_imageHeight;}

	vtkActor* refActor();
	QAction* fixAction() const;
	QAction* fixActionWithIcon() const;
	double aspectRatio() const;
	void setupActor(vtkActor* actor);
	bool visible() const;
	void setVisible(bool visible);
	void applySettingToActor(vtkActor* actor);

	double translateX() const;
	double translateY() const;
	double scale() const;
	double resizeScale() const;
	double angle() const;
	void setPreProcessorActor(vtkActor* actor);
	void setTranslateX(double x);
	void setTranslateY(double y);
	void setScale(double scale);
	QDialog* propertyDialog();
	void handlePropertyDialogAccepted(QDialog* dialog);
	void applyOffset(double x, double y);

	std::vector<GcpTableRow>* BackgroundImageInfo::gcpTable();
	GcpTableModel* gcpTableModel();
	void handleGeoreferenceDialogAccepted(QDialog* dialog);

	void hide();
	void show();
	void toggleVisibility();

	bool isVisible();
	void showGeoreferenceDialog(vtkActor* actor, VTKGraphicsView* v, double minDepth, double maxDepth, QWidget* w);

public slots:
	void selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices);
	void startGcpSelect();

protected slots:
	void editName() {}
	void toggleFixState();

private slots:
	void handleGeoreferenceDialogClosed();

signals:
	void isChanged();
	void gcpDefined(const QPointF&);
	// @todo Emit selectedIndexChanged(const std::unordered_set<std::vector<GcpTableRow>::size_type>&) after GCP points are selected.
	void selectedIndexChanged(const std::unordered_set<std::vector<GcpTableRow>::size_type>&);
	void isGeoreferenceDialogClosed();
	void isVisibilityChanged();

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
	QDialog* georeferenceDialog(QWidget* w);
	void updateGeoReferencePointsActor(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indice);

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

	bool m_isGeoReferencing;
	bool m_isGeoReferenceSelectingPoint;

	vtkActor* m_geoReferenceActor;
	vtkLineActor m_geoReferencePointsActor;
	vtkLineActor m_geoReferenceSelectedPointsActor;
	VTKGraphicsView* m_geoReferenceGraphicsView;
	QWidget* m_geoReferenceParentWindow;

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

	double m_aspectRatio;   // height / width

	bool m_fixed;

	bool m_hide; // for georeference

	QPixmap m_movePixmap;
	QPixmap m_rotatePixmap;
	QPixmap m_zoomPixmap;
	QCursor m_moveCursor;
	QCursor m_rotateCursor;
	QCursor m_zoomCursor;

	QDialog* m_georeferenceDialog;

	class SetActorPropertyCommand;

public:
	friend class BackgroundImageInfoDialog;
	friend class BackgroundImageInfoGeoreferenceDialog;
};

#endif // BACKGROUNDIMAGEINFO_H
