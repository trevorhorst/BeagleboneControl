#ifndef CLOCKMODULE_H
#define CLOCKMODULE_H

#include <QObject>

#include "register.h"

class ClockModule : public QObject
{
    Q_OBJECT
public:
    explicit ClockModule( QObject *parent = 0 );
    ~ClockModule();

    static bool mVerbose;
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose ) { mVerbose = verbose; }

    void Init( int fd, bool simulated );

    void AddRegs();

    enum RegisterID {
        // Clock Module Peripheral Registers
        CM_PER_GPIO1_CLKCTRL        = 0
        , CM_PER_GPIO2_CLKCTRL      = 1
        , CM_PER_GPIO3_CLKCTRL      = 2

        // Clock Module Wakeup Registers
        , CM_WKUP_CLKSTCTRL         = 3
        , CM_WKUP_CONTROL_CLKCTRL   = 4
        , CM_WKUP_GPIO0_CLKCTRL     = 5

    };

    union REG_CM_PER_GPIO1_CLKCTRL {
        uint32_t mValue;
        Register::BitField<  0,  2 > mMODULEMODE;
        Register::BitField< 16,  2 > mIDLEST;
        Register::BitField< 18,  1 > mOPTFCLKEN_GPIO_1_GDBCLK;
    };

signals:

public slots:
    void Write( uint32_t id, uint32_t val );
    void Write( const std::string &name, uint32_t val );
    uint32_t Read( const std::string &name );
    uint32_t Read( uint32_t id );
    void SetGpioClock();

private:
    volatile uint32_t *mMem;
    RegisterList mRegisters;

    Register *FindRegister( uint32_t id );
    Register *FindRegister( const std::string &name );
    void Write( Register* reg, uint32_t val );
    uint32_t Read( Register* reg );
};

#endif // CLOCKMODULE_H
