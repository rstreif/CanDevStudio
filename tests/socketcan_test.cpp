#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>
#include <QCoreApplication>
#include <QTimer>
#include <QVariant>
#include <candevice.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTimer timer;
    CanDevice cd;
    int cnt = 0;

    std::cout << cd.init("socketcan", "can0");
    std::cout << cd.start();
    
    QObject::connect(&timer, &QTimer::timeout, [&cnt, &cd] () {
                QCanBusFrame f;
                f.setFrameId(cnt++);
                f.setPayload(QByteArray::fromHex("123456"));
                cd.sendFrame(f, {});
            });

    timer.start(1000);

    std::cout << QObject::connect(&cd, &CanDevice::frameReceived, [] (const QCanBusFrame &frame) {
                printf("r(1) %03X [%d] %s\n", frame.frameId(), frame.payload().size(), frame.payload().toHex().data());
            });

    QObject::connect(&cd, &CanDevice::txStatus, [] (bool status, const QCanBusFrame &frame, const QVariant &) {
                printf("s(%hhu) %03X [%d] %s\n", status, frame.frameId(), frame.payload().size(), frame.payload().toHex().data());
            });

    std::cout << std::endl;

    return a.exec();
}

