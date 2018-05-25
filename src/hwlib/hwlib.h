#ifndef HWLIB_H
#define HWLIB_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QString>

#include <fcntl.h>
#include <unistd.h>

#include "common/script.h"
#include "common/debug.h"

#include "hwlib/gpio.h"
#include "hwlib/venus634lpx.h"
#include "hwlib/ssd1306.h"

class HwLib : public QObject
{
    Q_OBJECT
public:
    explicit HwLib( QObject *parent = 0 );
    ~HwLib();

    void InitScript( Script *script );
    void InitDebug( Debug *debug );

public slots:
    void Heartbeat();
    void ToggleHeart();

signals:
    void Stopheart();
    void Startheart();

private:
    int mFd;

    GPIO mGpio;
    Venus634LPx mGps;
    SSD1306 mOled;
    // Serial mSerial;
    // Venus634LPx mGps;

    QTimer mHeartbeat;
    QThread *mGpsThread;

};

#endif // HWLIB_H
