//============================================================================
// Name        : ndr354hwlib/register.h
// Author      : David L Walsh
// Copyright   : G3 Technologies, Inc. 2014
// Description : Register header
//============================================================================
#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h>
#include <type_traits>

#include <QObject>
#include <QList>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>


class Register;
typedef std::vector<Register*>  RegisterList;

class Register : public QObject
{
    Q_OBJECT
public:
    enum WIDTH {
        W8=0,   // 8-bit
        W16,    // 16-bit
        W32,    // 32-bit
        W48,    // 48-bit
        W64     // 64-bit
    };

    explicit Register(int id,uint32_t offset,
                      const char *name,WIDTH w=W32,
                      QObject *parent = 0);
    void Init(volatile uint32_t *mem,size_t len,bool *verbose);
    const std::string &Name() {return name; }
    int Id() { return id; }
    uint32_t Offset() { return offset; }
    uint32_t Read();
    uint16_t ReadLow16();
    uint16_t ReadHigh16();
    uint16_t Read16();
    uint16_t Read16(uint32_t off);    // read word @ address offset.  Read16(2) would be the same as ReadHigh16()


    void ReadModifyWrite(uint32_t val,uint32_t mask);
    void ReadModifyWrite16(uint16_t val,uint16_t mask);
    void ReadModifyWrite16(uint32_t off,uint16_t val,uint16_t mask);

    void Write(uint32_t val);
    void WriteHigh16(uint16_t val);
    void WriteLow16(uint16_t val);
    void Write16(uint16_t val);
    void Write16(uint32_t off,uint16_t val); // write word @ address offset.  Write16(2,val) would be the same as WriteHigh16(val)
    WIDTH Width() const { return width; }
    std::string Dump(bool displayZero=true);
    static Register *Find(uint32_t id, RegisterList &list);
    static Register *Find(const std::string &name,RegisterList &list);
    static Register *Find(int start,int stop,const std::string &name,RegisterList &list);
    static void Init(RegisterList &list,
                     volatile uint32_t *mem,
                     size_t len,bool *verbose);
    static std::string Dump(RegisterList &list, int start, int stop);
    static uint32_t RawRead32(volatile uint32_t *mem,size_t memLen,
                             uint32_t offset,bool *verbose=NULL);
    static uint16_t RawRead16(volatile uint32_t *mem,size_t memLen,
                             uint32_t offset,bool *verbose=NULL);
    static void RawWrite32(volatile uint32_t *mem,size_t memLen,
                           uint32_t offset,uint32_t val,bool *verbose=NULL);
    static void RawWrite16(volatile uint32_t *mem,size_t memLen,
                           uint32_t offset,uint16_t val,bool *verbose=NULL);
    static uint32_t Read(const std::string &name,RegisterList &list);
    static void Write(const std::string &name,uint32_t val,RegisterList &list);

    template <size_t LastBit>
    struct MinimumTypeHelper {
        typedef
            typename std::conditional<LastBit == 0 , void,
            typename std::conditional<LastBit <= 8 , uint8_t,
            typename std::conditional<LastBit <= 16, uint16_t,
            typename std::conditional<LastBit <= 32, uint32_t,
            typename std::conditional<LastBit <= 64, uint64_t,
            void>::type>::type>::type>::type>::type type;
    };

    template <size_t Index, size_t Bits = 1>
    class BitField {
    private:
        enum {
            Mask = (1u << Bits) - 1u
        };

        typedef typename MinimumTypeHelper<Index + Bits>::type T;
    public:
        template <class T2>
        BitField &operator=(T2 value) {
            value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
            return *this;
        }

        operator T() const             { return (value_ >> Index) & Mask; }
        explicit operator bool() const { return value_ & (Mask << Index); }
        BitField &operator++()         { return *this = *this + 1; }
        T operator++(int)              { T r = *this; ++*this; return r; }
        BitField &operator--()         { return *this = *this - 1; }
        T operator--(int)              { T r = *this; --*this; return r; }

    private:
        T value_;
    };


signals:
    
public slots:
protected:
    int id;
    uint32_t offset;
    std::string name;
    WIDTH width;
    volatile uint32_t *mem;
    size_t memLen;
    bool *verbose;
};


#endif // REGISTER_H
