#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_H

#include "post2dwindowdataitem.h"

#include <guibase/scalarsettingcontainer.h>
#include <guibase/vtktool/vtkactorpolydatamapperpair.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/polydata/postpolydatabasicsetting.h>

class NamedGraphicWindowDataItem;

class vtkScalarBarWidget;

class Post2dWindowPolyDataGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowPolyDataGroupDataItem(const std::string& name, Post2dWindowDataItem* parent);
	~Post2dWindowPolyDataGroupDataItem() override;

	std::string name() const;

	QColor color() const;
	void setColor(const QColor& c);

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void setupActors();
	void update();
	void updateActorSettings();

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
  void updateVisibility(bool visible) override;

	Post2dWindowZoneDataItem* zoneDataItem() const;

	vtkActorPolyDataMapperPair m_pair;
	vtkScalarBarWidget* m_scalarBarWidget;

	PostPolyDataBasicSetting m_setting;
	ScalarSettingContainer m_scalarSetting;
	std::map<std::string, QString> m_scalarbarTitleMap;

	class SetBasicSettingCommand;
	class SetSettingCommand;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_H
