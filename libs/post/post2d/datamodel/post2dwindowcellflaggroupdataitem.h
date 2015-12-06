#ifndef POST2DWINDOWCELLFLAGGROUPDATAITEM_H
#define POST2DWINDOWCELLFLAGGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <QString>
#include <QColor>

struct Post2dWindowCellFlagSetting {
	std::string attributeName;
	int value;
	QColor color;
	bool enabled;
	bool operator <(const Post2dWindowCellFlagSetting& s) const {
		if (attributeName != s.attributeName) {
			return attributeName < s.attributeName;
		}
		if (value != s.value) {
			return value < s.value;
		}
		return false;
	}
};

class Post2dWindowCellFlagDataItem;

class Post2dWindowCellFlagGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowCellFlagGroupDataItem(Post2dWindowDataItem* parent);
	void update();
	bool hasTransparentPart() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	QDialog* propertyDialog(QWidget* /*parent*/) override;
	void handlePropertyDialogAccepted(QDialog*) override;

private:
	QList<Post2dWindowCellFlagSetting> settings();
	void initSetting();
	void setSettings(const QList<Post2dWindowCellFlagSetting>& settings, int opacity);
	int m_opacityPercent;

	class SetSettingCommand;
};

#endif // POST2DWINDOWCELLFLAGGROUPDATAITEM_H
