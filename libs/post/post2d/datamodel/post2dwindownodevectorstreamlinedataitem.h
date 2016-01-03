#ifndef POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowNodeVectorStreamlineDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
