#include "trackdurationdelegate.h"
#include <QTime>
#include <QDebug>

TrackDurationDelegate::TrackDurationDelegate()
{

}

QString TrackDurationDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    int millisec = value.toInt();
    int remainingMillisec = millisec;
    int hours = millisec / 1000 / 60 / 60;
    remainingMillisec = millisec - (hours * 1000 * 60 * 60);
    int minutes = remainingMillisec / 1000 / 60;
    remainingMillisec = millisec - (minutes * 1000 * 60);
    int seconds = remainingMillisec / 1000;
    remainingMillisec = remainingMillisec - (seconds * 1000);

    QTime time(hours, minutes, seconds);
    QString format;
    if (hours)
    {
        format = "h:mm:ss";
    }
    else
    {
        format = "m:ss";
    }
    QString timeString = time.toString(format);
    return timeString;
}
