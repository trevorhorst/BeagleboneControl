#ifndef LED_H
#define LED_H

#include <QObject>
#include <QTimer>

#include "hwlib/gpio.h"

class Led : public QObject
{
    Q_OBJECT
public:
    explicit Led(uint8_t bank, uint8_t pin, QObject *parent = nullptr );
    ~Led();

    static bool mVerbose;
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose ) { mVerbose = verbose; }
    static uint32_t GetCount();

    void Init( GPIO *gpio );

signals:

public slots:
    void Enable( bool enable = true );
    void Toggle();
    void SetBlink( bool enable = true );
    bool SetInterval( quint16 interval );

private:
    bool mBlink;

    GPIO *mGpio;
    uint8_t mBank;
    uint8_t mPin;

    uint32_t mId;
    static QTimer mTimer;
    static uint32_t mCount;
};

#endif // LED_H
