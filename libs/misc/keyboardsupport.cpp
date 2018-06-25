#include "keyboardsupport.h"

#include <Qt>

namespace iRIC {

bool isEnterKey(int key)
{
	return (key == Qt::Key_Enter || key == Qt::Key_Return);
}

} // iRIC
