#ifndef POSTPARTICLEBASICPROPERTYDIALOG_H
#define POSTPARTICLEBASICPROPERTYDIALOG_H

#include "postbase_global.h"
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
	explicit PostParticleBasicPropertyDialog(QWidget* parent = 0);
	~PostParticleBasicPropertyDialog();
	const QColor color() const;
	int size() const;
	void setColor(const QColor color);
	void setSize(int size);

private:
	Ui::PostParticleBasicPropertyDialog* ui;
};

#endif // POSTPARTICLEBASICPROPERTYDIALOG_H
