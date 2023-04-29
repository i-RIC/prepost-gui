#ifndef POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post3dWindowNodeVectorParticleGroupDataItem;

class Post3dWindowNodeVectorParticleDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

public slots:
	void showPropertyDialog() override;

private:
	QDialog* propertyDialog(QWidget* parent) override;

	Post3dWindowNodeVectorParticleGroupDataItem* groupDataItem() const;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H
