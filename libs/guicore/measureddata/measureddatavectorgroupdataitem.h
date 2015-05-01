#ifndef MEASUREDDATAVECTORGROUPDATAITEM_H
#define MEASUREDDATAVECTORGROUPDATAITEM_H

#include "../datamodel/graphicswindowdataitem.h"
#include "../project/measureddata.h"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkGlyph3D.h>
#include <vtkHedgeHog.h>
#include <vtkConeSource.h>
#include <vtkWarpVector.h>
#include <vtkAppendPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkActor2D.h>
#include <vtkTextActor.h>
#include <vtkClipPolyData.h>

#include <QColor>

class vtkPointSet;
class MeasuredDataVectorDataItem;
class MeasuredDataVectorSelectValue;
class MeasuredDataVectorSetProperty;

class MeasuredDataVectorGroupDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	const static int STANDARD_LENGTH = 100;
	const static int AUTO_AVERAGECOUNT = 20;
	const static double MINLIMIT;
	/// Constructor
	MeasuredDataVectorGroupDataItem(GraphicsWindowDataItem* parent);
	~MeasuredDataVectorGroupDataItem();
	void updateActorSettings();
	void updateZDepthRangeItemCount();
	void assignActionZValues(const ZDepthRange& range);
	void update();
public slots:
	void exclusivelyCheck(MeasuredDataVectorDataItem*);
	void showSettingDialog() {showPropertyDialog();}
protected:
	QDialog* propertyDialog(QWidget* p);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	vtkPointSet* getPointSet();
	void innerUpdate2Ds();
	void updateColorSetting();
	void updatePolyData();
	void updateLegendData();
	void informGridUpdate();
	void setCurrentSolution(const QString& currentSol);
	const QString& currentSolution() {return m_currentSolution;}
	void doApplyOffset(double x, double y);
private:
	void setupActors();
	void calculateStandardValue();
	void updateScaleFactor();

protected:
	vtkSmartPointer<vtkActor> m_arrowActor;
	vtkSmartPointer<vtkPolyDataMapper> m_arrowMapper;
	vtkSmartPointer<vtkAppendPolyData> m_appendPolyData;
	vtkSmartPointer<vtkPolyData> m_polyData;
	vtkSmartPointer<vtkUnstructuredGrid> m_activePoints;

	vtkSmartPointer<vtkHedgeHog> m_hedgeHog;
	vtkSmartPointer<vtkGlyph3D> m_arrowGlyph;
	vtkSmartPointer<vtkWarpVector> m_warpVector;
	vtkSmartPointer<vtkConeSource> m_arrowSource;

	vtkSmartPointer<vtkTextActor> m_legendTextActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_baseArrowPolyData;
	vtkSmartPointer<vtkActor2D> m_baseArrowActor;

	QString m_scalarValueName;
	QString m_currentSolution;
	QColor m_color;
	double m_oldCameraScale;
	double m_scaleFactor;

	MeasuredData::ArrowColorMode m_colorMode;
	MeasuredData::ArrowLengthMode m_lengthMode;
	double m_standardValue;
	double m_legendLength;
	double m_minimumValue;
public:
	friend class MeasuredDataVectorSelectValue;
	friend class MeasuredDataVectorSetProperty;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_H
