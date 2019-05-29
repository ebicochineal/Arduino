#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// uno       G   V   8   9  10  11  13
// ST7735    G   V  DC RES  CS SDA SCL

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8
#define TFT_BACKLIGHT  7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

uint16_t ebi[] = {0b0000000000000000,0b0000000000000000,0b0000000000000000,0b0000000000000000,0b0000000000000000,0b0000000000000000,0b0000000000000000,0b0000000000000000,0b0000000111100000,0b0000011000011000,0b0000100000000000,0b0111111111110000,0b0001101111111100,0b0000011110111110,0b0000100101010110,0b0001001010101100,0b0001001000011000};

void setup(void) {
    
    tft.initR(INITR_MINI160x80);
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);
    
    tft.invertDisplay(true);
    tft.fillScreen(ST77XX_BLACK);
    delay(100);
    
    // display noise clear
    tft.setRotation(0);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(2);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(0);
    tft.fillScreen(ST77XX_BLACK);
    
}
uint16_t Color565 (uint8_t r, uint8_t g, uint8_t b) {
    return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
}

int Color64(int n) {
    if (n < 16) { return n; }
    if (n < 48) { return 16; }
    return 63 - n;
}

uint32_t xrnd () {
    static uint32_t y = 2463534242;
    y = y ^ (y << 13);
    y = y ^ (y >> 17);
    return y = y ^ (y << 5);
}

uint16_t nColor (int n) {
    n = n % 96;
    int r = 0;
    int g = 0;
    int b = 0;
    if (n < 64) {
        int p = n;
        r = min(Color64(p) * 8, 127);
    }
    if (32 <= n && n < 96) {
        int p = n - 32;
        g = min(Color64(p) * 8, 127);
    }
    if (n < 32 || n >= 64) {
        int p = n - 64;
        if (n < 32) { p = 32 + n; }
        b = min(Color64(p) * 8, 127);
    }
    
    return Color565(r, g, b);
}

int ncnt = 0;
void loop() {
    int cnt = ncnt;
    if (xrnd()%2) {
        int t = xrnd()%5;
        for (int py = 0; py < 10; ++py) {
            for (int px = 0; px < 1; ++px) {
                for (int x = 0; x < 16; ++x) {
                    for (int y = 0; y < 16; ++y) {
                        int mx = (px+t)*16 + x;
                        int my = py*16 + y;
                        if (ebi[y] >> (16-x) & 0b1) {
                            tft.drawPixel(mx, my, nColor(cnt + ncnt));
                        } else {
                            tft.drawPixel(mx, my, Color565(0, 0, 0));
                        }
                    }
                }
                cnt += 1;
            }
        }
        delay(160);
        for (int py = 0; py < 10; ++py) {
            for (int px = 0; px < 1; ++px) {
                for (int x = 0; x < 16; ++x) {
                    for (int y = 0; y < 16; ++y) {
                        int mx = (px+t)*16 + x;
                        int my = py*16 + y;
                        tft.drawPixel(mx, my, Color565(0, 0, 0));
                    }
                }
            }
        }
    } else {
        int t = xrnd()%10;
        for (int py = 0; py < 1; ++py) {
            for (int px = 0; px < 5; ++px) {
                for (int x = 0; x < 16; ++x) {
                    for (int y = 0; y < 16; ++y) {
                        int mx = px*16 + x;
                        int my = (py+t)*16 + y;
                        if (ebi[y] >> (16-x) & 0b1) {
                            tft.drawPixel(mx, my, nColor(cnt + ncnt));
                        } else {
                            tft.drawPixel(mx, my, Color565(0, 0, 0));
                        }
                    }
                }
                cnt += 1;
            }
        }
        delay(160);
        for (int py = 0; py < 1; ++py) {
            for (int px = 0; px < 5; ++px) {
                for (int x = 0; x < 16; ++x) {
                    for (int y = 0; y < 16; ++y) {
                        int mx = px*16 + x;
                        int my = (py+t)*16 + y;
                        tft.drawPixel(mx, my, Color565(0, 0, 0));
                    }
                }
            }
        }
    }
    ncnt += 5;
    ncnt %= 96;
}



