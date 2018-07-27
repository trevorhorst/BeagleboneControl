//============================================================================
// Name        : ndr354hwlib/register.cpp
// Author      : David L Walsh
// Copyright   : G3 Technologies, Inc. 2014
// Description : Register implementation
//
// The Register class provides generalized support for registers.
//
//============================================================================
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "hwlib/register.h"

Register::Register(int id, uint32_t offset, const char *name, WIDTH w, QObject *parent) :
    QObject(parent)
{
    this->id=id;
    this->offset=offset;
    this->name=name;
    this->mem=NULL;
    this->memLen=0;
    this->width=w;
    this->verbose=NULL;
}

void Register::Init(volatile uint32_t *mem, size_t len, bool *verbose)
{
    this->mem=mem;
    this->memLen=len;
    this->verbose=verbose;
}

std::string Register::Dump(bool displayZero)
{
    std::string output;
    if (Width()==W16) {
        uint16_t val=Read16();
        if( val != 0 || displayZero ) {
            // output = name + "(" + std::string(int(offset)) + "):"
            //         + std::string(int(val));
            // output.sprintf("%24s(0x%04x): 0x%04X\n",
            //                qPrintable(name),offset,val);
        }
    } else {
        uint32_t val=Read();
        if( val != 0 || displayZero ) {
            // output = name + "(" + std::string(int(offset)) + "):"
            //         + std::string(int(val));
            // output.sprintf("%24s(0x%04x): 0x%08X\n",
            //                qPrintable(name),offset,val);
        }
    }
    return output;
}

std::string Register::Dump(RegisterList &list, int start, int stop)
{
    std::string output;
    int first=start;
    if (first<0) first=0;
    int last=stop;
    if (last<0)
        last=list.size()-1;
    for (int i=first;i<=last;i++) {
        Register *reg=list.at(i);
        assert(reg);
        output+=reg->Dump();
    }
    return output;
}

Register *Register::Find( uint32_t id,RegisterList &list )
{
    // for registers that the id matches
    // the index in the list, then we can lookup
    // the register quickly
    if( id < list.size() ) {
        Register *reg = list.at( id );
        if( reg && reg->Id() == id ) {
            return reg;
        }
    }
    // okay using the offset into the array didn't work
    // so now we have to do a search
    for( uint32_t i = 0; i < list.size(); i++ ) {
        Register *reg = list.at( i );
        if( reg && reg->Id() == id ) {
            return reg;
        }
    }
    qWarning( "Register ID = %d not found. count = %d", id, (int)list.size() );
    return NULL;
}

Register *Register::Find(const std::string &name,RegisterList &list)
{
    return Find(0,-1,name,list);
}

Register *Register::Find(int start,int stop,const std::string &name,RegisterList &list)
{
    int first=start;
    if (first<0) first=0;
    int last=stop;
    if (last<0) last=list.size()-1;

    for (int i=first;i<=last;i++) {
        Register *reg=list.at(i);
        //if (reg && reg->Name().compare(name,Qt::CaseInsensitive)==0)
        if (reg && reg->Name().compare(name)==0)
            return reg;
    }
    std::cout << "Register '%s' not found." << name << std::endl;
    return NULL;
}

void Register::Init(RegisterList &list, volatile uint32_t *mem, size_t len, bool *verbose)
{
    for( uint32_t i = 0; i < list.size(); i++ ) {
        Register *reg=list.at(i);
        if (reg)
            reg->Init(mem,len,verbose);
//        if (reg->Id()!=i)
//            qWarning("Register('%s') #%d @ 0x%04X should be #%d",
//                     qPrintable(reg->Name()),reg->Id(),reg->Offset(),i);
    }
}

uint32_t Register::Read()
{
    if (mem) {
        if (width==W32)
            return Register::RawRead32(mem,memLen,offset,verbose);
        else
            return (uint32_t)Register::RawRead16(mem,memLen,offset,verbose);
    } else
        qWarning("Read Invalid memory\n");
    return 0;
}

uint16_t Register::Read16(uint32_t off)
{
    if (mem)
        return Register::RawRead16(mem,memLen,offset+off,verbose);
    else
        qWarning("Read Invalid memory\n");
    return 0;
}

uint16_t Register::Read16()
{
    return Read16(0);
}

uint16_t Register::ReadLow16()
{
    return Read16(0);
}

uint16_t Register::ReadHigh16()
{
    return Read16(2);
}

uint32_t Register::RawRead32(volatile uint32_t *mem, size_t memLen,
                            uint32_t offset,bool *verbose)
{
    assert(mem);
    if (offset<memLen) {
        int index=offset/sizeof(unsigned int);
        assert(offset<memLen);
        uint32_t val=mem[index];
        if (verbose && *verbose)
            qDebug("RawRead32: [0x%04X] -> 0x%08X",
                   offset,val);
        return val;
    }
    return 0xFFFFFFFF;
}

uint16_t Register::RawRead16(volatile uint32_t *mem,size_t memLen,
                            uint32_t offset,bool *verbose)
{
    assert(mem);
    if (offset<memLen) {
        volatile uint16_t *mem16=(uint16_t *)mem;
        int index=offset/sizeof(uint16_t);
        assert(offset<memLen);
        uint16_t val=mem16[index];
        if (verbose && *verbose)
            qDebug("RawRead16: [0x%04X] -> 0x%04X",
                   offset,val);
        return val;
    }
    return 0xFFFF;
}

void Register::Write16(uint32_t off,uint16_t val)
{
    if (mem) {
        RawWrite16(mem,memLen,offset+off,val,verbose);
    } else
        qWarning("Write Invalid memory");
}

void Register::Write(uint32_t val)
{
    if (mem) {
        if (width==W32)
            RawWrite32(mem,memLen,offset,val,verbose);
        else
            RawWrite16(mem,memLen,offset,val,verbose);
    } else
        qWarning("Write Invalid memory\n");
}

void Register::Write16(uint16_t val)
{
    Write16(0,val);
}

void Register::WriteLow16(uint16_t val)
{
    Write16(0,val);
}

void Register::WriteHigh16(uint16_t val)
{
    Write16(2,val);
}

void Register::RawWrite32(volatile uint32_t *mem, size_t memLen,
                          uint32_t offset,uint32_t val,bool *verbose)
{
    assert(mem);
    if (offset<memLen) {
        int index=offset/sizeof(uint32_t);
        if (verbose && *verbose)
            qDebug("RawWrite32: 0x%08X -> [0x%04X]",
                   val,offset);
        mem[index]=val;
    }
}

void Register::RawWrite16(volatile uint32_t *mem, size_t memLen,
                          uint32_t offset, uint16_t val,bool *verbose)
{
    assert(mem);
    if (offset<memLen) {
        volatile uint16_t *mem16=(uint16_t *)mem;
        int index=offset/sizeof(unsigned short);
        assert(offset<memLen);
        if (verbose && *verbose)
            qDebug("RawWrite16: 0x%04X -> [0x%04X]",
                   val,offset);
        mem16[index]=val;
    }
}

uint32_t Register::Read(const std::string &name, RegisterList &list)
{
    Register *reg=Register::Find(name,list);
    if (reg)
        return reg->Read();
    return 0xFFFFFFFF;
}

void Register::Write(const std::string &name, uint32_t val, RegisterList &list)
{
    Register *reg=Register::Find(name,list);
    if (reg)
        reg->Write(val);
}

void Register::ReadModifyWrite(uint32_t val, uint32_t mask)
{
    uint32_t d=val & mask;
    d|=(Read() & ~mask);
    Write(d);
}

void Register::ReadModifyWrite16(uint16_t val, uint16_t mask)
{
    uint16_t d=val & mask;
    d|=(Read16() & ~mask);
    Write16(d);
}

void Register::ReadModifyWrite16(uint32_t off, uint16_t val, uint16_t mask)
{
    uint16_t d=val & mask;
    d|=(Read16(off) & ~mask);
    Write16(off,d);
}
