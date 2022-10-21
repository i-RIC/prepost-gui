#ifndef PREFERENCEPAGETEXTFORMAT_H
#define PREFERENCEPAGETEXTFORMAT_H

#include "preferencepage.h"

#include <QWidget>

namespace Ui {
class PreferencePageTextFormat;
}

class PreferencePageTextFormat : public PreferencePage
{
	Q_OBJECT

public:
	explicit PreferencePageTextFormat(QWidget *parent = nullptr);
	~PreferencePageTextFormat();

	void update() override;

private:
	Ui::PreferencePageTextFormat *ui;
};

#endif // PREFERENCEPAGETEXTFORMAT_H
