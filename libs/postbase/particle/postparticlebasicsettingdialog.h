#ifndef POSTPARTICLEBASICSETTINGDIALOG_H
#define POSTPARTICLEBASICSETTINGDIALOG_H

#include "../postbase_global.h"

#include "postparticlebasicsetting.h"

#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/colorcontainer.h>

#include <QDialog>

class QColor;

namespace Ui
{
	class PostParticleBasicSettingDialog;
}

class POSTBASEDLL_EXPORT PostParticleBasicSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostParticleBasicSettingDialog(QWidget* parent = 0);
	~PostParticleBasicSettingDialog();

	PostParticleBasicSetting setting() const;
	void setSetting(const PostParticleBasicSetting& s);

private:
	Ui::PostParticleBasicSettingDialog* ui;
};

#endif // POSTPARTICLEBASICSETTINGDIALOG_H
