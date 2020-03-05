#ifndef TRACKDURATIONDELEGATE_H
#define TRACKDURATIONDELEGATE_H

#include <QStyledItemDelegate>

class TrackDurationDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TrackDurationDelegate();
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // TRACKDURATIONDELEGATE_H
