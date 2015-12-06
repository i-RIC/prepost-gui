#ifndef POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <guibase/contoursettingwidget.h>

class SolverDefinitionGridAttribute;
class PostZoneDataContainer;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class Post2dBirdEyeWindowNodeScalarDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post2dBirdEyeWindowNodeScalarDataItem(const std::string& name, const QString& caption, Post2dBirdEyeWindowDataItem* parent);
	const std::string& name() const {return m_name;}

	void handleStandardItemChange() override;

signals:
	void changed(Post2dBirdEyeWindowNodeScalarDataItem*);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	std::string m_name;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H
