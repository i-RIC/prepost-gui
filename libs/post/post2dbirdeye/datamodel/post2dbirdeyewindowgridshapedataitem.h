#ifndef POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_H
#define POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <guibase/gridshapeeditdialog.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkActor2D.h>
#include <vtkMapper.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkPoints.h>
#include <QVector2D>
#include <vtkLabeledDataMapper.h>
//#include <vtkCubeAxesActor2D.h>
#include <vtkWarpScalar.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

class QDialog;
class QAction;
class QColor;
class QString;
class Post2dBirdEyeWindowGridShapeDataSetSetting;

class Post2dBirdEyeWindowGridShapeDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT
private:
	static const int normalOutlineWidth = 1;
	static const int selectedOutlineWidth = 2;
public:
	/// Constructor
	Post2dBirdEyeWindowGridShapeDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowGridShapeDataItem();
	/// Inform that the grid is updated.
	void update();
	void handleStandardItemDoubleClicked();
	QColor color();
	QColor indexColor();
	GridShapeEditDialog::Shape shape() {return m_shape;}
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void updateZDepthRangeItemCount() {m_zDepthRange.setItemCount(2);}
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void innerUpdateZScale(double scale);
private:
	void setupActors();
	void setColor(const QColor& color);
	void setIndexColor(const QColor& color);
	void updateActorSettings();
	vtkSmartPointer<vtkWarpScalar> m_warp;
	vtkSmartPointer<vtkWarpScalar> m_labelWarp;
	vtkSmartPointer<vtkActor> m_outlineActor;
	vtkSmartPointer<vtkLODActor> m_wireframeActor;
	vtkSmartPointer<vtkMapper> m_outlineMapper;
	vtkSmartPointer<vtkDataSetMapper> m_wireframeMapper;
	vtkSmartPointer<vtkStructuredGridOutlineFilter> m_outlineFilter;
	vtkSmartPointer<vtkTransformFilter> m_indexTransformFilter;
	vtkSmartPointer<vtkTransform> m_indexTransform;
	vtkSmartPointer<vtkActor2D> m_indexActor;
	vtkSmartPointer<vtkLabeledDataMapper> m_indexMapper;
//	vtkSmartPointer<vtkCubeAxesActor2D> m_axesActor;

	vtkSmartPointer<vtkPoints> m_pointsBeforeDragging;

	double m_color[3];
	bool m_indexVisible;
	double m_indexColor[3];
//	bool m_axesVisible;
//	double m_axesColor[3];

	bool m_definingBoundingBox;
	bool m_draggingSelectedPoints;
	bool m_nearSelectedPoint;

	QAction* m_editNewPositionAction;
	QAction* m_editDeltaAction;
	QVector2D m_dragStartPoint;
	GridShapeEditDialog::Shape m_shape;
public:
	friend class Post2dBirdEyeWindowGridShapeDataSetSetting;
};

#endif // POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_H
