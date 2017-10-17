#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(10, 9, 12, 11, 13); // SPI

// G V 10 9 13 11 12

const byte swich_pin = 16;

byte g_nbw[8][8];// 0無 1黒 2白

byte g_cp;// 駒位置
bool g_cs;// 黒番0か白番1か
byte g_cf;// 駒の点滅フレーム３０
byte g_cb = 0;// ブリンク状態
          //bool g_ctp[64];// 置くことができるマス
byte g_bcount;// 押し時間をカウント byteのため３０以上なら３０に戻す
bool g_bf;// ボタンを押していたか
bool g_end;// 終了フラグ

byte g_move_pos[64];
byte g_move_max;
byte g_move_index;


void setup() {
    u8g.setColorIndex(1);
    u8g.setContrast(0);
    
    pinMode(swich_pin, INPUT_PULLUP);

    g_nbw[3][3] = 2;
    g_nbw[3][4] = 1;
    g_nbw[4][3] = 1;
    g_nbw[4][4] = 2;
    MovePointInit();
}

void loop() {
    u8g.firstPage();
    do {
        Draw();
    } while (u8g.nextPage());
    if (g_end) {  while (1) {} }
    while (1) {
        delay(30);
        if (Button()) { break; }
    }
}

void MovePointInit() {
    byte index = 0;
    for (byte y = 0; y < 8; ++y) {
        for (byte x = 0; x < 8; ++x) {
            if (g_nbw[y][x] == 0 && CheckPut(y, x)) {
                g_move_pos[index] = y * 8 + x;
                index += 1;
            }
        }
    }
    g_move_max = index;
    g_move_index = 0;
    g_cp = g_move_pos[0];
}

bool CheckPut(int py, int px) {
    int mx[8] = { -1, 0, 1,-1, 1,-1, 0, 1 };
    int my[8] = { -1,-1,-1, 0, 0, 1, 1, 1 };
    for (int i = 0; i < 8; ++i) {
        if (CheckLine(py, px, my[i], mx[i])) { return true; }
    }
    return false;
}

bool CheckLine(int py, int px, int my, int mx) {
    bool f = false;
    byte bw = g_cs ? 2 : 1;
    for (int i = 0; i < 8; ++i) {
        px += mx;
        py += my;
        if (CheckInside(py, px)) {
            if (g_nbw[py][px] == 0 || (!f && g_nbw[py][px] == bw)) { return false; }
            if (g_nbw[py][px] == bw) {
                return true;
            }
            else {
                f = true;
            }
        }
    }
    return false;
}

bool CheckInside(int py, int px) {
    if (min(py, px) < 0 || max(py, px) > 7) {
        return false;
    }
    else {
        return true;
    }
}

void Reverse(int py, int px) {
    int mx[8] = { -1, 0, 1,-1, 1,-1, 0, 1 };
    int my[8] = { -1,-1,-1, 0, 0, 1, 1, 1 };
    for (int i = 0; i < 8; ++i) { ReverseLine(py, px, my[i], mx[i]); }
}

void ReverseLine(int py, int px, int my, int mx) {
    bool f = false;
    byte bw = g_cs ? 2 : 1;
    for (int i = 0; i < 8; ++i) {
        px += mx;
        py += my;
        if (CheckInside(py, px)) {
            if (g_nbw[py][px] == 0 || (!f && g_nbw[py][px] == bw)) { return; }
            if (g_nbw[py][px] == bw) {
                for (; i > -1; --i) {
                    px -= mx;
                    py -= my;
                    g_nbw[py][px] = bw;
                }
                break;
            }
            else {
                f = true;
            }
        }
    }
}

bool Button() {
    bool f = false;
    if (digitalRead(swich_pin) == 0) {
        g_bf = true;
        g_bcount += 1;
        if (g_bcount > 29) {
            g_bcount = 30;
            g_cb = 1;
            return 1;
        }
    } else {
        if (g_bf) {
            if (g_bcount > 29) {
                NextTurn();
                g_cb = 0;
            } else {
                MovePos();
            }
            f = true;
        }
        g_bf = false;
        g_bcount = 0;
    }
    return f;
}

void MovePos() {
    tone(19, 900, 10);
    g_move_index = (g_move_index + 1) % g_move_max;
    g_cp = g_move_pos[g_move_index];
}

void NextTurn() {
    tone(19, 1200, 10);
    int py = g_cp / 8;
    int px = g_cp % 8;
    Reverse(py, px);

    g_cs = !g_cs;
    MovePointInit();
    if (g_move_max > 0) { return; }// パス１回目
    g_cs = !g_cs;
    MovePointInit();
    if (g_move_max > 0) { return; }// パス２回目
    g_end = true;
}

void Draw() {
    Grid();
    int n = 6;
    int w = 0;
    int b = 0;
    for (byte y = 0; y<8; ++y) {
        for (byte x = 0; x<8; ++x) {
            if (g_nbw[y][x] != 0) {
                u8g.drawCircle(9 + n * x, 9 + n * y, 2);
                if (g_nbw[y][x] == 1) {
                    u8g.drawBox(8 + n * x, 8 + n * y, 3, 3);
                    ++b;
                } else {
                    ++w;
                }
            }
        }
    }
    
    u8g.setFont(u8g_font_unifont);
    char s[32];
    sprintf(s, "Black=%d", b);
    u8g.drawStr(58, 16, s);
    sprintf(s, "White=%d", w);
    u8g.drawStr(58, 32, s);
    if (!g_end){ u8g.drawFrame(57, 3+15 * g_cs, 66, 16); }
    for (int i = 0; i < g_move_max; i++) {
        u8g.drawCircle(9 + n * (g_move_pos[i] % 8), 9 + n * (g_move_pos[i] / 8), 0);
    }
    u8g.drawCircle(9 + n * (g_cp % 8), 9 + n * (g_cp / 8), 1 + g_cb);
}

void Grid() {
    int n = 6;
    for (int x = 0; x < 9; x++) {
        u8g.drawLine(n, n*x + n, n * 9, n*x + n);
    }
    for (int y = 0; y < 9; y++) {
        u8g.drawLine(n*y + n, n, n*y + n, n * 9);
    }
}