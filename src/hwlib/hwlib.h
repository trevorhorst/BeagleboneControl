#ifndef HWLIB_H
#define HWLIB_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QString>

#include <fcntl.h>
#include <unistd.h>
#include <vector>

#include "common/script.h"
#include "common/debug.h"

#include "hwlib/gpio.h"
#include "hwlib/clockmodule.h"
#include "hwlib/led.h"

#include "hwlib/ssd1306.h"
// #include "hwlib/venus634lpx.h"

class HwLib : public QObject
{
    Q_OBJECT
public:
    explicit HwLib( QObject *parent = 0 );
    ~HwLib();

    static bool mVerbose;
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose ){ mVerbose = verbose; }

    GPIO* GetGpio();
    ClockModule* GetClockModule();
    Led* GetLed( uint32_t led );

public slots:
    void Heartbeat();
    void ToggleHeart();

signals:
    void Stopheart();
    void Startheart();

private:
    int mFd;

    GPIO mGpio;
    ClockModule mClockModule;
    std::vector< Led* > mLeds;
    // Led *mLeds[ 4 ];
    // Venus634LPx mGps;
    SSD1306 mOled;
    // Serial mSerial;
    // Venus634LPx mGps;

    QTimer mHeartbeat;
    QThread *mGpsThread;
};

#endif // HWLIB_H
