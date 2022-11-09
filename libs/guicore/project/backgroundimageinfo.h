#ifndef BACKGROUNDIMAGEINFO_H
#define BACKGROUNDIMAGEINFO_H

#include "../guicore_global.h"
#include "projectdataitem.h"

#include <unordered_set>

class BackgroundImageInfoGeoreferenceDialog;
class GcpTableModel;
class GcpTableRow;
class VTKGraphicsView;

class vtkActor;
class vtkPolyDataMapper;

class QAction;
class QDialog;
class QMouseEvent;

class GUICOREDLL_EXPORT BackgroundImageInfo : public ProjectDataItem
{
	Q_OBJECT

public:
	const static int MAXWIDTH;

	BackgroundImageInfo(const QString& fileName, ProjectDataItem* d);
	~BackgroundImageInfo();

	void initializePosition(const QString& originalFileName);

	QString fileName() const;
	QString fullFileName() const;

	int originalImageWidth() const;
	int originalImageHeight() const;

	double angle() const;

	void mouseMoveEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(vtkActor* actor, QMouseEvent* event, VTKGraphicsView* v);

	QAction* fixAction() const;
	QAction* fixActionWithIcon() const;

	void setupActor(vtkActor* actor);
	void setupMapper(vtkPolyDataMapper* mapper);

	void applySettingToActor(vtkActor* actor);
	void setPreProcessorActor(vtkActor* actor);
	void deleteImageFile();

	QDialog* propertyDialog();
	void handlePropertyDialogAccepted(QDialog* dialog);

	void applyOffset(double x, double y);

	std::vector<GcpTableRow>* gcpTable();
	GcpTableModel* gcpTableModel();
	void handleGeoreferenceDialogAccepted(BackgroundImageInfoGeoreferenceDialog* dialog);

	void show();
	void toggleVisibility();

	bool isVisible();
	void showGeoreferenceDialog(vtkActor* actor, VTKGraphicsView* v, double minDepth, double maxDepth, QWidget* w);

public slots:
	void selectPoints(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices);
	void startGcpSelect();

private slots:
	void toggleFixState();
	void handleGeoreferenceDialogClosed();

signals:
	void isChanged();
	void gcpDefined(const QPointF&);
	void selectedIndexChanged(const std::unordered_set<std::vector<GcpTableRow>::size_type>&);
	void isGeoreferenceDialogClosed();
	void isVisibilityChanged();

private:
	void fitImageToData();
	void initializePosition(double resizeScale);
	void informChange();

	BackgroundImageInfoGeoreferenceDialog* georeferenceDialog(QWidget* w);
	void updateGeoReferencePointsActor(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indice);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	bool m_hide; // for georeference

	BackgroundImageInfoGeoreferenceDialog* m_georeferenceDialog;

	class SetActorPropertyCommand;
	class Setting;

	class Impl;
	Impl* impl;

public:
	friend class BackgroundImageInfoDialog;
	friend class BackgroundImageInfoGeoreferenceDialog;
};

#endif // BACKGROUNDIMAGEINFO_H
