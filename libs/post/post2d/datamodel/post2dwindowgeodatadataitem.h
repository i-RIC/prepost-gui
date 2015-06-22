#ifndef POST2DWINDOWGEODATADATAITEM_H
#define POST2DWINDOWGEODATADATAITEM_H

#include <guicore/datamodel/geodataproxydataitem.h>

class GeoDataProxy;

class Post2dWindowGeoDataDataItem : public GeoDataProxyDataItem
{
	Q_OBJECT

public:
	Post2dWindowGeoDataDataItem(GraphicsWindowDataItem* parent);
	~Post2dWindowGeoDataDataItem();
	void setGeoDataProxy(GeoDataProxy* proxy);
	GeoDataProxy* geoDataProxy() const {return m_geoDataProxy;}
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void updateZDepthRangeItemCount() override;

private slots:
	void updateCaption(const QString& newcaption);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;

private:
	GeoDataProxy* m_geoDataProxy;

};

#endif // POST2DWINDOWGEODATADATAITEM_H
