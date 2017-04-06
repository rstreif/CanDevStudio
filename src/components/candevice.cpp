#include <utility>
#include <QQueue>
#include "candevice_p.h"
#include "candevice.h"


bool CanDevice::init(const QString &backend, const QString &interface)
{
    Q_D(CanDevice);
    QString errorString;

    d->mBackend = backend;
    d->mInterface = interface;
    d->mDevice.reset(QCanBus::instance()->createDevice(backend, interface, &errorString));

    if (d->mDevice) {
        return false;
    }

    connect(d->mDevice.take(), &QCanBusDevice::framesWritten, this, &CanDevice::framesWritten);
    connect(d->mDevice.take(), &QCanBusDevice::framesReceived, this, &CanDevice::framesReceived);
    connect(d->mDevice.take(), &QCanBusDevice::errorOccurred, this, &CanDevice::errorOccurred);

    return true;
}

void CanDevice::sendFrame(const QCanBusFrame &frame, const QVariant &context)
{
    Q_D(CanDevice);
    bool status = false;

    if (!d->mDevice)
        return;

    status = d->mDevice->writeFrame(frame);

    if(!status) {
        emit txStatus(status, frame, context);
    }
    else {
        // Success will be reported in framesWritten signal.
        // Sending may be buffered. Keep correlation between sending results and frame/context
        d->mSendQueue.enqueue({frame, context});
    }
}

bool CanDevice::start()
{
    Q_D(CanDevice);

    if(d->mDevice) {
        return d->mDevice->connectDevice();
    } else {
        return false;
    }
}

void CanDevice::framesReceived()
{
    Q_D(CanDevice);

    if (!d->mDevice)
        return;

    while (d->mDevice->framesAvailable()) {
        const QCanBusFrame frame = d->mDevice->readFrame();

        emit frameReceived(frame);
    }
}

void CanDevice::framesWritten(qint64) {
    Q_D(CanDevice);

    auto sendItem = d->mSendQueue.dequeue(); 
    emit txStatus(true, sendItem.first, sendItem.second);
}

void CanDevice::errorOccurred(QCanBusDevice::CanBusError error)
{
    Q_D(CanDevice);

    if(error == QCanBusDevice::WriteError) {
        auto sendItem = d->mSendQueue.dequeue(); 
        emit txStatus(false, sendItem.first, sendItem.second);
    }
}
