#ifndef POST2DWINDOWGEODATADATAITEM_H
#define POST2DWINDOWGEODATADATAITEM_H

#include <guicore/datamodel/geodataproxydataitem.h>
#include <guicore/post/post2d/base/post2dwindowgeodatadataitemi.h>

class GeoDataProxy;

class Post2dWindowGeoDataDataItem : public GeoDataProxyDataItem, public Post2dWindowGeoDataDataItemI
{
	Q_OBJECT

public:
	Post2dWindowGeoDataDataItem(GraphicsWindowDataItem* parent);
	~Post2dWindowGeoDataDataItem();

	GeoDataProxy* geoDataProxy() const;
	void setGeoDataProxy(GeoDataProxy* proxy);

	bool addToolBarButtons(QToolBar* toolBar) override;
	DelegatedColorMapSettingContainer* colorMapSetting() const override;
	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;
	void updateZDepthRangeItemCount() override;
	GraphicsWindowDataModel* dataModel() const override;
	void handleStandardItemChange() override;
	void applyColorMapSetting();

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
