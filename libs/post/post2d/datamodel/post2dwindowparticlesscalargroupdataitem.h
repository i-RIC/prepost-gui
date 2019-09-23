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

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private:
	void setupActors();
	void updateActorSettings();
	void setupScalarBarSetting();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateVisibility(bool visible) override;

	ScalarSettingContainer m_setting;
	QMap<std::string, QString> m_scalarbarTitleMap;

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

	class SetSettingCommand;
};

#endif // POST2DWINDOWPARTICLESSCALARGROUPDATAITEM_H
