#ifndef TEXTEDITI_H
#define TEXTEDITI_H

class QString;

class TextEditI
{
public:
	virtual ~TextEditI() {}

	virtual QString text() const = 0;
	virtual void setText(const QString& text) = 0;
};

#endif // TEXTEDITI_H
