#ifndef ITEMSELECTINGDIALOG_DETAIL_H
#define ITEMSELECTINGDIALOG_DETAIL_H

#include "../itemselectingdialog.h"

#include <QComboBox>

template <typename STRCONTAINER>
void ItemSelectingDialog::setItems(const STRCONTAINER& items)
{
	auto c = comboBox();
	c->clear();
	for (auto item : items) {
		c->addItem(item);
	}
	c->setCurrentIndex(0);
}

#endif // ITEMSELECTINGDIALOG_DETAIL_H
