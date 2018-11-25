#ifndef LOG_H
#define LOG_H
#include <QString>

class LOG
{
public:
    LOG();
    static void LogInformation(const QString& log);
    static void LogError(const QString& log);
};

#endif // LOG_H
