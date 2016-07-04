#ifndef EXECUTERI_H
#define EXECUTERI_H

class QString;

class ExecuterI
{
public:
	virtual ~ExecuterI() {}

	virtual void start() = 0;
	virtual void wait(int msec) = 0;
	virtual void cancel() = 0;

	virtual bool isFinished() const = 0;
	virtual bool isCanceled() const = 0;
};

#endif // EXECUTERI_H

