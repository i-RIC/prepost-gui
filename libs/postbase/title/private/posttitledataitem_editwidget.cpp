#include "posttitledataitem_editwidget.h"
#include "ui_posttitledataitem_editwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

PostTitleDataItem::EditWidget::EditWidget(PostTitleDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::PostTitleDataItem_EditWidget)
{
	ui->setupUi(this);

	setSetting(item->m_setting);
}

PostTitleDataItem::EditWidget::~EditWidget()
{
	delete ui;
}

PostTitleSetting PostTitleDataItem::EditWidget::setting() const
{
	PostTitleSetting ret;
	ret.title = ui->titleLineEdit->text().trimmed();
	ret.fontSetting = ui->fontSettingWidget->setting();
	return ret;
}

void PostTitleDataItem::EditWidget::setSetting(const PostTitleSetting& setting)
{
	ui->titleLineEdit->setText(setting.title);
	ui->fontSettingWidget->setSetting(setting.fontSetting);
}

QUndoCommand* PostTitleDataItem::EditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<PostTitleSetting>(iRIC::generateCommandId("PostTitleDataItem::EditWidget"), apply, setting(), &m_item->m_setting);
}
