#ifndef POST3DWINDOWNODEVECTORARROWDATAITEM_H
#define POST3DWINDOWNODEVECTORARROWDATAITEM_H

#include "post3dwindowfacedataitem.h"
#include "../post3dwindowdataitem.h"
#include "../post3dwindowfacesettingcontainer.h"
#include "../post3dsamplingratesettingcontainer.h"

#include <guicore/arrows/arrowssettingcontainer.h>
#include <guicore/filter/structured3dfilteringsettingcontainer.h>
#include <misc/compositecontainer.h>

class ArrowsSettingToolBarWidget;
class Post3dWindowNodeVectorArrowGroupDataItem;

class Post3dWindowNodeVectorArrowDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	class Setting : public CompositeContainer {
	public:
		Setting();
		Setting(const Setting& setting);

		Setting& operator=(const Setting& c);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

		Post3dWindowFaceSettingContainer face;
		Structured3dFilteringSettingContainer filtering;
		ArrowsSettingContainer arrow;
	};

	Post3dWindowNodeVectorArrowDataItem(const QString& label, Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorArrowDataItem();

	vtkStructuredGrid* faceGrid() const;

	Setting setting() const;
	void setSetting(const Setting& setting);

	void update();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;
	void handleStandardItemChange() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void innerUpdateZScale(double scale) override;
	void innerUpdate2Ds() override;

	void setupActors();
	void updateActorSetting() override;
	void updateVisibility(bool visible) override;
	Post3dWindowNodeVectorArrowGroupDataItem* groupDataItem() const;

	vtkActor* m_actor;

	Setting m_setting;
	double m_zScale;

	ArrowsSettingToolBarWidget* m_arrowsToolBarWidget;
};

#endif // POST3DWINDOWNODEVECTORARROWDATAITEM_H
