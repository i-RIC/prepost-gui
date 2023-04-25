#ifndef POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindowparticlesbasescalargroupdataitem.h"

#include <QDialog>

#include <postbase/particle/particledatasetting.h>

namespace Ui {
class Post2dWindowParticlesBaseScalarGroupDataItem_PropertyDialog;
}

class ColorMapSettingEditWidget;

class Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post2dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply = false);

	void accept() override;
	void reject() override;

private slots:
	void colorTargetChanged(int index);
	void handleButtonClick(QAbstractButton* button);

private:
	ParticleDataSetting setting() const;
	void setSetting(const ParticleDataSetting& setting);
	void apply();

	void setupColorTargets();
	std::string colorTarget() const;

	bool m_applied;

	std::vector<std::string> m_colorTargets;

	ColorMapSettingEditWidget* m_colorMapWidget;
	Post2dWindowParticlesBaseScalarGroupDataItem* m_item;

	Ui::Post2dWindowParticlesBaseScalarGroupDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_PROPERTYDIALOG_H
