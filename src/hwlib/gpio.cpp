//============================================================================
// Name        : ndr354hwlib/gpio.cpp
// Author      : David L Walsh
// Copyright   : G3 Technologies, Inc. 2014
// Description : General Purpose IO implementation
//
// The GPIO class provides support for the AM335x general purpose IO.
//
//============================================================================
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <sys/mman.h>

#include "hwlib/gpio.h"
#include <QFile>
#include <QDir>

#define GPIO0_BASE_ADDR     0x44E07000
#define GPIO1_BASE_ADDR     0x4804C000
#define GPIO2_BASE_ADDR     0x481AC000
#define GPIO3_BASE_ADDR     0x481AE000
#define GPIO_BASE_LEN       0x01000

bool GPIO::mVerbose = false;

GPIO::GPIO(QObject *parent) :
    QObject(parent)
{
    len=GPIO_BASE_LEN;
    for (int i=0;i<GPIO_BANKS;i++) {
        mem[i]=NULL;
        AddRegs(i);
    }
}

GPIO::~GPIO()
{
    for (int i=0;i<GPIO_BANKS;i++) {
        if (mem[i   ]) {
            if (simulated)
                free((void*)mem[i]);
            else
                munmap((void*)mem[i],len);
        }
    }
}

void GPIO::Init(int fd,bool simulated)
{
    this->simulated=simulated;
    for (int i=0;i<GPIO_BANKS;i++) {
        assert(mem[i]==NULL);
        unsigned int addr;
        if (i==0)
            addr=GPIO0_BASE_ADDR;
        else if (i==1)
            addr=GPIO1_BASE_ADDR;
        else if (i==2)
            addr=GPIO2_BASE_ADDR;
        else
            addr=GPIO3_BASE_ADDR;

        if (simulated)
            mem[i]=(volatile unsigned int *)malloc(len);
        else {
            mem[i]=(volatile unsigned int *)mmap(0,len,PROT_READ|PROT_WRITE,
                                                MAP_SHARED,fd,addr);
            // lets see if we have already exported a pin for the bank.
            // int pin=32*i+1;
            // // std::string filename;
            // // filename.sprintf("/sys/class/gpio/gpio%d",pin);
            // // QFile file(filename);
            // std::stringstream filename;
            // filename << "/sys/class/gpio/gpio%d" << pin;
            // if (!file.exists()) {
            //     // pin is not exported, so lets export it
            //     // this is necessary to activate the GPIO module in the kernel
            //     QFile exfile("/sys/class/gpio/export");
            //     if (exfile.open(QFile::WriteOnly)) {
            //         std::string data;
            //         data.sprintf("%d\n",pin);
            //         exfile.write(qPrintable(data));
            //         exfile.close();
            //     } else
            //         qWarning("Failed to open '%s' for writing.",qPrintable(exfile.fileName()));
            // }
        }
        Register::Init(regs[i],mem[i],len,NULL);
    }
}

void GPIO::InitScript(Script *script)
{
    script->AddGlobal( "gpio", this );
}

void GPIO::AddRegs(int i)
{
    regs[i].push_back(new Register(GPIO::REVISION,     0x0000,"Revision"));
    regs[i].push_back(new Register(GPIO::SYSCONFIG,    0x0010,"SysConfig"));
    regs[i].push_back(new Register(GPIO::SYSSTATUS,    0x0114,"SysStatus"));
    regs[i].push_back(new Register(GPIO::CTRL,         0x0130,"CTRL"));
    regs[i].push_back(new Register(GPIO::OE,           0x0134,"OE"));
    regs[i].push_back(new Register(GPIO::DATAIN,       0x0138,"DATAIN"));
    regs[i].push_back(new Register(GPIO::DATAOUT,      0x013C,"DATAOUT"));
    regs[i].push_back(new Register(GPIO::CLR_DATAOUT,  0x0190,"CLRDATAOUT"));
    regs[i].push_back(new Register(GPIO::SET_DATAOUT,  0x0194,"SETDATAOUT"));
}

uint32_t GPIO::Read(Register *reg)
{
    if (reg) {
        unsigned int val=reg->Read();
        if (mVerbose)
            printf("GPIO.Read: [%s(0x%04X)] -> 0x%08X\n",
                   reg->Name().c_str(),reg->Offset(),val);
        return val;
    }
    return 0xFFFFFFFF;
}

uint32_t GPIO::Read(int bank,int regId)
{
    return Read(FindRegister(bank,regId));
}

uint32_t GPIO::Read(int bank,const std::string &name)
{
    return Read(FindRegister(bank,name));
}

void GPIO::Write(Register *reg,uint32_t val)
{
    if (reg) {
        if (mVerbose)
            printf("GPIO.Write: [%s(0x%04X)] <- 0x%08X\n",
                   reg->Name().c_str(),reg->Offset(),val);
        reg->Write(val);
    }
}

void GPIO::Write(int bank, int regId, uint32_t val)
{
    return Write(FindRegister(bank,regId),val);
}

void GPIO::Write(int bank,const std::string &name, unsigned int val)
{
    return Write(FindRegister(bank,name),val);
}

void GPIO::Dump(RegisterList &list)
{
    for (int i=0;i<list.size();i++) {
        Register *reg=list.at(i);
        assert(reg);
        unsigned int val=Read(reg);
        printf("%24s: 0x%08X\n",reg->Name().c_str(),val);
    }
}

void GPIO::Dump()
{
    for (int i=0;i<GPIO_BANKS;i++)
        Dump(i);
}

void GPIO::Dump(int bank)
{
    printf("GPIO%d Registers\n",bank);
    Dump(regs[bank]);
}

Register *GPIO::FindRegister(int bank,int id)
{
    if (bank>=0 && bank<GPIO_BANKS)
        return Register::Find(id,regs[bank]);
    return NULL;
}

Register *GPIO::FindRegister(int bank,const std::string &name)
{
    if (bank>=0 && bank<GPIO_BANKS)
        return Register::Find(name,regs[bank]);
    return NULL;
}

bool GPIO::Get(int gpio)
{
    return Get( gpio/32, gpio%32 );
}

bool GPIO::Get(int bank, int pin)
{
    unsigned int val=Read(bank,GPIO::DATAIN);
    unsigned int mask=1<<pin;
    return (val&mask)?true:false;
}

void GPIO::Set(int gpio)
{
    Set( gpio/32, gpio%32 );
}

void GPIO::Set(int bank, int pin)
{
    unsigned int mask=1<<pin;
    Write(bank,GPIO::SET_DATAOUT,mask);
}

void GPIO::Clr(int gpio)
{
    Clr( gpio/32, gpio%32 );
}

void GPIO::Clr(int bank, int pin)
{
    unsigned int mask=1<<pin;
    Write(bank,GPIO::CLR_DATAOUT,mask);
}

void GPIO::Dir(int gpio, bool output)
{
    Dir( gpio/32, gpio%32, output );
}

void GPIO::Dir(int bank, int pin, bool output)
{
    unsigned int mask=1<<pin;
    unsigned int val=Read(bank,GPIO::OE);
    if (output)
        val&=~mask;
    else
        val|=mask;
    Write(bank,GPIO::OE,val);
}
