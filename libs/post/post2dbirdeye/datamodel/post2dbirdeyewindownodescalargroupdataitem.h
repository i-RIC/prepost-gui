#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <guibase/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <guibase/structuredgridregion.h>
#include <QMap>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkWarpScalar.h>
#include <vtkLODActor.h>
#include <vtkScalarBarWidget.h>

class Post2dBirdEyeWindowNodeScalarDataItem;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;
class Post2dBirdEyeWindowContourSetProperty;
class Post2dBirdEyeWindowContourSelectSolution;

class Post2dBirdEyeWindowNodeScalarGroupDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT
private:
	static const int DEFAULT_NUMOFDIV = 15;
public:
	/// Constructor
	Post2dBirdEyeWindowNodeScalarGroupDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowNodeScalarGroupDataItem();
	const QString& currentCondition() const {return m_currentSolution;}
	void update();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void setCurrentSolution(const QString& currentSol);
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
public slots:
	void exclusivelyCheck(Post2dBirdEyeWindowNodeScalarDataItem* item);
protected:
	void updateVisibility(bool visible);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void innerUpdateZScale(double scale);
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
	ScalarBarSetting m_scalarBarSetting;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;

	vtkSmartPointer<vtkWarpScalar> m_warp;
	vtkLODActor* m_contourActor;
	vtkDataSetMapper* m_contourMapper;
	vtkActor* m_isolineActor;
	vtkPolyDataMapper* m_isolineMapper;
	vtkContourFilter* m_isolineFilter;
	vtkLODActor* m_fringeActor;
	vtkDataSetMapper* m_fringeMapper;
	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_valueClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_colorContourPolyData;

public:
	friend class Post2dBirdEyeWindowContourSetProperty;
	friend class Post2dBirdEyeWindowContourSelectSolution;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
