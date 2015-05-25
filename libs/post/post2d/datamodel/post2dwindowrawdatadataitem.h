#ifndef POST2DWINDOWRAWDATADATAITEM_H
#define POST2DWINDOWRAWDATADATAITEM_H

#include <guicore/datamodel/rawdataproxydataitem.h>

class RawDataProxy;

class Post2dWindowRawDataDataItem : public RawDataProxyDataItem
{
	Q_OBJECT
public:
	Post2dWindowRawDataDataItem(GraphicsWindowDataItem* parent);
	~Post2dWindowRawDataDataItem();
	void setRawDataProxy(RawDataProxy* proxy);
	RawDataProxy* rawDataProxy() const {return m_rawDataProxy;}
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
	RawDataProxy* m_rawDataProxy;

};

#endif // POST2DWINDOWRAWDATADATAITEM_H
