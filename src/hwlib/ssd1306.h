#ifndef SSD1306_H
#define SSD1306_H

#include <QObject>

#include <string>
#include <cstdint>
//#include "../common/common.h"
#define UINT8 int8_t
#define UINT16 int16_t

#define WIDTH     128          // 128 columns of 8bits deep
#define HEIGHT    8            // 4 rows, 8 bits high
#define TILE_SIZE 8
#define STEP_SIZE 16

#include "common/script.h"
#include "common/debug.h"

/*
   Class OLED will drive an Adafruit 128x64 I2C OLED Display
   on a Raspberry Pi using i2c port defined above.
*/

class I2C;

class SSD1306 : public QObject
{
    Q_OBJECT
public:
    explicit SSD1306();
    ~SSD1306 ();

    static bool mVerbose;
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose ){ mVerbose = verbose; }

    void InitScript( Script *script );

public slots:
    void clearBuffer ();
    void ClearScreen();
    bool writeScreen ();
    bool WriteCenter( QString text, uint8_t row );
    bool writeText (std::string text, UINT8 row, UINT8 col);
    bool writeImage (UINT8* image);

    bool Write( QString text );

private:
    bool init ();
    UINT8 reverseByte (UINT8 b);
    void getTileFromBitmap (int index);
    void setTileInBuffer(int index);
    void rotateTile();

    UINT8 *screenBuf_;
    UINT8 *data_;
    UINT8 *tmp_;
    I2C *i2c_;
};

#endif /* SSD1306_H */
