/*
Author: Mike White (mike808@gmail.com; @mikewhite808)

Implementation for using a 128x64 OLED display from Adafruit.
https://www.adafruit.com/products/938

Code can easily be modified to work with a 128x32 OLED display from Adafruit.
Just modify the HEIGHT to be 4 and that should be it. Of course I have not
tested it as I do not own a 128x32 OLED.
https://www.adafruit.com/products/931

*/

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

#define _GLCDFONT
#include "ssd1306.h"
#include "common/glcdfont.h"
#include "common/hi_logo.h"
#include "i2c.h"

using namespace std;

bool SSD1306::mVerbose = false;

/** ****************************************************************************
 * @brief Constructor
 * ****************************************************************************/
SSD1306::SSD1306()
    : QObject( NULL )
    , mScreenBuffer( nullptr )
    , mData( nullptr )
    , tmp_( nullptr )
    , mI2C( nullptr )
{
   mI2C = new I2C( 0x3C, 0x00 );
   init ();
}

/** ****************************************************************************
 * @brief Destructor
 * ****************************************************************************/
SSD1306::~SSD1306()
{
    if( mScreenBuffer ) {
        delete[] mScreenBuffer;
        mScreenBuffer = 0;
    }

    if( mI2C ) {
        delete mI2C;
        mI2C = 0;
    }
}

bool SSD1306::Write( QString text )
{
    ClearScreen();
    WriteCenter( text, 4 );
    WriteScreen();
}

/** ****************************************************************************
 * @brief Clears the SSD1306's internal memory buffer
 * ****************************************************************************/
void SSD1306::ClearBuffer ()
{
    if( mScreenBuffer ) {
        memset( mScreenBuffer, 0x00, ( WIDTH * HEIGHT ) );
    }
}

/** ****************************************************************************
 * @brief Clears the SSD1306's screen
 * ****************************************************************************/
void SSD1306::ClearScreen()
{
    // Clear internal buffer
    ClearBuffer();
    // Write cleared buffer to the screen
    WriteScreen();
}

/** ****************************************************************************
 * @brief Writes the screen buffer to the SSD1306
 * @return Boolean indicating success of the operation
 * ****************************************************************************/
bool SSD1306::WriteScreen()
{
    bool fOK;
    int16_t uiIndex = 0x00;
    int8_t uchTemp[ 0x09 ];

    // Need to set OLED screen pointer back to 0, 0 before writing
    // This was the only way I found to do this with the chip set commands.
    fOK = mI2C->writeCommand( 3, 0x21, 0x00, 0x7F ); // Set column address; start 0, end 127
    fOK = mI2C->writeCommand( 3, 0x22, 0x00, 0x07 ); // Set row address; start 0, end 7

    while( uiIndex < ( WIDTH * HEIGHT ) ) {
       uchTemp[ 0 ] = 0x40;           // Co = 0; D/C# = 1 for data
       for( int i = 0; i < 8; i++ ) {  // Copy 8 bytes to temp buffer
          uchTemp[ i + 1 ] = mScreenBuffer[ uiIndex + i ];
       }
       fOK = mI2C->writeBytes( uchTemp, 0x09 ); //Write control byte with 8 data bytes
       uiIndex += 0x08;             // Move to next Octet
    }

    return fOK;
}

/************************************************************
 * Writes text that is centered for the specified row
 *
 * Parameters:
 *   text - the string to write
 *   row  - the row to write the text to
 *
 * Returns false if the row is out of range; true for success
 ************************************************************/
bool SSD1306::WriteCenter( QString text, uint8_t row )
{
    if( ( text.length() * 6 ) < 127 ) { // If there is not more than 1 space left, don't center
       // To center, multiply # of characters * 6 (5+space), subtract the last space, then divide by 2
       return writeText ( qPrintable( text ), row, (128 - ((text.length()*6)-1))/2);
    } else {
       return writeText ( qPrintable( text ), row, 0);
    }
}

/************************************************************
 * Writes the specified text to the OLED display
 *
 * Parameters:
 *   text - the text to write to the display
 *   row  - the row to write text to
 *   col  - the colum to start writing text on
 *
 * Returns false if the row/column is out of range; returns true otherwise
 ************************************************************/
bool SSD1306::writeText (string text, int8_t row, int8_t col) //
{
   int8_t uchI, uchJ;
   int16_t uiIndex;     // Indexes in the screen buffer

   uiIndex = (row * WIDTH) + col;             // Set absolute start position in buffer

   if ((row >= HEIGHT) || (col >= WIDTH))     // If asked for something out of range, abandon
      return false;

   for (uchI = 0; uchI < text.length(); uchI++)  // Loop through each character in string
   {
      for (uchJ = 0; uchJ < 5; uchJ++)             // Loop through each 7bit segment of the character
      {
         mScreenBuffer[uiIndex] = font[(text[uchI]*5)+uchJ]; // Trial and error!
         uiIndex ++;                              // Move to next buffer position
      }
      mScreenBuffer[uiIndex] = 0x00;                  // This puts a space between the characters on screen
      uiIndex++;
   }
   return true;
}

/************************************************************
 * Initializes the OLED display
 *
 * Returns true for success; false otherwise
 ************************************************************/
bool SSD1306::init ()
{
   bool retval = false;

   mScreenBuffer = new int8_t[WIDTH * HEIGHT]; // Create a block of memory for the screen buffer
   memset(mScreenBuffer, 0, (WIDTH * HEIGHT));
   if (!mScreenBuffer)
   {
#ifdef DEBUG
      cerr << "could not create screenBuf buffer" << endl;
#endif
      return retval;
   }

   mData = new int8_t[TILE_SIZE];
   memset(mData, 0, TILE_SIZE);
   if (!mData)
   {
#ifdef DEBUG
      cerr << "could not create data buffer" << endl;
#endif
   }

   tmp_ = new int8_t[TILE_SIZE];
   memset(tmp_, 0, TILE_SIZE);
   if (!tmp_)
   {
#ifdef DEBUG
      cerr << "could not create tmp buffer" << endl;
#endif
   }

   memcpy( mScreenBuffer, hi_logo, sizeof( hi_logo ) );

   //write command to the screen registers.
   // retval &= i2c_->writeCommand(1,0xAE);//display off
   // retval &= i2c_->writeCommand(1,0x00);//Set Memory Addressing Mode
   // retval &= i2c_->writeCommand(1,0x10);//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
   // retval &= i2c_->writeCommand(1,0x40);//Set Page Start Address for Page Addressing Mode,0-7
   // retval &= i2c_->writeCommand(1,0xB0);//Set COM Output Scan Direction
   // retval &= i2c_->writeCommand(1,0x81);//---set low column address
   // retval &= i2c_->writeCommand(1,0xCF);//---set high column address
   // retval &= i2c_->writeCommand(1,0xA1);//--set start line address
   // retval &= i2c_->writeCommand(1,0xA6);//--set contrast control register
   // retval &= i2c_->writeCommand(1,0xA8);
   // retval &= i2c_->writeCommand(1,0x3F);//--set segment re-map 0 to 127
   // retval &= i2c_->writeCommand(1,0xC8);//--set normal display
   // retval &= i2c_->writeCommand(1,0xD3);//--set multiplex ratio(1 to 64)
   // retval &= i2c_->writeCommand(1,0x00);//
   // retval &= i2c_->writeCommand(1,0xD5);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
   // retval &= i2c_->writeCommand(1,0x80);//-set display offset
   // retval &= i2c_->writeCommand(1,0xD9);//-not offset
   // retval &= i2c_->writeCommand(1,0xF1);//--set display clock divide ratio/oscillator frequency
   // retval &= i2c_->writeCommand(1,0xDA);//--set divide ratio
   // retval &= i2c_->writeCommand(1,0x12);//--set pre-charge period
   // retval &= i2c_->writeCommand(1,0xDB);//
   // retval &= i2c_->writeCommand(1,0x40);//--set com pins hardware configuration
   // retval &= i2c_->writeCommand(1,0x8D);//--set vcomh
   // retval &= i2c_->writeCommand(1,0x14);//0x20,0.77xVcc
   // retval &= i2c_->writeCommand(1,0xAF);//--set DC-DC enable
   // retval &= i2c_->writeCommand(1,0xAF);//--turn on oled panel


   retval &= mI2C->writeCommand( 1, 0xAE );             // Display off
   retval &= mI2C->writeCommand( 2, 0xD5, 0x80 );       // set display clock division
   retval &= mI2C->writeCommand( 2, 0xA8, 0x3F );       // set multiplex
   retval &= mI2C->writeCommand( 2, 0xD3, 0x00 );       // set display offset
   retval &= mI2C->writeCommand( 1, 0x40 );             // set start line #0
   retval &= mI2C->writeCommand( 2, 0x8D, 0x14 );       // set charge pump
   retval &= mI2C->writeCommand( 2, 0x20, 0x00 );       // Memory mode
   retval &= mI2C->writeCommand( 1, 0xA1 );             // Segremap(0xA0 = reset, 0xA1 = 127 = SEG0)
   retval &= mI2C->writeCommand( 1, 0xC8 );             // Com scan dec (0xC0 = reset normal, 0xC8 = scan  from Com[n-1] - Com 0
   retval &= mI2C->writeCommand( 2, 0xDA, 0x12 );       // Set com pins
   retval &= mI2C->writeCommand( 2, 0x81, 0xCF );       // Set contrast
   retval &= mI2C->writeCommand( 2, 0xD9, 0xF1 );       // Set precharge
   retval &= mI2C->writeCommand( 2, 0xDB, 0x40 );       // Set Vcom select
   retval &= mI2C->writeCommand( 1, 0xA4 );             // Resume RAM content display
   retval &= mI2C->writeCommand( 1, 0xA6 );             // Normal display not inverted
   retval &= mI2C->writeCommand( 1, 0x00 );             // low col=0
   retval &= mI2C->writeCommand( 1, 0x10 );             // high col=0
   retval &= mI2C->writeCommand( 1, 0x40 );             // line #0
   retval &= mI2C->writeCommand( 1, 0xAF );             // Display ON
   retval &= mI2C->writeCommand( 3, 0x21, 0x00, 0x7F ); // Set column address; start 0, end 127
   retval &= mI2C->writeCommand( 3, 0x22, 0x00, 0x07 ); // Set row address; start 0, end 7
   retval &= mI2C->writeCommand( 1, 0xAF );             // Display ON

   mI2C->writeCommand (3, 0x21, 0x00, 0x7F); // Set column address; start 0, end 127
   mI2C->writeCommand (3, 0x22, 0x00, 0x07); // Set row address; start 0, end 7

   ClearScreen();
   // int16_t uiIndex = 0x00;
   // int8_t uchTemp[0x09];

   // while (uiIndex < (1024))
   // {
   //    uchTemp[0] = 0x40;           // Co = 0; D/C# = 1 for data
   //    for (int i = 0; i < 8; i++)  // Copy 8 bytes to temp buffer
   //       uchTemp[i+1] = screenBuf_[uiIndex+i];
   //    i2c_->writeBytes( uchTemp, 0x09 );
   //    // i2c.writeBytes (uchTemp, 0x09); //Write control byte with 8 data bytes
   //    uiIndex += 0x08;             // Move to next Octet
   // }


   return retval;
}

/************************************************************
 * Writes a image to the OLED display. The image can be made
 * with LCDAssistant.exe. When saving the output, use the vertical
 * mode to save the output. Currently, only 128x64 is supported.
 *
 * Parameters:
 *   image - the image to write
 *
 * Returns false
 ************************************************************/
bool SSD1306::writeImage (int8_t* image)
{
   for (int lcv=0; lcv<(WIDTH * HEIGHT); lcv++)
   {
      getTileFromBitmap(lcv);

      // rotate the 8x8 tile
      rotateTile();

#ifdef DEBUG
      cout << "==============temp buffer===========" << endl;
#endif
      // reverse the bites in the byte
      for (int i=0; i<TILE_SIZE; i++)
      {
         tmp_[i] = reverseByte(tmp_[i]);
#ifdef DEBUG
         cout << hex << uppercase << "0x" << (int)tmp_[i] << endl;
#endif
      }

      setTileInBuffer(lcv);

      // FIXME: we can probably skip to the next octect instead of
      //        doing this for every byte!
   }
   return false;
}

/************************************************************
 * Reverses the bits for a byte
 *
 * Parameters:
 *   b - the byte to reverse the bits for
 *
 * Returns the byte with the bits reversed
 ************************************************************/
int8_t SSD1306::reverseByte (int8_t b)
{
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

/************************************************************
 * Retrieves the 8 bytes that represent a tile for bitmap
 *
 * Parameters:
 *   index - the index of the byte
 ************************************************************/
void SSD1306::getTileFromBitmap (int index)
{
   int step = 0;
   for (int i=0; i<TILE_SIZE; i++, step+=STEP_SIZE)
   {
      mData[i] = mScreenBuffer[index+step];
#if DEBUG
      cout << "getting byte at index: " << dec << index+step << " with value: "
         << hex << uppercase << "0x" << (int)screenBuf_[index+step] << endl;
#endif
   }
}

/************************************************************
 * Writes the rotated buffer to the screen buffer
 *
 * Parameters:
 *   index - the index of the tile we are working with
 ************************************************************/
void SSD1306::setTileInBuffer(int index)
{
   int step = 0;
   for (int i=0; i<TILE_SIZE; i++, step+=STEP_SIZE)
   {
      mScreenBuffer[index+step] |= tmp_[i];
#ifdef DEBUG
      cout << "setting byte at index: " << dec << index+step << " with value: "
         << hex << uppercase << "0x" << (int)tmp_[i] << endl;
#endif
   }
}

/************************************************************
 * Rotates the "tile" which is the set of 8 bytes from being
 * horizontal to being vertical
 ************************************************************/
void SSD1306::rotateTile()
{
   // process bits for a bytes in data
   for (int i=0; i<TILE_SIZE; i++)
   {
#ifdef DEBUG
      cout << " process bit " << dec << i << endl;
#endif

      // process data bytes
      for (int j=0; j<TILE_SIZE; j++)
      {
#ifdef DEBUG
         cout << " anding data 0x" << hex << uppercase << (int)mData[j]
            << " with 2 to the " << 7-i << endl;
#endif
         int8_t temp = mData[j] & (int8_t)pow(2, 7-i);
#ifdef DEBUG
         cout << "\ttemp before shift: " << hex << uppercase << "0x" << (int)temp << endl;
#endif
         int shift = 7-i-j;
         int8_t shifted = 0x0;
         if (shift < 0)
         {
            shift *= -1;
#ifdef DEBUG
            cout << "\tshifting temp to the LEFT " << dec << shift << endl;
#endif
            shifted = temp << shift;
         }
         else
         {
#ifdef DEBUG
            cout << "\tshifting temp to the RIGHT " << dec << shift << endl;
#endif
            shifted = temp >> shift;
         }
#ifdef DEBUG
         cout << "\tshifted temp value: " << hex << uppercase << "0x" << (int)shifted << endl;
         cout << "\ttmp[" << dec << i << "] beofre OR: " << hex << uppercase << (int)tmp_[i] << endl;
#endif
         tmp_[i] |= shifted;
#ifdef DEBUG
         cout << "\ttmp[" << dec << i << "] after OR: " << hex << uppercase << "0x" << (int)tmp_[i] << endl;
#endif
      }
      tmp_[i] = reverseByte(tmp_[i]);
   }
}
