#ifndef POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
#define POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post3dWindowNodeVectorStreamlineGroupDataItem;

class Post3dWindowNodeVectorStreamlineDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post3dWindowNodeVectorStreamlineDataItem();

public slots:
	void showPropertyDialog() override;

private:
	QDialog* propertyDialog(QWidget* parent) override;

	Post3dWindowNodeVectorStreamlineGroupDataItem* groupDataItem() const;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
