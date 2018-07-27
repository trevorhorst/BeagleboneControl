#ifndef SSD1306_H
#define SSD1306_H

#include <QObject>

#include <string>
#include <cstdint>
//#include "../common/common.h"

#define WIDTH     128          // 128 columns of 8bits deep
#define HEIGHT    8            // 4 rows, 8 bits high
#define TILE_SIZE 8
#define STEP_SIZE 16

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

public slots:
    void ClearBuffer();
    void ClearScreen();
    bool WriteScreen();
    bool WriteCenter( QString text, uint8_t row );
    bool writeText (std::string text, int8_t row, int8_t col);
    bool writeImage (int8_t* image);

    bool Write( QString text );

private:
    bool init ();
    int8_t reverseByte (int8_t b);
    void getTileFromBitmap (int index);
    void setTileInBuffer(int index);
    void rotateTile();

    int8_t *mScreenBuffer;
    int8_t *mData;
    int8_t *tmp_;
    I2C *mI2C;
};

#endif /* SSD1306_H */
