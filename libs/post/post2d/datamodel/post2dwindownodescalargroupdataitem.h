#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <guibase/structuredgridregion.h>
#include <QMap>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkLODActor.h>
#include <vtkScalarBarWidget.h>

class Post2dWindowNodeScalarDataItem;
class vtkLODActor;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;
class Post2dWindowContourSetProperty;
class Post2dWindowContourSelectSolution;

class Post2dWindowNodeScalarGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
private:
	static const int DEFAULT_NUMOFDIV = 15;
public:
	/// Constructor
	Post2dWindowNodeScalarGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeScalarGroupDataItem();
	const QString& currentSolution() const {return m_currentSolution;}
	void updateZDepthRangeItemCount();
	void assignActionZValues(const ZDepthRange& range);
	void update();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void setCurrentSolution(const QString& currentSol);
	bool hasTransparentPart();
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);
	bool exportKMLHeader(QXmlStreamWriter& writer);
	bool exportKMLFooter(QXmlStreamWriter& writer);
	bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time);

public slots:
	void exclusivelyCheck(Post2dWindowNodeScalarDataItem* item);
protected:
	void updateVisibility(bool visible);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
private:
	void setupActors();
	void updateActorSettings();
	void createRangeClippedPolyData();
	void createValueClippedPolyData();
	void setDefaultValues();
	void setupIsolineSetting();
	void setupColorContourSetting();
	void setupColorFringeSetting();
	void setupScalarBarSetting();

	int m_numberOfDivisions;
	QString m_currentSolution;
	ContourSettingWidget::Contour m_contour;
	bool m_fillUpper;
	bool m_fillLower;
	int m_opacityPercent;

	// Region setting
	StructuredGridRegion::RegionMode m_regionMode;
	StructuredGridRegion::Range2d m_range;

	// for scalar bar
	QMap<QString, QString> m_colorbarTitleMap;
	ScalarBarSetting m_scalarbarSetting;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;

	vtkSmartPointer<vtkLODActor> m_contourActor;
	vtkDataSetMapper* m_contourMapper;
	vtkActor* m_isolineActor;
	vtkPolyDataMapper* m_isolineMapper;
	vtkContourFilter* m_isolineFilter;
	vtkSmartPointer<vtkLODActor> m_fringeActor;
	vtkDataSetMapper* m_fringeMapper;
	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_valueClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_colorContourPolyData;

public:
	friend class Post2dWindowContourSetProperty;
	friend class Post2dWindowContourSelectSolution;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_H
