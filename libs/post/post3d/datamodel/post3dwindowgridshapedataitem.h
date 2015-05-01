#ifndef POST3DWINDOWGRIDSHAPEDATAITEM_H
#define POST3DWINDOWGRIDSHAPEDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/gridshapeeditdialog.h>

#include <vtkSmartPointer.h>
#include <vtkLODActor.h>
#include <vtkActor2D.h>
#include <vtkDataSetMapper.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkLabeledDataMapper.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
//#include <vtkCubeAxesActor2D.h>

#include <QVector2D>

class Post3dWindowGridShapeDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
private:
	static const int normalOutlineWidth = 1;
	static const int selectedOutlineWidth = 2;
public:
	/// Constructor
	Post3dWindowGridShapeDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowGridShapeDataItem();
	void handleStandardItemDoubleClicked();
	QColor color();
	QColor indexColor();
	GridShapeEditDialog::Shape shape() {return m_shape;}
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void update();
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
	void loadShapeFromProjectMainFile(const QDomNode& shapeNode);
	void saveShapeToProjectMainFile(QXmlStreamWriter& writer);
	vtkSmartPointer<vtkActor> m_outlineActor;
	vtkSmartPointer<vtkLODActor> m_wireframeActor;
	vtkSmartPointer<vtkMapper> m_outlineMapper;
	vtkSmartPointer<vtkDataSetMapper> m_wireframeMapper;
	vtkSmartPointer<vtkStructuredGridOutlineFilter> m_outlineFilter;
	vtkSmartPointer<vtkPoints> m_pointsBeforeDragging;
	vtkSmartPointer<vtkTransformFilter> m_indexTransformFilter;
	vtkSmartPointer<vtkTransform> m_indexTransform;
	vtkSmartPointer<vtkActor2D> m_indexActor;
	vtkSmartPointer<vtkLabeledDataMapper> m_indexMapper;
//	vtkSmartPointer<vtkCubeAxesActor2D> m_axesActor;

	double m_color[3];
	bool m_indexVisible;
	double m_indexColor[3];
//	bool m_axesVisible;
//	double m_axesColor[3];

	bool m_definingBoundingBox;
	bool m_draggingSelectedPoints;
	bool m_nearSelectedPoint;

	GridShapeEditDialog::Shape m_shape;
public:
	friend class Post3dWindowGridShapeDataSetSetting;
};

#endif // POST3DWINDOWGRIDSHAPEDATAITEM_H
