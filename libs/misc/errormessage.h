#ifndef ERRORMESSAGE_H
#define ERRORMESSAGE_H

#include "misc_global.h"
#include <QString>

/// The class that contains error messages.
class MISCDLL_EXPORT ErrorMessage : public QString
{

public:
	ErrorMessage(const QString& msg);
};

#endif // ERRORMESSAGE_H
