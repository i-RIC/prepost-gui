#ifndef POST2DWINDOWPARTICLESBASETOPDATAITEM_H
#define POST2DWINDOWPARTICLESBASETOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <unordered_set>

class ColorMapSettingContainerI;
class Post2dWindowParticlesBaseScalarGroupDataItem;
class Post2dWindowParticlesBaseVectorGroupTopDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowParticlesBaseTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesBaseTopDataItem(const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowParticlesBaseTopDataItem() override;

	void setup();

	Post2dWindowParticlesBaseScalarGroupDataItem* scalarGroupDataItem() const;
	Post2dWindowParticlesBaseVectorGroupTopDataItem* vectorGroupDataItem() const;

	QAction* showAttributeBrowserAction() const;

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	void updateColorMapLegendsVisibility();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	virtual vtkPolyData* particleData() const = 0;
	virtual Post2dWindowZoneDataItem* zoneDataItem() const = 0;

private slots:
	void showAttributeBrowser();

private:
	std::unordered_set<ColorMapSettingContainerI*> activeColorMapsWithVisibleLegend() const;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dWindowParticlesBaseScalarGroupDataItem* m_scalarGroupDataItem;
	Post2dWindowParticlesBaseVectorGroupTopDataItem* m_vectorGroupDataItem;

	QAction* m_showAttributeBrowserAction;
};

#endif // POST2DWINDOWPARTICLESBASETOPDATAITEM_H
