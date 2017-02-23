#ifndef __SOCKETCAN_H
#define __SOCKETCAN_H

#include <common/component.h>
#include <common/event_canframe.h>
#include <experimental/any>
#include <memory>
#include <QObject>
#include <QCanBus>
#include <QCanBusFrame>
#include <QCanBusDevice>
#include <QVariant>
#include <QDebug>
#include <QTranslator>

class CanInterface
{
public:
    virtual ~CanInterface() {}

    virtual bool configure(const std::experimental::any settings) = 0;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool send(const CanFrame &frame) = 0;
    virtual void setReceiveClbk(const CanFrameClbk &&clbk) = 0;
};


typedef QPair<QCanBusDevice::ConfigurationKey, QVariant> ConfigurationItem;

struct Settings {
    std::string backendName;
    std::string deviceInterfaceName;
};

class QtCanBackend : public QObject, public CanInterface
{
    Q_OBJECT

public:
    QtCanBackend() : QObject(nullptr) {}

    virtual bool configure(const std::experimental::any settings)
    {
        Settings p = std::experimental::any_cast<Settings>(settings);
        QString errorString;

        mCanDevice.reset(QCanBus::instance()->createDevice(QString::fromStdString(p.backendName), QString::fromStdString(p.deviceInterfaceName), &errorString));
        if (!mCanDevice) {
            return false;
        }


        connect(mCanDevice.get(), &QCanBusDevice::errorOccurred, this, &QtCanBackend::receiveError);
        connect(mCanDevice.get(), &QCanBusDevice::framesReceived, this, &QtCanBackend::frameReceived);
        connect(mCanDevice.get(), &QCanBusDevice::framesWritten, this, &QtCanBackend::framesWritten);

        return true;
    }

    virtual bool start()
    {
        if(mCanDevice) {
            return mCanDevice->connectDevice();
        } else {
            return false;
        }
    }

    virtual void stop()
    {
        if(mCanDevice) {
            mCanDevice->disconnectDevice();
        }
    }

    virtual bool send(const CanFrame &frame)
    {
        return false;
    }

    virtual void setReceiveClbk(const CanFrameClbk &&clbk)
    {
        mRcvClbk = clbk;
    }

private:
    void framesWritten(qint64 count)
    {
        qDebug() << "Number of frames written:" << count;
    }

    void frameReceived()
    {
        if (!mCanDevice)
            return;

        while (mCanDevice->framesAvailable()) {
            const QCanBusFrame qf = mCanDevice->readFrame();
            CanFrame cf;

            cf.id = qf.frameId();
            cf.data = std::vector<uint8_t>(qf.payload().constData(), qf.payload().constData() + qf.payload().size());
            cf.extended = qf.hasExtendedFrameFormat();
            cf.error = qf.error() != QCanBusFrame::NoError;

            if(mRcvClbk) { 
                mRcvClbk(cf);
            }
        }
    }

    void receiveError(QCanBusDevice::CanBusError error)
    {
        switch (error) {
        case QCanBusDevice::ReadError:
        case QCanBusDevice::WriteError:
        case QCanBusDevice::ConnectionError:
        case QCanBusDevice::ConfigurationError:
        case QCanBusDevice::UnknownError:
            qWarning() << mCanDevice->errorString();
        default:
            break;
        }
    }

    std::unique_ptr<QCanBusDevice> mCanDevice;
    CanFrameClbk mRcvClbk;
};

struct SocketCan : public Component<CanFrameSink, CanFrameSource> {

    SocketCan() : canIf(std::make_unique<QtCanBackend>())
    {
        canIf->setReceiveClbk([this] (const CanFrame &frame)
                {
                    std::get<CanFrameSource>(mElements).emitEvent(frame);
                });
    }
        

    bool configure(const std::experimental::any settings)
    {
        if(canIf) {
            return canIf->configure(settings);
        }

        return false;
    }

    bool start() 
    {
        if(canIf) {
            return canIf->start();
        }

        return false;
    }

    //void stop() = 0;
    //bool send(const CanFrame &frame) = 0;
    //void receive(const CanFrame &frame) = 0;
private:
    std::unique_ptr<CanInterface> canIf;
};

#endif /* !__SOCKETCAN_H */
