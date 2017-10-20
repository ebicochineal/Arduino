//                             0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
const static byte pins[17] = {-1, 5, 4, 3, 2,14,15,16,17,13,12,11,10, 9, 8, 7, 6};
const static byte cols[8] = {pins[13],pins[ 3],pins[ 4],pins[10],pins[ 6],pins[11],pins[15],pins[16]};
const static byte rows[8] = {pins[ 9],pins[14],pins[ 8],pins[12],pins[ 1],pins[ 7],pins[ 2],pins[ 5]};

// tone pin 19
const byte swich_pin = 18;

byte g_nbw[8][8];// 0無 1黒 2白

byte g_cp;// 駒位置
bool g_cs;// 黒番0か白番1か
byte g_cf;// 駒の点滅フレーム３０
bool g_cb;// ブリンク状態
byte g_bcount;// 押し時間をカウント
bool g_bf;// ボタンを押していたか
bool g_end;// 終了フラグ

byte g_move_pos[64];
byte g_move_max;
byte g_move_index;


void setup() {
    for(byte i=1;i<=16;i++) pinMode(pins[i], OUTPUT);
    for(byte i=0;i<= 8;i++) digitalWrite(cols[i], LOW);
    for(byte i=0;i<= 8;i++) digitalWrite(rows[i], LOW);
    
    pinMode(swich_pin, INPUT_PULLUP);
    
    g_nbw[3][3] = 2;
    g_nbw[3][4] = 1;
    g_nbw[4][3] = 1;
    g_nbw[4][4] = 2;
    MovePointInit();
    //Serial.begin(9600);
}


void loop() {
    byte im[8][8];
    for(byte i=0; i<8; ++i){
        if(g_end){
            SetImageEnd(im);
        }else{
            SetImage(im);
            g_cf = (g_cf+1)%30;
            Button();
        }
        Draw(im);
    }
}


void MovePointInit(){
    byte index = 0;
    for(byte y = 0; y < 8; ++y){
        for(byte x = 0; x < 8; ++x){
            if(g_nbw[y][x] == 0 && CheckPut(y, x)){
                g_move_pos[index] = y * 8 + x;
                index += 1;
            }
        }
    }
    g_move_max = index;
    g_move_index = 0;
    g_cp = g_move_pos[0];
}

bool CheckPut(int py, int px){
    int mx[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
    int my[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    for(int i = 0; i < 8; ++i){
        if(CheckLine(py, px, my[i], mx[i])){ return true; }
    }
    return false;
}

bool CheckLine(int py, int px, int my, int mx){
    bool f = false;
    byte bw = g_cs ? 2 : 1;
    for (int i = 0; i < 8; ++i){
        px += mx;
        py += my;
        if(CheckInside(py, px)){
            if(g_nbw[py][px] == 0 || (!f && g_nbw[py][px] == bw) ){ return false; }
            if(g_nbw[py][px] == bw){
                return true;
            }else{
                f = true;
            }
        }
    }
    return false;
}

bool CheckInside(int py, int px){
    if(min(py, px) < 0 || max(py, px) > 7){
        return false;
    }else{
        return true;
    }
}

void Reverse(int py, int px){
    int mx[8] = {-1, 0, 1,-1, 1,-1, 0, 1};
    int my[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    for(int i = 0; i < 8; ++i){ ReverseLine(py, px, my[i], mx[i]); }
}

void ReverseLine(int py, int px, int my, int mx){
    bool f = false;
    byte bw = g_cs ? 2 : 1;
    for (int i = 0; i < 8; ++i){
        px += mx;
        py += my;
        if(CheckInside(py, px)){
            if(g_nbw[py][px] == 0 || (!f && g_nbw[py][px] == bw) ){ return; }
            if(g_nbw[py][px] == bw){
                for(; i > -1; --i){
                    px -= mx;
                    py -= my;
                    g_nbw[py][px] = bw;
                }
                break;
            }else{
                f = true;
            }
        }
    }
}

void Button(){
    if(digitalRead(swich_pin) == 0){
        g_bf = true;
        g_bcount += 1;
        if(g_bcount > 29){ g_bcount = 30; }
    }else{
        if(g_bf){
            if(g_bcount > 29){
                NextTurn();
            } else {
                MovePos();
            }
        }
        g_bf = false;
        g_bcount = 0;
    }
}

void MovePos(){
    tone(19, 900, 10);
    g_move_index = (g_move_index + 1) % g_move_max;
    g_cp = g_move_pos[g_move_index];
}

void NextTurn(){
    tone(19, 1200, 10);
    int py = g_cp / 8;
    int px = g_cp % 8;
    Reverse(py, px);
    
    g_cs = !g_cs;
    MovePointInit();
    if(g_move_max > 0){ return; }// パス１回目
    g_cs = !g_cs;
    MovePointInit();
    if(g_move_max > 0){ return; }// パス２回目
    g_end = true;
}

void SetImage(byte im[8][8]){
    for(byte y=0; y<8; ++y){
        for(byte x=0; x<8; ++x){
            if(g_nbw[y][x] == 0){
                im[y][x] = 4;
            }else{
                im[y][x] = g_nbw[y][x] == 1 ? 0 : 7;
            }
        }
    }
    byte h = g_cs ? 7 : 4;
    byte l = g_cs ? 4 : 0;
    byte blink_frame = g_bcount > 29 ? 3 : 6;
    if(g_cf % blink_frame == 0){g_cb = !g_cb;}
    im[g_cp/8][g_cp%8] = g_cb ? l : h;
}

void SetImageEnd(byte im[8][8]){
    for(byte y=0; y<8; ++y){
        for(byte x=0; x<8; ++x){
            if(g_nbw[y][x] == 0){
                im[y][x] = 4;
            }else{
                im[y][x] = g_nbw[y][x] == 1 ? 0 : 7;
            }
        }
    }
}


byte aim[6][8][8];
void PushAfter(byte im[8][8]){
    for(byte z=0; z<6; ++z){
        for(byte y=0; y<8; ++y){
            for(byte x=0; x<8; ++x){
                if(z == 5){
                    aim[0][y][x] = max(im[y][x]-1, 0);
                }else{
                    aim[5-z][y][x] = max(aim[4-z][y][x]-1, 0);
                }
            }
        }
    }
}

void AfterDraw(byte im[8][8]){
    for(byte y=0; y<8; ++y){
        digitalWrite(rows[y], 0);
        for(byte x=0; x<8; ++x){
            byte b = im[y][x];
            for(byte z=0; z<6; ++z){
                b = max(aim[z][y][x], b);
            }
            GradationLED(cols[x], b);
        }
        digitalWrite(rows[y], 1);
    }
}

void Draw(byte im[8][8]){
    for(byte y=0; y<8; ++y){
        digitalWrite(rows[y], 0);
        for(byte x=0; x<8; ++x){
            GradationLED(cols[x], im[y][x]);
        }
        digitalWrite(rows[y], 1);
    }
}

void GradationLED(byte pin, byte bright){
    if(bright == 0 || bright > 7 || pin > 18){
        delayMicroseconds(300);
    }else{
        int l[8] = {0, 0, 5, 15, 40, 75, 150, 300};
        int b = l[bright];
        if(pin < 8){//  0 1 2 3 4 5 6 7
            PORTD |= _BV(pin);
            delayMicroseconds(b);
            PORTD &= ~_BV(pin);
        }else if(pin < 14){//  8 9 10 11 12 13
            byte _pin = pin-8;
            PORTB |= _BV(_pin);
            delayMicroseconds(b);
            PORTB &= ~_BV(_pin);
        }else{// 14 15 16 17 18
            byte _pin = pin-14;
            PORTC |= _BV(_pin);
            delayMicroseconds(b);
            PORTC &= ~_BV(_pin); 
        }
        delayMicroseconds(300 - b);
    }
}

