#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include <QtCore/qobject.h>
#include <QCanBus>
#include <QCanBusFrame>
#include <QScopedPointer>
#include <QVariant>
#include "candevice.h"

class CanDevicePrivate 
{
public:
    QString mBackend;
    QString mInterface;
    QQueue<std::pair<QCanBusFrame, QVariant>> mSendQueue;
    QScopedPointer<QCanBusDevice> mDevice;
};

#endif /* !__CANDEVICE_P_H */
