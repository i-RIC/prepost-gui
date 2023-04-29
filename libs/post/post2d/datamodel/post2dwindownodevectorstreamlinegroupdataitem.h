#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guicore/misc/targeted/targeteditemi.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

#include <vector>

class NamedGraphicWindowDataItem;
class Post2dWindowNodeVectorStreamlineDataItem;

class vtkActor;
class vtkStreamTracer;

class Post2dWindowNodeVectorStreamlineGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);

		Setting& operator=(const Setting& s);

		StringContainer target;
	};

	Post2dWindowNodeVectorStreamlineGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineGroupDataItem() override;

	void updateActorSetting() override;
	void updateZDepthRangeItemCount() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	virtual void assignActorZValues(const ZDepthRange& range) override;
	void update();

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	std::string target() const override;
	void setTarget(const std::string& target) override;

	virtual void informGridUpdate();
	virtual void setupActors() = 0;
	void clearActors();

	static void setupStreamTracer(vtkStreamTracer* tracer);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;

	Setting m_setting;
	double m_zScale;

	std::vector<vtkActor*> m_streamlineActors;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
