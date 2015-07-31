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
	void handleStandardItemDoubleClicked() override;
	GridShapeEditDialog::Shape shape() const {return m_shape;}
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(2);}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void innerUpdateZScale(double scale) override;

private:
	void setupActors();
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

	vtkSmartPointer<vtkPoints> m_pointsBeforeDragging;

	GridShapeEditDialog::Setting m_setting;

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
