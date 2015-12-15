#ifndef POST3DWINDOWPARTICLESVECTORDATAITEM_H
#define POST3DWINDOWPARTICLESVECTORDATAITEM_H

#include "../post3dwindowdataitem.h"

class SolverDefinitionGridRelatedCondition;
class QAction;

class Post3dWindowParticlesVectorDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post3dWindowParticlesVectorDataItem(const QString& name, const QString& caption, Post3dWindowDataItem* parent);
	void handleStandardItemChange() override;
	const QString& name() const {return m_name;}

signals:
	void changed(Post3dWindowParticlesVectorDataItem*);
protected:
	void doLoadFromProjectMainFile(const QDomNode&) {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) {}
private:
	QString m_name;
};

#endif // POST3DWINDOWPARTICLESVECTORDATAITEM_H
