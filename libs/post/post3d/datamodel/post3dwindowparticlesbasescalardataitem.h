#ifndef POST3DWINDOWPARTICLESBASESCALARDATAITEM_H
#define POST3DWINDOWPARTICLESBASESCALARDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>

class Post3dWindowParticlesBaseScalarGroupDataItem;

class vtkActor2D;

class Post3dWindowParticlesBaseScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post3dWindowParticlesBaseScalarDataItem();

	void update();
	ColorMapSettingContainer& colorMapSetting();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

private:
	Post3dWindowParticlesBaseScalarGroupDataItem* groupDataItem() const;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	ColorMapSettingContainer m_colorMapSetting;
	vtkActor2D* m_legendActor;
};

#endif // POST3DWINDOWPARTICLESBASESCALARDATAITEM_H
