#ifndef POSTPARTICLEBASICPROPERTYDIALOG_H
#define POSTPARTICLEBASICPROPERTYDIALOG_H

#include "postbase_global.h"
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/colorcontainer.h>

#include <QDialog>

class QColor;

namespace Ui
{
	class PostParticleBasicPropertyDialog;
}

class POSTBASEDLL_EXPORT PostParticleBasicPropertyDialog : public QDialog
{
	Q_OBJECT

public:
	struct POSTBASEDLL_EXPORT Setting : public CompositeContainer
	{
		/// Constructor
		Setting();

		ColorContainer color;
		IntContainer size;
	};

	explicit PostParticleBasicPropertyDialog(QWidget* parent = 0);
	~PostParticleBasicPropertyDialog();
	Setting setting() const;
	void setSetting(const Setting& s);

private:
	Ui::PostParticleBasicPropertyDialog* ui;
};

#endif // POSTPARTICLEBASICPROPERTYDIALOG_H
