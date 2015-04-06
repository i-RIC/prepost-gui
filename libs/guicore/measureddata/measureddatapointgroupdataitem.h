#ifndef MEASUREDDATAPOINTGROUPDATAITEM_H
#define MEASUREDDATAPOINTGROUPDATAITEM_H

#include "../datamodel/graphicswindowdataitem.h"
#include <guibase/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <QMap>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkScalarBarWidget.h>

class MeasuredData;
class MeasuredDataPointDataItem;
class vtkLODActor;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;
class Post2dWindowContourSetProperty;
class MeasuredDataPointSetProperty;
class MeasuredDataPointSelectValue;
class LookupTableContainer;

class GUICOREDLL_EXPORT MeasuredDataPointGroupDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
private:
	static const int DEFAULT_NUMOFDIV = 15;
public:
	/// Constructor
	MeasuredDataPointGroupDataItem(GraphicsWindowDataItem* parent);
	~MeasuredDataPointGroupDataItem();
	const QString& currentMeasuredValue() const {return m_currentMeasuredValue;}
	void updateZDepthRangeItemCount();
	void assignActionZValues(const ZDepthRange& range);
	void update();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void setCurrentMeasuredValue(const QString& value);
	bool hasTransparentPart();
	LookupTableContainer* lookupTable(const QString& attName){return m_lookupTables.value(attName, 0);}
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void doApplyOffset(double x, double y);

public slots:
	void exclusivelyCheck(MeasuredDataPointDataItem* item);
	void showSettingDialog(){showPropertyDialog();}
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
		void setupPointSetting();
	void setupIsolineSetting();
	void setupColorContourSetting();
	void setupColorFringeSetting();
	void setupScalarBarSetting();

	int m_numberOfDivisions;
	QString m_currentMeasuredValue;
	ContourSettingWidget::Contour m_contour;
	bool m_fillUpper;
	bool m_fillLower;
	int m_pointSize;
	int m_opacityPercent;

	// for scalar bar
	QMap<QString, LookupTableContainer*> m_lookupTables;
	QMap<QString, QString> m_colorbarTitleMap;
	ScalarBarSetting m_scalarBarSetting;

	vtkLODActor* m_contourActor;
	vtkPolyDataMapper* m_contourMapper;
	vtkActor* m_isolineActor;
	vtkPolyDataMapper* m_isolineMapper;
	vtkContourFilter* m_isolineFilter;
	vtkLODActor* m_fringeActor;
	vtkPolyDataMapper* m_fringeMapper;
	vtkActor* m_pointsActor;
		vtkPolyDataMapper* m_pointsMapper;
		vtkActor* m_blackPointsActor;
		vtkPolyDataMapper* m_blackPointsMapper;
	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

	vtkSmartPointer<vtkPolyData> m_valueClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_colorContourPolyData;

public:
	friend class MeasuredDataPointSetProperty;
	friend class MeasuredDataPointSelectValue;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_H
