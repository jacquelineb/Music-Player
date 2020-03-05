#include "trackdurationdelegate.h"

#include <QTime>


TrackDurationDelegate::TrackDurationDelegate()
{
}

QString TrackDurationDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    int durationInMillisec = value.toInt();
    int remainingMillisec = durationInMillisec;
    int hours = durationInMillisec / 1000 / 60 / 60;
    remainingMillisec = durationInMillisec - (hours * 1000 * 60 * 60);
    int minutes = remainingMillisec / 1000 / 60;
    remainingMillisec = durationInMillisec - (minutes * 1000 * 60);
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
