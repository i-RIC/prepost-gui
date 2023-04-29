#include "gridbirdeyeobjectbrowserview.h"

GridBirdEyeObjectBrowserView::GridBirdEyeObjectBrowserView(QWidget* parent) :
	ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::NoEditTriggers);
	setEditTriggers(triggers);
}
