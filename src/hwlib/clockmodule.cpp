#include <sys/mman.h>

#include "clockmodule.h"

#define CM_BASE_ADDR     0x44E00000

#define CM_PER_OFFSET    0x00000000
#define CM_WKUP_OFFSET   0x00000400

#define CM_PER_LEN  0x400
#define CM_WKUP_LEN 0x100
#define CM_LEN      CM_PER_LEN + CM_WKUP_LEN

bool ClockModule::mVerbose = false;

ClockModule::ClockModule( QObject *parent )
    : QObject( parent )
    , mMem( NULL )
{
    AddRegs();
}

ClockModule::~ClockModule()
{
    // mRegisters.erase( mRegisters.begin(), mRegisters.end() );
    if( mMem ) {
        munmap( (void*)mMem, CM_LEN );
    }
}

void ClockModule::Init( int fd, bool simulated )
{
    if( simulated ) {
        mMem = (volatile unsigned int*)malloc( CM_LEN );
    } else {
        mMem = (volatile unsigned int*)mmap( 0
                                             , CM_LEN
                                             , PROT_READ | PROT_WRITE
                                             , MAP_SHARED
                                             , fd
                                             , CM_BASE_ADDR );
    }

    Register::Init( mRegisters, mMem, CM_LEN, NULL );

    // Active gpio clocks
    Write( CM_WKUP_GPIO0_CLKCTRL, 0x2 );
    Write( CM_PER_GPIO1_CLKCTRL, 0x2 );
    Write( CM_PER_GPIO2_CLKCTRL, 0x2 );
    Write( CM_PER_GPIO3_CLKCTRL, 0x2 );
}

void ClockModule::AddRegs()
{
    mRegisters.push_back( new Register( CM_PER_GPIO1_CLKCTRL    , CM_PER_OFFSET + 0xAC, "GPIO1_CLKCTRL" ) );
    mRegisters.push_back( new Register( CM_PER_GPIO2_CLKCTRL    , CM_PER_OFFSET + 0xB0, "GPIO2_CLKCTRL" ) );
    mRegisters.push_back( new Register( CM_PER_GPIO3_CLKCTRL    , CM_PER_OFFSET + 0xB4, "GPIO3_CLKCTRL" ) );
    mRegisters.push_back( new Register( CM_WKUP_CLKSTCTRL       , CM_WKUP_OFFSET + 0x00, "CLKSTCTRL") );
    mRegisters.push_back( new Register( CM_WKUP_CONTROL_CLKCTRL , CM_WKUP_OFFSET + 0x04, "CONTROL_CLKCTRL" ) );
    mRegisters.push_back( new Register( CM_WKUP_GPIO0_CLKCTRL   , CM_WKUP_OFFSET + 0x08, "CPIO0_CLKCTRL" ) );

}

Register* ClockModule::FindRegister( uint32_t id )
{
    return Register::Find( id, mRegisters );
}

Register* ClockModule::FindRegister( const std::string &name )
{
    return Register::Find( name, mRegisters );
}

void ClockModule::Write(Register *reg, uint32_t val)
{
    if( reg ) {
        if( mVerbose ) {
            printf( "ClockModule.Write: [%s(0x%04X)] <- 0x%08X\n",
                   reg->Name().c_str(), reg->Offset(), val );
        }
        reg->Write( val );
    }
}

void ClockModule::Write( uint32_t id, uint32_t val )
{
    Write( FindRegister( id ), val );
}

void ClockModule::Write(const std::string &name, uint32_t val )
{
    Write( FindRegister( name ), val );
}

uint32_t ClockModule::Read( Register *reg )
{
    if( reg ) {
        unsigned int val = reg->Read();
        if( mVerbose) {
            printf( "ClockModule.Read: [%s(0x%04X)] -> 0x%08X\n",
                   reg->Name().c_str(), reg->Offset(), val );
        }
        return val;
    }
    return 0xFFFFFFFF;
}

uint32_t ClockModule::Read( uint32_t id )
{
    return Read( FindRegister( id ) );
}

uint32_t ClockModule::Read( const std::string &name )
{
    return Read( FindRegister( name ) );
}

void ClockModule::SetGpioClock()
{
    Write( CM_PER_GPIO1_CLKCTRL, 0x2 );
}
