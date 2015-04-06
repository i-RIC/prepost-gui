#ifndef PREFERENCEPAGEPOST3D_H
#define PREFERENCEPAGEPOST3D_H

#include "preferencepage.h"

namespace Ui {
    class PreferencePagePost3d;
}

class PreferencePagePost3d : public PreferencePage
{
    Q_OBJECT
public:
    explicit PreferencePagePost3d(QWidget *parent = 0);
    ~PreferencePagePost3d();
	void update();
private:
    Ui::PreferencePagePost3d *ui;
};

#endif // PREFERENCEPAGEPOST3D_H
