#ifndef POST2DWINDOWPARTICLEIMAGEDATAITEM_H
#define POST2DWINDOWPARTICLEIMAGEDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowParticleImageTopDataItem;

class Post2dWindowParticleImageDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticleImageDataItem(const std::string& groupName, const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowParticleImageDataItem();

	const std::string& name() const;

	void update();
	QDialog* propertyDialog(QWidget* parent) override;

public slots:
	void showPropertyDialog() override;

private:
	Post2dWindowParticleImageTopDataItem* topDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;

	void updateActorSetting() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	const std::string m_groupName;

	class Setting;
	class SettingEditWidget;
	class ValueSizePair;

	class Impl;
	Impl* impl;
};

#endif // POST2DWINDOWPARTICLEIMAGEDATAITEM_H
