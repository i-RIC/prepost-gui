#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/vtksubdividegrid.h>
#include <guibase/structuredgridregion.h>
#include <guibase/vtkCustomStreamPoints.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>
#include <misc/doublecontainer.h>

#include <QMap>
#include <QColor>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkStreamPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGrid.h>
#include <vtkClipPolyData.h>

#include <vector>

class NamedGraphicWindowDataItem;
class Post2dWindowNodeVectorParticleDataItem;

class Post2dWindowNodeVectorParticleGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
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

		StringContainer target;
		EnumContainerT<TimeMode> timeMode;

		IntContainer timeSamplingRate;
		IntContainer timeDivision;
		IntContainer particleSize;
		EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	};

	Post2dWindowNodeVectorParticleGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorParticleGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

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

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void informGridUpdate();
	vtkPointSet* getRegion();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	virtual vtkPointSet* newParticles(int i) = 0;
	virtual void setupActors() = 0;
	virtual void setupParticleSources() = 0;
	void setupStreamTracer();
	void resetParticles();
	void addParticles();

private:
	void innerUpdateZScale(double scale) override;

protected:
	vtkSmartPointer<vtkClipPolyData> m_IBCClipper;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	std::vector<vtkPolyData*> m_particleGrids;
	std::vector<vtkActor*> m_particleActors;
	std::vector<vtkDataSetMapper*> m_particleMappers;

	vtkSmartPointer<vtkStreamPoints> m_streamTracer;
	vtkSmartPointer<vtkCustomStreamPoints> m_streamPoints;

	Setting m_setting;

	unsigned int m_previousStep;
	double m_previousTime;
	int m_nextStepToAddParticles;
	double m_zScale;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPDATAITEM_H
