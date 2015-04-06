#ifndef POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/structuredgridregion.h>
#include <misc/arrowsettingcontainer.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkGlyph3D.h>
#include <vtkHedgeHog.h>
#include <vtkMaskPoints.h>
#include <vtkArrowSource.h>
#include <vtkConeSource.h>
#include <vtkWarpVector.h>
#include <vtkExtractGrid.h>
#include <vtkAppendPolyData.h>
#include <vtkActor2D.h>
#include <vtkTextActor.h>
#include <vtkClipPolyData.h>

#include <QColor>

class vtkPointSet;
class Post2dWindowNodeVectorArrowDataItem;

class Post2dWindowNodeVectorArrowGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	const static int STANDARD_LENGTH = 100;
	const static int AUTO_AVERAGECOUNT = 20;
	const static double MINLIMIT;
	enum Mapping{Specific, Scalar};
	enum LegendMode{lmAuto, lmCustom};
	enum LengthMode{lenAuto, lenCustom};
	/// Constructor
	Post2dWindowNodeVectorArrowGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorArrowGroupDataItem();
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void updateActorSettings();
	void updateZDepthRangeItemCount();
	void assignActionZValues(const ZDepthRange& range);
	void addCustomMenuItems(QMenu* menu);
	void update();

public slots:
	void exclusivelyCheck(Post2dWindowNodeVectorArrowDataItem*);

protected:
	void innerUpdate2Ds();
	void updateColorSetting();
	void updatePolyData();
	void updateLegendData();
	void informGridUpdate();
	virtual void updateActivePoints() = 0;
//	virtual vtkPointSet* getPointSet() = 0;
	void setCurrentSolution(const QString &currentSol);
	const QString& currentSolution(){return m_currentSolution;}
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

private:
	void setupActors();
	void calculateStandardValue();
	void updateScaleFactor();

protected:
	vtkSmartPointer<vtkActor> m_arrowActor;
	vtkSmartPointer<vtkPolyDataMapper> m_arrowMapper;
	vtkSmartPointer<vtkAppendPolyData> m_appendPolyData;
	vtkSmartPointer<vtkPolyData> m_polyData;

	vtkSmartPointer<vtkHedgeHog> m_hedgeHog;
	vtkSmartPointer<vtkGlyph3D> m_arrowGlyph;
	vtkSmartPointer<vtkWarpVector> m_warpVector;
	vtkSmartPointer<vtkConeSource> m_arrowSource;

	vtkSmartPointer<vtkTextActor> m_legendTextActor;

	vtkSmartPointer<vtkUnstructuredGrid> m_baseArrowPolyData;
	vtkSmartPointer<vtkActor2D> m_baseArrowActor;
	vtkSmartPointer<vtkPolyData> m_activePoints;

	QString m_scalarValueName;
	QString m_currentSolution;
	QColor m_color;
	double m_oldCameraScale;
	double m_scaleFactor;
	StructuredGridRegion::RegionMode m_regionMode;
	Mapping m_mapping;
	LegendMode m_legendMode;
	LengthMode m_lengthMode;
	double m_standardValue;
	double m_legendLength;
	double m_minimumValue;
	ArrowSettingContainer m_arrowSetting;

public:
	friend class Post2dWindowGridArrowSelectSolution;
	friend class Post2dWindowArrowStructuredSetProperty;
	friend class Post2dWindowArrowUnstructuredSetProperty;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_H
