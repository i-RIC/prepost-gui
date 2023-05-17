#ifndef POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
#define POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guicore/misc/targeted/targeteditemi.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

#include <vector>

class NamedGraphicWindowDataItem;
class Post3dWindowNodeVectorStreamlineDataItem;
class Post3dWindowStreamlineStructuredSetProperty;

class vtkActor;
class vtkStreamTracer;

class Post3dWindowNodeVectorStreamlineGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
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

	Post3dWindowNodeVectorStreamlineGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorStreamlineGroupDataItem() override;

	void updateActorSetting() override;
	void update();

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	std::string target() const override;
	void setTarget(const std::string& target) override;

	virtual void informGridUpdate();
	virtual void setupActors() = 0;
	void clearActors();

	void innerUpdateZScale(double zscale) override;
	void applyZScale();

	static void setupStreamTracer(vtkStreamTracer* tracer);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

protected:
	Setting m_setting;

	std::vector<vtkActor*> m_streamlineActors;

private:
	double m_zScale;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEGROUPDATAITEM_H
