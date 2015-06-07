#ifndef MEASUREDDATAVECTORDATAITEM_H
#define MEASUREDDATAVECTORDATAITEM_H

#include "../datamodel/graphicswindowdataitem.h"

class QAction;

class MeasuredDataVectorDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	MeasuredDataVectorDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem* parent);
	void handleStandardItemChange() override;
	const QString& name() const {return m_name;}

signals:
	void changed(MeasuredDataVectorDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) override {}

private:
	QString m_name;
};

#endif // MEASUREDDATAVECTORDATAITEM_H
