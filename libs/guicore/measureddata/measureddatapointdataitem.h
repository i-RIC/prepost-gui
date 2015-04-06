#ifndef MEASUREDDATAPOINTDATAITEM_H
#define MEASUREDDATAPOINTDATAITEM_H

#include "../datamodel/graphicswindowdataitem.h"

class MeasuredDataPointDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	MeasuredDataPointDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem* parent);
	void handleStandardItemChange();
	const QString& name() const {return m_name;}
signals:
	void changed(MeasuredDataPointDataItem*);
protected:
	void doLoadFromProjectMainFile(const QDomNode&){}
	void doSaveToProjectMainFile(QXmlStreamWriter&){}
private:
	QString m_name;
};

#endif // MEASUREDDATAPOINTDATAITEM_H
