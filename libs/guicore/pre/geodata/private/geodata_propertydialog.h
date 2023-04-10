#ifndef GEODATA_PROPERTYDIALOG_H
#define GEODATA_PROPERTYDIALOG_H

#include "../../../guicore_global.h"
#include "../geodata.h"

#include <misc/modifycommanddialog.h>

class GUICOREDLL_EXPORT GeoData::PropertyDialog : public ModifyCommandDialog
{
public:
	PropertyDialog(GeoData* geoData, QWidget* parent);

	void pushCommand(QUndoCommand* command) override;

private:
	GeoData* m_geoData;
};

#endif // GEODATA_PROPERTYDIALOG_H
