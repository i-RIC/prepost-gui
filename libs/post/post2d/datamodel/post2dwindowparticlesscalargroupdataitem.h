#ifndef POST2DWINDOWPARTICLESSCALARGROUPDATAITEM_H
#define POST2DWINDOWPARTICLESSCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guibase/scalarsettingcontainer.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QMenu>
#include <QMouseEvent>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkScalarBarWidget.h>

class NamedGraphicWindowDataItem;

class Post2dWindowParticlesScalarGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowParticlesScalarGroupDataItem(Post2dWindowDataItem* parent);
	virtual ~Post2dWindowParticlesScalarGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateZDepthRangeItemCount();
	void assignActionZValues(const ZDepthRange& range);
	void update();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	void addCustomMenuItems(QMenu* menu);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void updateVisibility(bool visible) override;

private:
	void setupActors();
	void updateActorSettings();
	void setupScalarBarSetting();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	ScalarSettingContainer m_setting;
	QMap<std::string, QString> m_scalarbarTitleMap;

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

	class SetSettingCommand;
};

#endif // POST2DWINDOWPARTICLESSCALARGROUPDATAITEM_H
