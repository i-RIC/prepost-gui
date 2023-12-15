#ifndef NAMEDQSTRINGGRAPHICWINDOWDATAITEM_H
#define NAMEDQSTRINGGRAPHICWINDOWDATAITEM_H

#include "../guicore_global.h"
#include "../datamodel/graphicswindowdataitem.h"

#include <string>

class GUICOREDLL_EXPORT NamedQStringGraphicWindowDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	NamedQStringGraphicWindowDataItem(const QString& name, const QString& caption, GraphicsWindowDataItem* parent);
	virtual ~NamedQStringGraphicWindowDataItem();

	const QString& name() const;
	void handleStandardItemChange() override;

signals:
	void changed(NamedQStringGraphicWindowDataItem*);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QString m_name;
};

#endif // NAMEDQSTRINGGRAPHICWINDOWDATAITEM_H
