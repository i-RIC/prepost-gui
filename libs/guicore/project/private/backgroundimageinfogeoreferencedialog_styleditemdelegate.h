#ifndef BACKGROUNDIMAGEINFOGEOREFERENCEDIALOG_STYLEDITEMDELEGATE_H
#define BACKGROUNDIMAGEINFOGEOREFERENCEDIALOG_STYLEDITEMDELEGATE_H

#include "../backgroundimageinfogeoreferencedialog.h"
#include <QStyledItemDelegate>

class BackgroundImageInfoGeoreferenceDialog::StyledItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit StyledItemDelegate(QObject* parent = nullptr);
	QString displayText(const QVariant& value, const QLocale& locale) const override;

	void setEditorData(QWidget *editor, const QModelIndex& index) const override;

private:
	int m_decimals;
};

#endif // BACKGROUNDIMAGEINFOGEOREFERENCEDIALOG_STYLEDITEMDELEGATE_H
