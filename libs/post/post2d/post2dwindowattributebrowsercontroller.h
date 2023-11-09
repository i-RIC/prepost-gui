#ifndef POST2DWINDOWATTRIBUTEBROWSERCONTROLLER_H
#define POST2DWINDOWATTRIBUTEBROWSERCONTROLLER_H

#include <guicore/grid/public/v4grid_attributedataprovider.h>

class Post2dWindow;
class Post2dWindowDataItem;
class PropertyBrowser;
class PropertyBrowserAttribute;
class PropertyBrowserView;
class VTKGraphicsView;

class Post2dWindowAttributeBrowserController
{
public:
	Post2dWindowAttributeBrowserController(Post2dWindowDataItem* item);
	virtual ~Post2dWindowAttributeBrowserController();

	virtual void initialize() = 0;
	void update(const QPoint& p, VTKGraphicsView* v);
	void fix(const QPoint& p, VTKGraphicsView* v);
	void clear();

protected:
	QPointF worldPos(const QPoint& p, VTKGraphicsView* v);
	PropertyBrowser* propertyBrowser() const;
	PropertyBrowserView* propertyBrowserView() const;
	Post2dWindowDataItem* item() const;
	QList<PropertyBrowserAttribute> buildAttributes(vtkIdType id, vtkDataSetAttributes* atts, v4Grid::AttributeDataProvider* provider) const;

private:
	virtual bool doUpdate(const QPoint& p, VTKGraphicsView* v) = 0;

	Post2dWindow* post2dWindow() const;

	Post2dWindowDataItem* m_item;
	bool m_fixed;
};

#endif // POST2DWINDOWATTRIBUTEBROWSERCONTROLLER_H
