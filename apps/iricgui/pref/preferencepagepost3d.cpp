#include "preferencepagepost3d.h"
#include "ui_preferencepagepost3d.h"

PreferencePagePost3d::PreferencePagePost3d(QWidget* parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePagePost3d)
{
	ui->setupUi(this);
}

PreferencePagePost3d::~PreferencePagePost3d()
{
	delete ui;
}

void PreferencePagePost3d::update()
{

}
