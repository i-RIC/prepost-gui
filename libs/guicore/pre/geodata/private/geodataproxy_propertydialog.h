#ifndef GEODATAPROXY_PROPERTYDIALOG_H
#define GEODATAPROXY_PROPERTYDIALOG_H

#include "../../../guicore_global.h"

#include "../geodataproxy.h"

#include <misc/modifycommanddialog.h>

class GUICOREDLL_EXPORT GeoDataProxy::PropertyDialog : public ModifyCommandDialog
{
public:
	PropertyDialog(GeoDataProxy* proxy, QWidget* parent);

	void pushCommand(QUndoCommand* command) override;

private:
	GeoDataProxy* m_proxy;
};

#endif // GEODATAPROXY_PROPERTYDIALOG_H
