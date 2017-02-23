#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>
#include <iostream>
#include <components/socketcan.h>
#include <QCoreApplication>
#include <QCanBus>
#include <QCanBusFrame>
#include <QCanBusDevice>
#include <QVariant>
#include <QDebug>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
 
    SocketCan sc;
    Settings s {"socketcan", "can0"};

    sc.configure(s);
    sc.start();
    sc.connect(EventType::CanFrame, [] (const CanFrame &cf) {
                std::cout << cf.id << std::endl;
            });

    return a.exec();
}

