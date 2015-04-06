#ifndef POST2DWINDOWRAWDATADATAITEM_H
#define POST2DWINDOWRAWDATADATAITEM_H

#include <guicore/datamodel/rawdataproxydataitem.h>

class RawDataProxy;

class Post2dWindowRawDataDataItem : public RawDataProxyDataItem
{
	Q_OBJECT
public:
	Post2dWindowRawDataDataItem(GraphicsWindowDataItem * parent);
	~Post2dWindowRawDataDataItem();
	void setRawDataProxy(RawDataProxy* proxy);
	RawDataProxy* rawDataProxy(){return m_rawDataProxy;}
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void updateZDepthRangeItemCount();

private slots:
	void updateCaption(const QString& newcaption);
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void assignActionZValues(const ZDepthRange& range);

private:
	RawDataProxy* m_rawDataProxy;

};

#endif // POST2DWINDOWRAWDATADATAITEM_H
