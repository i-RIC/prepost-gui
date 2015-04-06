#ifndef POST2DWINDOWCELLFLAGGROUPDATAITEM_H
#define POST2DWINDOWCELLFLAGGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <QString>
#include <QColor>

struct Post2dWindowCellFlagSetting {
	QString attributeName;
	int value;
	QColor color;
	bool enabled;
	bool operator <(const Post2dWindowCellFlagSetting& s) const {
		if (attributeName != s.attributeName){
			return attributeName < s.attributeName;
		}
		if (value != s.value){
			return value < s.value;
		}
		return false;
	}
};

class Post2dWindowCellFlagGroupSettingCommand;
class Post2dWindowCellFlagDataItem;

class Post2dWindowCellFlagGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowCellFlagGroupDataItem(Post2dWindowDataItem* parent);
	void update();
	bool hasTransparentPart();
	void informSelection(VTKGraphicsView * v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);

protected:
	void doLoadFromProjectMainFile(const QDomNode & node);
	void doSaveToProjectMainFile(QXmlStreamWriter & writer);
	QDialog* propertyDialog(QWidget* /*parent*/);
	void handlePropertyDialogAccepted(QDialog*);

private:
	QList<Post2dWindowCellFlagSetting> settings();
	void initSetting();
	void setSettings(const QList<Post2dWindowCellFlagSetting>& settings, int opacity);
	int m_opacityPercent;

public:
	friend class Post2dWindowCellFlagGroupSettingCommand;
};

#endif // POST2DWINDOWCELLFLAGGROUPDATAITEM_H
