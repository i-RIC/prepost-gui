#ifndef POSTPARTICLEVECTORPROPERTYDIALOG_H
#define POSTPARTICLEVECTORPROPERTYDIALOG_H

#include "postbase_global.h"
#include <misc/arrowsettingcontainer.h>
#include <misc/arrowshapecontainer.h>
#include <QDialog>

namespace Ui {
class PostParticleVectorPropertyDialog;
}

class ArrowShapeContainer;
class PostWindowGridTypeDataItemInterface;
class PostZoneDataContainer;

class POSTBASEDLL_EXPORT PostParticleVectorPropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostParticleVectorPropertyDialog(QWidget *parent = nullptr);
	~PostParticleVectorPropertyDialog();
	void setZoneData(PostZoneDataContainer* data);

	ArrowSettingContainer arrowSetting() const;
	void setArrowSetting(const ArrowSettingContainer& arrowSetting);

	ArrowShapeContainer arrowShape() const;
	void setArrowShape(const ArrowShapeContainer& arrowShape);

private:
	void setupComboBoxes(PostZoneDataContainer *zoneData);

	QList<QString> m_attributes;
	QList<QString> m_scalars;

	Ui::PostParticleVectorPropertyDialog *ui;
};

#endif // POSTPARTICLEVECTORPROPERTYDIALOG_H
