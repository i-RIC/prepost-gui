#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guibase/widget/gridshapeeditdialog.h>

#include <QVector2D>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkLabeledDataMapper.h>
#include <vtkMapper.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkPoints.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>

class QDialog;
class QAction;
class QColor;
class QString;
class Post2dWindowGridShapeDataSetSetting;

class Post2dWindowGridShapeDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

private:
	static const int normalOutlineWidth = 1;
	static const int selectedOutlineWidth = 2;

public:
	Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowGridShapeDataItem();
	/// Inform that the grid is updated.
	void update();
	void handleStandardItemDoubleClicked() override;
	GridShapeEditDialog::Shape shape() const {return m_setting.shape;}
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(2);}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void assignActorZValues(const ZDepthRange& range) override;

private:
	void setupActors();
	void updateActorSettings();
	void loadShapeFromProjectMainFile(const QDomNode& shapeNode);
	void saveShapeToProjectMainFile(QXmlStreamWriter& writer);
	void updateAxesRegion();
	void innerUpdateZScale(double scale) override;

	vtkSmartPointer<vtkActor> m_outlineActor;
	vtkSmartPointer<vtkActor> m_wireframeActor;
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

public:
	friend class Post2dWindowGridShapeDataSetSetting;

private:
	class SetSettingCommand;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_H
