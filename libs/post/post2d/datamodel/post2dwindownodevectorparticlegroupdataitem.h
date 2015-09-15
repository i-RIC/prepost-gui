#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/vtksubdividegrid.h>
#include <guibase/structuredgridregion.h>
#include <guibase/vtkCustomStreamPoints.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>
#include <misc/doublecontainer.h>

#include <QMap>
#include <QColor>
#include <QList>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkStreamPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGrid.h>
#include <vtkClipPolyData.h>

class Post2dWindowNodeVectorParticleDataItem;

class Post2dWindowNodeVectorParticleGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

protected:
	const static int DEFAULT_SIZE = 3;

public:
	enum TimeMode {tmNormal, tmSubdivide, tmSkip};

	struct Setting : public CompositeContainer
	{
		/// Constructor
		Setting();
		/// Copy constructor
		Setting(const Setting& s);
		/// Copy operator
		Setting& operator=(const Setting& s);

		StringContainer currentSolution;
		EnumContainerT<TimeMode> timeMode;

		IntContainer timeSamplingRate;
		IntContainer timeDivision;
		IntContainer particleSize;
		EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	};

	/// Constructor
	Post2dWindowNodeVectorParticleGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorParticleGroupDataItem();
	void updateActorSettings();
	void setupClipper();
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void updateZDepthRangeItemCount() override;
	virtual void assignActorZValues(const ZDepthRange& range) override;
	void update();
	bool exportParticles(const QString& filePrefix, int fileIndex, double time);
	QString currentSolution() {return m_setting.currentSolution;}

public slots:
	void exclusivelyCheck(Post2dWindowNodeVectorParticleDataItem*);

protected:
	void informGridUpdate();
	vtkPointSet* getRegion();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void setCurrentSolution(const QString& currentSol);

	virtual vtkPointSet* newParticles(int i) = 0;
	virtual void setupActors() = 0;
	virtual void setupParticleSources() = 0;
	void setupStreamTracer();
	void resetParticles();
	void addParticles();

	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	QList<vtkPolyData*> m_particleGrids;
	QList<vtkActor*> m_particleActors;
	QList<vtkDataSetMapper*> m_particleMappers;

	vtkSmartPointer<vtkStreamPoints> m_streamTracer;
	vtkSmartPointer<vtkCustomStreamPoints> m_streamPoints;

	Setting m_setting;

	unsigned int m_previousStep;
	double m_previousTime;
	int m_nextStepToAddParticles;

	class SelectSolutionCommand;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
