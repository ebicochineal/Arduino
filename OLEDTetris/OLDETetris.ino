#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(10, 9, 12, 11, 13); // SPI

// G V 10 9 13 11 12

const byte H = 20;
const byte W = 10;

byte WX = 0;
byte WY = 2;

struct pos {
    int x;
    int y;
};

byte tet[H+1][W];
pos mino[4];
pos fallmino[4];
pos rotmino[4];
byte minotype = 0;

int score = 0;
int maxscore = 0;
int jx, jy, jz;
int px, py;
int raw_jx, raw_jy;

void setup() {
    u8g.setColorIndex(1);
    u8g.setContrast(0);
    pinMode(14, INPUT);       // X uno14 leo18
    pinMode(15, INPUT);       // Y uno15 leo19
    pinMode(16, INPUT_PULLUP);// 3 uno16 leo20
    minotype = random(0, 7);
    SetMino();
}

int draw_loop = 1;
int frame = 0;

void loop() {
    ++frame;
    JoyStick();
    MoveMino();
    if (frame > 6) {
        frame = 0;
        FallMino();
    }
    DeleteLine();
    FallLine();
    GameOver();
    for (byte i = 0; i < draw_loop; i++) {
        FallMinoPosSet();
        Draw();
        delay(10);  
    }
    draw_loop = 1;
}

void SetMino() {
    int x = W / 2 - 2;
    int y = H - 1;
    if (minotype == 0) {
        mino[0].x = 0;
        mino[1].x = 1;
        mino[2].x = 2;
        mino[3].x = 3;
        mino[0].y = 0;
        mino[1].y = 0;
        mino[2].y = 0;
        mino[3].y = 0;
    }
    if (minotype == 1) {
        mino[0].x = 0;
        mino[1].x = 1;
        mino[2].x = 0;
        mino[3].x = 1;
        mino[0].y = 0;
        mino[1].y = 0;
        mino[2].y = 1;
        mino[3].y = 1;
    }
    if (minotype == 2) {
        mino[0].x = 0;
        mino[1].x = 1;
        mino[2].x = 1;
        mino[3].x = 2;
        mino[0].y = 0;
        mino[1].y = 0;
        mino[2].y = 1;
        mino[3].y = 1;
    }
    if (minotype == 3) {
        mino[0].x = 0;
        mino[1].x = 1;
        mino[2].x = 1;
        mino[3].x = 2;
        mino[0].y = 1;
        mino[1].y = 1;
        mino[2].y = 0;
        mino[3].y = 0;
    }
    if (minotype == 4) {
        mino[0].x = 0;
        mino[1].x = 0;
        mino[2].x = 1;
        mino[3].x = 2;
        mino[0].y = 1;
        mino[1].y = 0;
        mino[2].y = 0;
        mino[3].y = 0;
    }
    if (minotype == 5) {
        mino[0].x = 0;
        mino[1].x = 1;
        mino[2].x = 2;
        mino[3].x = 2;
        mino[0].y = 0;
        mino[1].y = 0;
        mino[2].y = 0;
        mino[3].y = 1;
    }
    if (minotype == 6) {
        mino[0].x = 0;
        mino[1].x = 1;
        mino[2].x = 1;
        mino[3].x = 2;
        mino[0].y = 0;
        mino[1].y = 0;
        mino[2].y = 1;
        mino[3].y = 0;
    }

    for (byte i = 0; i < 4; i++) {
        mino[i].x += x;
        mino[i].y += y;
    }

    minotype = random(0, 7);
    draw_loop = 3;
}

void FallMinoPosSet() {
    for (byte i = 0; i < 4; i++) {
        fallmino[i].x = mino[i].x;
        fallmino[i].y = mino[i].y;
    }
    bool f = true;
    while (f) {
        for (byte i = 0; i < 4; i++) {
            if (!(fallmino[i].y > 0 && tet[fallmino[i].y - 1][fallmino[i].x] == 0)) {
                f = false;
            }
        }
        if (f) {
            for (byte i = 0; i < 4; i++) { --fallmino[i].y; }
        }
    }
}

bool bj0 = false;
bool bj0d = false;
bool JoyDown0() {
    if (bj0d) {
        bj0d = false;
        return true;
    } else {
        return false;
    }
}

void MoveMino() {
    if (JoyDown0()) {
        RotateMino();
    } else {
        if (abs(jy) > 4) {
            int x = jy < 0 ? 1 : -1;
            bool col = false;
            for (byte i = 0; i < 4; i++) {
                int px = mino[i].x + x;
                if (!(px >= 0 && px < W && tet[mino[i].y][px] == 0)) {
                    col = true;
                }
            }
            if (!col) {
                for (byte i = 0; i < 4; i++) { mino[i].x += x; }
            }
        } else {
            if (jx > 4) { while (!FallMino()) {} }
            if (jx > 0) { frame = 7; }
            if (jz) { 
                delay(150);
                WX = (WX + 1) % 4;
            }
        }
    }
}


void RotateMino() {
    int minx = W+4;
    int maxx = -4;
    int miny = H+4;
    int maxy = -4;
    int x, y;
    for (byte i = 0; i < 4; i++) {
        x = mino[i].x;
        y = mino[i].y;
        minx = min(minx, x);
        maxx = max(maxx, x);
        miny = min(miny, y);
        maxy = max(maxy, y);
    }
    int mx = minx + (maxx - minx) / 2;
    int my = miny + (maxy - miny) / 2;
    for (byte i = 0; i < 4; i++) {
        rotmino[i].y = my + (mx - mino[i].x) + (maxx - minx) % 2;
        rotmino[i].x = mx + (mino[i].y - my);
    }

    if (XPushBackMino() && YPushBackMino()) {
        for (byte i = 0; i < 4; i++) {
            mino[i].y = rotmino[i].y;
            mino[i].x = rotmino[i].x;
        }
    }
}

void FixMino(int y, int x) {
    for (byte i = 0; i < 4; i++) {
        mino[i].y += y;
        mino[i].x += x;
    }
}

void FixRotMino(int y, int x) {
    for (byte i = 0; i < 4; i++) {
        rotmino[i].y += y;
        rotmino[i].x += x;
    }
}

bool XPushBackMino() {
    bool left = false;
    bool right = false;
    int x, y;

    int minx = W + 4;
    int maxx = -4;

    for (byte i = 0; i < 4; i++) {
        minx = min(minx, rotmino[i].x);
        maxx = max(maxx, rotmino[i].x);
    }
    int mx = minx + (maxx - minx) / 2;
    bool f = true;
    while(f && !(left && right)){
        f = false;
        for (byte i = 0; i < 4; i++) {
            y = rotmino[i].y;
            x = rotmino[i].x;
            if (x < mx && tet[y][x] && tet[y][x+1] == 0) {
                mx += 1;
                left = true;
                f = true;
                FixRotMino(0, 1);
            }
            if (x > mx && tet[y][x] && tet[y][x-1] == 0) {
                mx -= 1;
                right = true;
                f = true;
                FixRotMino(0, -1);
            }

            if (x < 0) {
                mx += -x;
                left = true;
                f = true;
                FixRotMino(0, -x);
            }
            if (x > W - 1) {
                mx += W - 1 - x;
                right = true;
                f = true;
                FixRotMino(0, W - 1 - x);
            }
        }
    }
    return !(left && right);
}

bool YPushBackMino() {
    bool up = false;
    bool down = false;
    int x, y;

    int miny = H + 4;
    int maxy = -4;

    for (byte i = 0; i < 4; i++) {
        miny = min(miny, mino[i].y);
        maxy = max(maxy, mino[i].y);
    }
    int my = miny + (maxy - miny) / 2;

    bool f = true;
    while (f && !(up && down)) {
        f = false;
        for (byte i = 0; i < 4; i++) {
            y = rotmino[i].y;
            x = rotmino[i].x;
            if (y < my && tet[y][x]) {
                my += 1;
                up = true;
                f = true;
                FixRotMino(1, 0);
            }
            if (y > my && tet[y][x]) {
                my -= 1;
                down = true;
                f = true;
                FixRotMino(-1, 0);
            }
            if (y < 0) {
                my += -y;
                up = true;
                f = true;
                FixRotMino(-y, 0);
            }
            if (y > H - 1) {
                my += H - 1 - y;
                down = true;
                f = true;
                FixRotMino(H - 1 - y, 0);
            }
        }
    }
    return !(up && down);
}

bool FallMino() {
    bool col = false;
    for (byte i = 0; i < 4; i++) {
        if (!(mino[i].y > 0 && tet[mino[i].y-1][mino[i].x] == 0)) {
            col = true;
        }
    }
    if (col) {
        for (byte i = 0; i < 4; i++) {
            tet[mino[i].y][mino[i].x] = 1;
        }
        SetMino();
    } else {
        for (byte i = 0; i < 4; i++) { --mino[i].y; }
    }
    return col;
}

void DeleteLine() {
    byte linecnt = 0;
    for (byte y = 0; y < H; y++) {
        byte cnt = 0;
        for (byte x = 0; x < W; x++) {
            cnt += tet[y][x];
        }
        if (cnt == W) {
            ++linecnt;
            for (byte x = 0; x < W; x++) { tet[y][x] = 0; }
        }
    }
    score += 100 * linecnt * linecnt;
}

void FallLine() {
    byte hcnt = 0;
    for (byte y = 0; y < H; y++) {
        byte wcnt = 0;
        for (byte x = 0; x < W; x++) { wcnt += tet[y][x]; }
        if (wcnt > 0 && hcnt < 1) { continue; }
        if (wcnt < 1) { ++hcnt; }
        if (wcnt > 0 && hcnt > 0) {
            for (byte x = 0; x < W; x++) {
                tet[y-hcnt][x] = tet[y][x];
                tet[y][x] = 0;
            }
        }
    }
}

void GameOver() {
    int cnt = 0;
    for (byte x = 0; x < 4; x++) { cnt += tet[H-1][W/2-2+x]; }
    if (cnt) {
        for (byte y = 0; y < H+1; y++) {
            for (byte x = 0; x < W; x++) {
                tet[y][x] = 0;
            }
        }
        minotype = random(0, 7);
        SetMino();
        maxscore = max(score, maxscore);
        score = 0;
    }
}

void JoyStick() {
    raw_jx = analogRead(14);
    raw_jy = analogRead(15);
    int x = (raw_jx - 512) / 80;
    int y = (raw_jy - 512) / 80;
    jx = x - (abs(x) < 2 ? x : 0);
    jy = y - (abs(y) < 2 ? y : 0);
    jz = !digitalRead(16);

    if (jx < -4 && bj0) {
        bj0 = false;
        bj0d = true;
    }
    if (jx >= -2) { 
        bj0 = true;
        bj0d = false;
    }
}

int wave = 0;
void Draw() {
    wave++;
    u8g.firstPage();
    do {
        _Draw();
        _DrawNextMino();
    } while (u8g.nextPage());
}

void _Draw() {
    u8g.setFont(u8g_font_unifont);
    char s[32];
    sprintf(s, "%d", maxscore);
    u8g.drawStr90(128 - 12, 0, s);
    sprintf(s, "%d", score);
    u8g.drawStr90(128 - 24, 0, s);
    for (byte y = 0; y < H+2; y++) {
        int e = sin((y + wave) / WY) * WX + WX;
        for (byte x = 0; x < W+2; x++) {
            if (x > 0 && x < W+1 && y>0 && y < H+1) {
                if (tet[y-1][x-1]) { 
                    u8g.drawBox(4 * y, 4 * x + e, 3, 3);
                }
            } else {
                u8g.drawBox(4 * y, 4 * x + e, 3, 3);
            }
        }
    }

    for (byte i = 0; i < 4; i++) {
        int e = sin((mino[i].y + 1 + wave) / WY) * WX + WX;
        u8g.drawBox(4 + 4 * mino[i].y, 4 + 4 * mino[i].x + e, 3, 3);
    }

    for (byte i = 0; i < 4; i++) {
        int e = sin((fallmino[i].y + 1 + wave) / WY) * WX + WX;
        u8g.drawFrame(4 + 4 * fallmino[i].y, 4 + 4 * fallmino[i].x + e, 4, 4);
    }
}

void _DrawNextMino() {
    int x = W / 2 - 2;
    int y = H + 2;
    pos dmino[4];
    if (minotype == 0) {
        dmino[0].x = 0;
        dmino[1].x = 1;
        dmino[2].x = 2;
        dmino[3].x = 3;
        dmino[0].y = 0;
        dmino[1].y = 0;
        dmino[2].y = 0;
        dmino[3].y = 0;
    }
    if (minotype == 1) {
        dmino[0].x = 0;
        dmino[1].x = 1;
        dmino[2].x = 0;
        dmino[3].x = 1;
        dmino[0].y = 0;
        dmino[1].y = 0;
        dmino[2].y = 1;
        dmino[3].y = 1;
    }
    if (minotype == 2) {
        dmino[0].x = 0;
        dmino[1].x = 1;
        dmino[2].x = 1;
        dmino[3].x = 2;
        dmino[0].y = 0;
        dmino[1].y = 0;
        dmino[2].y = 1;
        dmino[3].y = 1;
    }
    if (minotype == 3) {
        dmino[0].x = 0;
        dmino[1].x = 1;
        dmino[2].x = 1;
        dmino[3].x = 2;
        dmino[0].y = 1;
        dmino[1].y = 1;
        dmino[2].y = 0;
        dmino[3].y = 0;
    }
    if (minotype == 4) {
        dmino[0].x = 0;
        dmino[1].x = 0;
        dmino[2].x = 1;
        dmino[3].x = 2;
        dmino[0].y = 1;
        dmino[1].y = 0;
        dmino[2].y = 0;
        dmino[3].y = 0;
    }
    if (minotype == 5) {
        dmino[0].x = 0;
        dmino[1].x = 1;
        dmino[2].x = 2;
        dmino[3].x = 2;
        dmino[0].y = 0;
        dmino[1].y = 0;
        dmino[2].y = 0;
        dmino[3].y = 1;
    }
    if (minotype == 6) {
        dmino[0].x = 0;
        dmino[1].x = 1;
        dmino[2].x = 1;
        dmino[3].x = 2;
        dmino[0].y = 0;
        dmino[1].y = 0;
        dmino[2].y = 1;
        dmino[3].y = 0;
    }

    for (byte i = 0; i < 4; i++) {
        dmino[i].x += x;
        dmino[i].y += y;
    }

    for (byte i = 0; i < 4; i++) {
        int e = sin((dmino[i].y + 1 + wave) / WY) * WX + WX;
        u8g.drawBox(4 + 4 * dmino[i].y, 4 + 4 * dmino[i].x + e, 3, 3);
    }
}
