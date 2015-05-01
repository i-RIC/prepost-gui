#ifndef POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <guibase/contoursettingwidget.h>

class SolverDefinitionGridRelatedCondition;
class PostZoneDataContainer;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class Post2dBirdEyeWindowNodeScalarDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dBirdEyeWindowNodeScalarDataItem(const QString& name, const QString& caption, Post2dBirdEyeWindowDataItem* parent);
	void handleStandardItemChange();
	const QString& name() const {return m_name;}
signals:
	void changed(Post2dBirdEyeWindowNodeScalarDataItem*);
protected:
	void doLoadFromProjectMainFile(const QDomNode&) {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) {}
private:
	QString m_name;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H
