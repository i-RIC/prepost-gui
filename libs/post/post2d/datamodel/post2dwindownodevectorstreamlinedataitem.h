#ifndef POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H

#include "../post2dwindowdataitem.h"

class QAction;
class QColor;

class Post2dWindowNodeVectorStreamlineDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post2dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent);

	const std::string& name() const;

	void handleStandardItemChange() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;

signals:
	void changed(Post2dWindowNodeVectorStreamlineDataItem*);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	std::string m_name;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
