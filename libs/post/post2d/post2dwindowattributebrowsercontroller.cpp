#include "post2dwindow.h"
#include "post2dwindowattributebrowsercontroller.h"
#include "post2dwindowdataitem.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>

#include <vtkDataSetAttributes.h>

Post2dWindowAttributeBrowserController::Post2dWindowAttributeBrowserController(Post2dWindowDataItem* item) :
	m_item {item},
	m_fixed {false}
{}

Post2dWindowAttributeBrowserController::~Post2dWindowAttributeBrowserController()
{}

void Post2dWindowAttributeBrowserController::update(const QPoint& p, VTKGraphicsView* v)
{
	if (! propertyBrowser()->isVisible()) {return;}
	if (m_fixed) {return;}

	doUpdate(p, v);
}

void Post2dWindowAttributeBrowserController::fix(const QPoint& p, VTKGraphicsView* v)
{
	if (! propertyBrowser()->isVisible()) {return;}

	m_fixed = doUpdate(p, v);
}

void Post2dWindowAttributeBrowserController::clear()
{
	propertyBrowserView()->hideAll();
}

Post2dWindowDataItem* Post2dWindowAttributeBrowserController::item() const
{
	return m_item;
}

QList<PropertyBrowserAttribute> Post2dWindowAttributeBrowserController::buildAttributes(vtkIdType id, vtkDataSetAttributes* atts, v4Grid::AttributeDataProvider* provider) const
{
	QList<PropertyBrowserAttribute> ret;

	for (int i = 0; i < atts->GetNumberOfArrays(); ++i) {
		auto arr = atts->GetAbstractArray(i);
		auto da = dynamic_cast<vtkDataArray*> (arr);
		if (da == nullptr) {continue;}
		if (da->GetNumberOfComponents() == 1) {
			// scalar
			double val = da->GetComponent(id, 0);
			ret.push_back(PropertyBrowserAttribute(provider->caption(da->GetName()), provider->attributeBrowserValue(da->GetName(), val)));
		} else {
			// vector
			std::string name = da->GetName();

			double xVal = da->GetComponent(id, 0);
			auto nameX = name + "X";
			ret.push_back(PropertyBrowserAttribute(provider->caption(nameX), provider->attributeBrowserValue(nameX, xVal)));

			double yVal = da->GetComponent(id, 1);
			auto nameY = name + "Y";
			ret.push_back(PropertyBrowserAttribute(provider->caption(nameY), provider->attributeBrowserValue(nameY, yVal)));
		}
	}

	return ret;
}

PropertyBrowser* Post2dWindowAttributeBrowserController::propertyBrowser() const
{
	return post2dWindow()->propertyBrowser();
}

PropertyBrowserView* Post2dWindowAttributeBrowserController::propertyBrowserView() const
{
	return propertyBrowser()->view();
}

QPointF Post2dWindowAttributeBrowserController::worldPos(const QPoint& p, VTKGraphicsView* v)
{
	auto v2 = dynamic_cast<VTK2DGraphicsView*> (v);
	return v2->viewportToWorld(p);
}

Post2dWindow* Post2dWindowAttributeBrowserController::post2dWindow() const
{
	return dynamic_cast<Post2dWindow*> (m_item->mainWindow());
}
