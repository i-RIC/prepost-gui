#ifndef POSTPARTICLEVECTORPROPERTYDIALOG_H
#define POSTPARTICLEVECTORPROPERTYDIALOG_H

#include "../postbase_global.h"

#include <guibase/arrowsettingcontainer.h>

#include <QDialog>

#include <string>
#include <vector>

namespace Ui {
class PostParticleVectorPropertyDialog;
}

class PostWindowGridTypeDataItemInterface;

class vtkPolyData;

class POSTBASEDLL_EXPORT PostParticleVectorPropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostParticleVectorPropertyDialog(QWidget *parent = nullptr);
	~PostParticleVectorPropertyDialog();

	void setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item);
	void setData(vtkPolyData* data);

	ArrowSettingContainer setting() const;
	void setSetting(const ArrowSettingContainer& setting);

private:
	void setupComboBoxes(vtkPolyData *data);

	PostWindowGridTypeDataItemInterface* m_gridTypeDataItem;

	std::vector<std::string> m_targets;
	std::vector<std::string> m_colorTargets;

	Ui::PostParticleVectorPropertyDialog *ui;
};

#endif // POSTPARTICLEVECTORPROPERTYDIALOG_H
