#ifndef POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H

#include "../post3dwindowdataitem.h"

class QAction;
class QColor;

class Post3dWindowNodeVectorParticleDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowNodeVectorParticleDataItem(const QString& name, const QString& caption, Post3dWindowDataItem* parent);
	void handleStandardItemChange();
	const QString& name() const {return m_name;}
signals:
	void changed(Post3dWindowNodeVectorParticleDataItem*);
protected:
	void doLoadFromProjectMainFile(const QDomNode&) {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) {}
private:
	QString m_name;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H
