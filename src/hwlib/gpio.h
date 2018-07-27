//============================================================================
// Name        : ndr354hwlib/gpio.h
// Author      : David L Walsh
// Copyright   : G3 Technologies, Inc. 2014
// Description : General Purpose IO implementation
//============================================================================
#ifndef GPIO_H
#define GPIO_H

#include <QObject>

#include <string>
#include <linux/types.h>

#include "common/script.h"

#include "register.h"

#define GPIO_BANKS 4

class GPIO : public QObject
{
    Q_OBJECT
public:
    explicit GPIO( QObject *parent = 0 );
    ~GPIO();
    static bool mVerbose;
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose ) { mVerbose = verbose; }

    void Init(int fd, bool simulated);

    enum RegisterID {
        REVISION=0,
        SYSCONFIG,
        IRQSTATUS_RAW0,
        IRQSTATUS_RAW1,
        IRQSTATUS0,
        IRQSTATUS1,
        IRQSTATUS_SET0,
        IRQSTATUS_SET1,
        IRQSTATUS_CLR0,
        IRQSTATUS_CLR1,
        IRQWAKEN0,
        IRQWAKEN1,
        SYSSTATUS,
        CTRL,
        OE,
        DATAIN,
        DATAOUT,
        LEVELDETECT0,
        LEVELDETECT1,
        RISINGDETECT,
        FALLINGDETECT,
        DEBOUNCE_ENABLE,
        DEBOUNCING_TIME,
        CLR_DATAOUT,
        SET_DATAOUT
    };
signals:
public slots:
    void Dump();
    void Dump(int bank);
    uint32_t Read(int bank,const std::string &name);
    uint32_t Read(int bank,int regId);
    void Write(int bank,int regId,uint32_t val);
    void Write(int bank,const std::string &name,unsigned int val);
    bool Get(int gpio);
    bool Get(int bank,int pin);
    void Set(int gpio);
    void Set(int bank,int pin);
    void Clr(int gpio);
    void Clr(int bank,int pin);
    void Dir(int gpio, bool output);
    void Dir(int bank,int pin,bool output);

protected:
    uint32_t Read(Register *reg);
    void Write(Register *reg, uint32_t val);
    Register *FindRegister(int bank,int id);
    Register *FindRegister(int bank,const std::string &name);

    void Dump(RegisterList &list);
    void AddRegs(int bank);
    RegisterList regs[GPIO_BANKS];
    volatile uint32_t *mem[GPIO_BANKS];
    size_t len;
    bool mSimulated;
};

#endif // GPIO_H
