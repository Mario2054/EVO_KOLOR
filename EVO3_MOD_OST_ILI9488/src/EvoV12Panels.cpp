#include "EvoV12Panels.h"

static const uint16_t C_BG = 0x0000;
static const uint16_t C_PANEL = 0x0206;
static const uint16_t C_CYAN = 0x07FF;
static const uint16_t C_BLUE = 0x03BF;
static const uint16_t C_YELLOW = 0xFFE0;
static const uint16_t C_GREEN = 0x07E0;
static const uint16_t C_ORANGE = 0xFD20;
static const uint16_t C_RED = 0xF800;
static const uint16_t C_WHITE = 0xFFFF;
static const uint16_t C_DIM = 0x39E7;
static const uint16_t C_DARK = 0x1082;

static uint16_t levelColor(int pct) {
  pct = constrain(pct, 0, 100);
  if (pct < 38) return C_GREEN;
  if (pct < 66) return C_YELLOW;
  if (pct < 84) return C_ORANGE;
  return C_RED;
}

static String clipText(String s, int maxChars) {
  if ((int)s.length() <= maxChars) return s;
  return s.substring(0, maxChars - 2) + "..";
}

static String timeText() {
  unsigned long sec = millis() / 1000UL;
  int hh = (sec / 3600UL) % 24;
  int mm = (sec / 60UL) % 60;
  char b[8];
  snprintf(b, sizeof(b), "%02d:%02d", hh, mm);
  return String(b);
}

static void tx(int x, int y, String s, uint8_t sz, uint16_t c, uint16_t bg = C_BG) {
  display.setTextSize(sz);
  display.setTextColor(c, bg);
  display.setCursor(x, y);
  display.print(s);
}

static void center(int x, int y, int w, String s, uint8_t sz, uint16_t c) {
  display.setTextSize(sz);
  int tw = display.textWidth(s);
  tx(x + max(0, (w - tw) / 2), y, s, sz, c);
}

static void rightText(int x, int y, int w, String s, uint8_t sz, uint16_t c) {
  display.setTextSize(sz);
  int tw = display.textWidth(s);
  tx(x + max(0, w - tw), y, s, sz, c);
}

static void panel(int x, int y, int w, int h, uint16_t border = C_CYAN, uint16_t fill = C_BG) {
  display.fillRoundRect(x + 3, y + 3, w, h, 8, 0x0008);
  display.fillRoundRect(x, y, w, h, 8, fill);
  display.drawRoundRect(x, y, w, h, 8, border);
  display.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 7, 0x401F);
  display.drawFastHLine(x + 9, y + 3, w - 18, 0x07FF);
  display.drawFastVLine(x + 3, y + 9, h - 18, 0x07FF);
  display.drawFastHLine(x + 9, y + h - 4, w - 18, 0x0010);
  display.drawFastVLine(x + w - 4, y + 9, h - 18, 0x0010);
}

static void header(const EvoUiState& s, String title) {
  panel(6, 6, 468, 36, C_CYAN, C_BG);
  for (int i = 0; i < 5; i++) {
    int h = 5 + i * 5;
    display.fillRoundRect(18 + i * 8, 31 - h, 5, h, 2, i < s.wifiLevel ? C_CYAN : C_DARK);
  }
  panel(142, 12, 200, 22, C_CYAN, C_BG);
  center(142, 18, 200, clipText(title.length() ? title : s.station, 24), 1, C_CYAN);
  rightText(394, 17, 64, timeText(), 1, C_WHITE);
}

static void footer(const EvoUiState& s) {
  panel(6, 282, 468, 30, C_DIM, C_BG);
  tx(18, 293, s.sampleRate.length() ? s.sampleRate : "44.1kHz", 1, C_CYAN);
  tx(92, 293, s.bits.length() ? s.bits : "16bit", 1, C_WHITE);
  tx(158, 293, s.bitrate.length() ? s.bitrate + "kbps" : "128kbps", 1, C_WHITE);
  tx(240, 293, s.codec.length() ? s.codec : "MP3", 1, C_WHITE);
  panel(340, 287, 55, 20, C_CYAN, C_BG);
  center(340, 293, 55, "B-" + String(s.bank < 10 ? "0" : "") + String(s.bank), 1, C_WHITE);
  for (int i = 0; i < 5; i++) {
    int h = 5 + i * 5;
    display.fillRoundRect(425 + i * 8, 306 - h, 5, h, 2, i < s.wifiLevel ? C_CYAN : C_DARK);
  }
}

static void progress(int x, int y, int w, int h, int value, int maxv, uint16_t col) {
  display.drawRoundRect(x, y, w, h, 3, C_DIM);
  display.fillRoundRect(x + 2, y + 2, w - 4, h - 4, 2, C_BG);
  int fw = ((int32_t)constrain(value, 0, maxv) * (w - 4)) / max(1, maxv);
  if (fw > 0) display.fillRoundRect(x + 2, y + 2, fw, h - 4, 2, col);
}

static void sevenDigit(int x, int y, int s, char ch, uint16_t col) {
  int w = 10 * s, h = 18 * s, th = max(2, 2 * s);
  int a=0,b=0,c=0,d=0,e=0,f=0,g=0;
  switch(ch){case '0':a=b=c=d=e=f=1;break;case '1':b=c=1;break;case '2':a=b=g=e=d=1;break;case '3':a=b=c=d=g=1;break;case '4':f=g=b=c=1;break;case '5':a=f=g=c=d=1;break;case '6':a=f=e=d=c=g=1;break;case '7':a=b=c=1;break;case '8':a=b=c=d=e=f=g=1;break;case '9':a=b=c=d=f=g=1;break;}
  uint16_t off = 0x1082;
  auto H=[&](int xx,int yy,int len,uint16_t cc){display.fillRoundRect(xx,yy,len,th,th/2,cc);};
  auto V=[&](int xx,int yy,int len,uint16_t cc){display.fillRoundRect(xx,yy,th,len,th/2,cc);};
  H(x+s,y,w-2*s,a?col:off); V(x+w-th,y+s,h/2-s,b?col:off); V(x+w-th,y+h/2,h/2-s,c?col:off);
  H(x+s,y+h-th,w-2*s,d?col:off); V(x,y+h/2,h/2-s,e?col:off); V(x,y+s,h/2-s,f?col:off); H(x+s,y+h/2-th/2,w-2*s,g?col:off);
}

static void sevenText(int x, int y, String s, int scale, uint16_t col) {
  int cx = x;
  for (uint16_t i=0; i<s.length(); i++) {
    char ch = s[i];
    if (ch >= '0' && ch <= '9') { sevenDigit(cx, y, scale, ch, col); cx += 12 * scale; }
    else if (ch == ':') { display.fillCircle(cx + 3 * scale, y + 6 * scale, scale + 1, col); display.fillCircle(cx + 3 * scale, y + 13 * scale, scale + 1, col); cx += 6 * scale; }
    else if (ch == '.') { display.fillCircle(cx + 3 * scale, y + 17 * scale, scale + 1, col); cx += 6 * scale; }
    else { cx += 5 * scale; }
  }
}

static void smallVuBars(int x, int y, int w, int v, int peak, const char* label) {
  int pct = map(constrain(v, 0, 255), 0, 255, 0, 100);
  int pk = map(constrain(peak, 0, 255), 0, 255, 0, 100);
  tx(x, y + 7, label, 2, C_WHITE);
  int segs = 30;
  int base = x + 32;
  int usable = w - 40;
  int sw = max(5, usable / segs);
  for (int i = 0; i < segs; i++) {
    int sp = ((i + 1) * 100) / segs;
    int bh = 9 + (i % 5) * 2;
    display.fillRoundRect(base + i * sw, y + 31 - bh, sw - 2, bh, 2, sp <= pct ? levelColor(sp) : C_DARK);
  }
  if (pk > 0) {
    int px = base + ((int32_t)pk * usable) / 100;
    display.drawFastVLine(px, y + 2, 33, C_RED);
  }
}

static void mainRadio(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, s.station);
  panel(10, 52, 460, 90);
  tx(26, 66, "CURRENT STATION", 1, C_CYAN);
  tx(26, 88, clipText(s.station, 24), 2, C_WHITE);
  sevenText(260, 76, "93.60", 2, C_WHITE);
  tx(405, 90, "FM", 1, C_CYAN);
  panel(10, 152, 460, 82);
  smallVuBars(24, 166, 420, s.vuL, s.peakL, "L");
  smallVuBars(24, 196, 420, s.vuR, s.peakR, "R");
  panel(10, 242, 460, 34, C_DIM);
  tx(26, 252, "TRACK:", 1, C_CYAN);
  tx(90, 252, clipText(s.track, 46), 1, C_WHITE);
  footer(s);
}

static void bootPanel(const EvoUiState& s) {
  display.fillScreen(C_BG);
  panel(36, 40, 408, 230);
  center(36, 93, 408, "EVO", 5, C_CYAN);
  center(36, 153, 408, "RADIO MOD", 2, C_WHITE);
  center(36, 185, 408, "V12 ILI9488", 2, C_CYAN);
  progress(86, 228, 308, 14, 72, 100, C_CYAN);
}

static void infoPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "RADIO INFO");
  panel(14, 56, 132, 210); center(14, 100, 132, "RADIO", 2, C_CYAN); center(14, 132, 132, "INFO", 3, C_WHITE);
  panel(158, 56, 308, 210);
  tx(178, 76, clipText(s.station, 22), 2, C_CYAN);
  tx(178, 114, "Gatunek: Informacje", 1, C_WHITE);
  tx(178, 136, "Region:  Slask", 1, C_WHITE);
  tx(178, 158, "Jezyk:   Polski", 1, C_WHITE);
  tx(178, 180, "Audio:   " + s.codec + " " + s.bitrate + "kbps", 1, C_WHITE);
  tx(178, 214, clipText(s.track, 34), 1, C_YELLOW);
  footer(s);
}

static void clockPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, s.station);
  panel(10, 54, 460, 218);
  sevenText(58, 92, timeText() + ":00", 4, C_CYAN);
  center(10, 178, 460, "29.04.2026  |  Wednesday", 1, C_WHITE);
  center(10, 214, 460, clipText(s.station, 30), 2, C_CYAN);
  footer(s);
}

static void boomerang(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, s.station);
  panel(14, 58, 452, 214); center(14, 78, 452, "VU BOOMERANG", 2, C_YELLOW);
  int vals[2] = {s.vuL, s.vuR};
  const char* labs[2] = {"L", "R"};
  for (int row=0; row<2; row++) {
    int y = 132 + row * 56;
    tx(38, y - 6, labs[row], 2, C_WHITE);
    int pct = map(constrain(vals[row],0,255),0,255,0,100);
    int cx = 240;
    int segs = 26;
    for (int i=0;i<segs;i++) {
      int sp = ((i+1)*100)/segs, dist = ((i+1)*170)/segs;
      uint16_t c = sp <= pct ? levelColor(sp) : C_DARK;
      int bh = 18 + (i % 4) * 3;
      display.fillRoundRect(cx-dist-6, y+18-bh/2, 6, bh, 2, c);
      display.fillRoundRect(cx+dist, y+18-bh/2, 6, bh, 2, c);
    }
    display.drawFastVLine(cx, y-20, 75, C_DIM);
  }
  footer(s);
}

static void needleMeter(int x, int y, int w, int h, int val, const char* lab, bool blue) {
  uint16_t bg = blue ? C_CYAN : C_BG;
  uint16_t fg = blue ? C_BG : C_WHITE;
  panel(x,y,w,h, blue ? C_WHITE : C_CYAN, bg);
  tx(x+15,y+14,lab,3,fg,bg);
  int pct = map(constrain(val,0,255),0,255,0,100);
  int cx=x+w/2, py=y+h-24, r=min(w/2-18,h-52);
  const char* lbls[]={"-20","-10","-5","0","+3","+6","+9"};
  int poss[]={0,20,35,50,65,80,100};
  for(int p=0;p<=100;p+=5){float a=radians(-72+144.0*p/100.0);int x1=cx+sin(a)*(r-16),y1=py-cos(a)*(r-16),x2=cx+sin(a)*r,y2=py-cos(a)*r;display.drawLine(x1,y1,x2,y2,blue?C_BG:(p<=pct?levelColor(p):C_DIM));}
  for(int i=0;i<7;i++){float a=radians(-72+144.0*poss[i]/100.0);int lx=cx+sin(a)*(r-36),ly=py-cos(a)*(r-36);tx(lx-10,ly-5,lbls[i],1,blue?C_BG:C_WHITE,bg);}
  float a=radians(-72+144.0*pct/100.0);int nx=cx+sin(a)*(r-5),ny=py-cos(a)*(r-5);display.drawLine(cx,py,nx,ny,blue?C_BG:levelColor(pct));display.drawLine(cx+1,py,nx+1,ny,blue?C_BG:levelColor(pct));display.fillCircle(cx,py,6,blue?C_BG:C_WHITE);
}

static void analogPanel(const EvoUiState& s, bool blue) {
  display.fillScreen(C_BG); header(s, blue ? "VU CRYSTAL" : "VU ANALOG");
  needleMeter(14, 70, 218, 176, s.vuL, "L", blue);
  needleMeter(248, 70, 218, 176, s.vuR, "R", blue);
  footer(s);
}

static void wingPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "VU WING");
  panel(14, 58, 452, 214);
  center(14, 82, 452, "WING V-METER", 2, C_YELLOW);
  int lv = map(constrain(s.vuL,0,255),0,255,0,100);
  int rv = map(constrain(s.vuR,0,255),0,255,0,100);
  int cx=240, cy=210;
  for(int i=0;i<20;i++){
    int p=(i+1)*5;
    uint16_t cl=p<=lv?levelColor(p):C_DARK, cr=p<=rv?levelColor(p):C_DARK;
    int len=40+i*5;
    display.drawLine(cx-8,cy,cx-len,cy-10-i*4,cl);
    display.drawLine(cx+8,cy,cx+len,cy-10-i*4,cr);
  }
  footer(s);
}

static void volumePanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "VOLUME");
  panel(80, 70, 320, 170);
  center(80, 96, 320, "GLOSNOSC", 2, C_WHITE);
  center(80, 132, 320, String(s.volume), 4, C_WHITE);
  for(int i=0;i<22;i++){uint16_t c=i<map(s.volume,0,63,0,22)?C_CYAN:C_DARK;display.fillRect(126+i*10,196,7,18,c);}
  footer(s);
}

static void listPanel(const EvoUiState& s, String title, bool banks, bool fav) {
  display.fillScreen(C_BG); header(s, title);
  panel(14, 54, 452, 220);
  const char* bankNames[]={"Pop & Rock","News & Talk","Chillout","Electronic","Favorites","Local"};
  const char* stations[]={"Radio Pogoda Katowice","RMF FM","Radio ZET","Eska ROCK","Radio 357","Chillizet","Deep House Lounge","Radio 24"};
  for(int i=0;i<8;i++){
    int y=70+i*23;
    if(i==0){display.fillRoundRect(28,y-4,416,20,4,0x0210);display.drawRoundRect(28,y-4,416,20,4,C_CYAN);}
    String row = banks ? String(i+1)+". "+String(i<6?bankNames[i]:"Bank") : String(i+1)+". "+String(stations[i]);
    tx(38,y,row,1,i==0?C_WHITE:C_DIM);
    rightText(370,y,60,banks ? ("("+String(12+i*3)+")") : "128 kbps",1,C_CYAN);
  }
  footer(s);
}

static void eqPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "EQUALIZER");
  panel(14,58,452,214,C_DIM);
  tx(30,76,"EQUALIZER",1,0x9CFF); rightText(350,76,80,"PRESET: ROCK",1,C_WHITE);
  const char* labs[]={"31","62","125","250","500","1K","2K","4K","8K","16K"};
  for(int i=0;i<10;i++){int x=54+i*39;int val=35+((i*17+s.vuL)%90);display.drawFastVLine(x+5,110,110,C_DARK);display.fillRect(x,220-val,11,val,0x5C9F);tx(x-6,232,labs[i],1,C_WHITE);}
  footer(s);
}

static void fftPanel(const EvoUiState& s, bool advanced) {
  display.fillScreen(C_BG); header(s, advanced ? "ANALIZATOR ZAAWANSOWANY" : "ANALIZATOR FFT");
  panel(14,58,452,214,C_DIM);
  int bars=advanced?56:42;
  for(int i=0;i<bars;i++){int h=18+((i*19+s.vuL+s.vuR)%130);int x=28+i*(advanced?7:10);display.fillRect(x,242-h,advanced?4:7,h,levelColor((i*100)/bars));}
  if(advanced){panel(338,86,90,150,C_DIM);tx(352,104,"LUFS",1,C_WHITE);tx(352,130,"-14.2",2,C_WHITE);tx(352,170,"TRUE",1,C_DIM);tx(352,190,"PEAK",1,C_DIM);}
  footer(s);
}

static void bluetoothPlayer(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "BLUETOOTH - ODTWARZACZ");
  panel(14,58,452,214,C_DIM);
  panel(40,96,90,90,0x5C9F); center(40,126,90,"EVO",2,C_CYAN); center(40,152,90,"RADIO",2,C_WHITE);
  tx(154,96,"Artysta",1,C_WHITE); tx(154,122,"Tytul utworu",2,C_WHITE); tx(154,158,"Album",1,C_DIM);
  progress(154,214,250,10,48,100,0x5C9F);
  footer(s);
}

static void bluetoothSettings(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "BLUETOOTH - USTAWIENIA");
  panel(14,58,452,214,C_DIM);
  String rows[]={"Bluetooth:        Wlaczony","Nazwa urzadzenia: EVO RADIO MOD","Widocznosc:       Widoczny","Parowanie:        Nowe urzadzenie","Polaczone:        SoundCore 2","Kodek audio:      SBC"};
  for(int i=0;i<6;i++) tx(34,84+i*26,rows[i],1,i%2?C_WHITE:C_GREEN);
  footer(s);
}

static void sdPlayer(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "SD - ODTWARZACZ");
  panel(14,58,452,214,C_DIM);
  panel(44,108,76,64,0x5C9F); center(44,132,76,"MP3",2,C_CYAN);
  tx(144,100,"Folder",1,C_DIM); tx(144,122,"Utwor.mp3",2,C_WHITE); tx(144,158,"Wykonawca",1,C_DIM);
  progress(144,210,260,9,48,100,0x5C9F);
  footer(s);
}

static void sdRecorder(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "SD - REJESTRATOR");
  panel(14,58,452,214,C_DIM);
  tx(34,84,"REC",2,C_RED); tx(330,84,"00:02:35",2,C_WHITE);
  for(int i=0;i<90;i++){int h=5+((i*23+s.vuL)%54);display.drawFastVLine(34+i*4,180-h/2,h,0x5C9F);}
  tx(34,218,"Plik: NAGRANIE_001.WAV",1,C_WHITE);
  footer(s);
}

static void fileBrowser(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "SD - MENEDZER PLIKOW");
  panel(14,58,452,214,C_DIM);
  String rows[]={"Music","AC_DC","Metallica","01 - Back In Black.mp3","02 - Hells Bells.mp3","03 - Shoot To Thrill.mp3"};
  for(int i=0;i<6;i++){int y=84+i*25;if(i==3){display.fillRect(30,y-5,410,20,0x0210);}tx(40,y,rows[i],1,C_WHITE);rightText(374,y,60,i>2?"5.1 MB":"<DIR>",1,C_DIM);}
  footer(s);
}

static void wifiPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "WIFI / WEBUI");
  panel(14,58,452,214,C_DIM);
  center(36,96,130,"WiFi",3,C_CYAN);
  for(int i=0;i<4;i++) display.drawCircle(100,160,20+i*18,C_CYAN);
  tx(210,100,"WiFi:    Polaczono",1,C_GREEN);
  tx(210,128,"SSID:    EVO_RADIO",1,C_WHITE);
  tx(210,156,"IP:      192.168.1.105",1,C_WHITE);
  tx(210,184,"WebUI:   Aktywny",1,C_GREEN);
  footer(s);
}

static void otaPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "OTA - AKTUALIZACJA");
  panel(14,58,452,214,C_DIM);
  tx(50,92,"Nowa wersja dostepna",1,C_GREEN);
  tx(50,124,"Wersja: 1.2.0",1,C_WHITE);
  tx(50,152,"Pobieranie...",1,C_WHITE);
  progress(50,206,330,14,68,100,C_GREEN); tx(392,207,"68%",1,C_WHITE);
  footer(s);
}

static void sleepPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "SLEEP TIMER");
  panel(14,58,452,214,C_DIM);
  center(14,114,452,"00:30:00",4,C_WHITE);
  center(14,180,452,"Timer will end at: 21:27",1,C_DIM);
  footer(s);
}

static void settingsPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "USTAWIENIA / INFO SYSTEMOWE");
  panel(14,58,452,214,C_DIM);
  String rows[]={"Network","Audio / EQ","Display","Time / Date","Language","Power / Sleep","Other Settings"};
  for(int i=0;i<7;i++){tx(42,84+i*25,"> "+rows[i],1,i==0?C_CYAN:C_WHITE);}
  footer(s);
}

static void systemPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "SYSTEM MONITOR");
  panel(14,58,452,214,C_DIM);
  String rows[]={"CPU Load","RAM Usage","Flash Usage","Temp CPU","Uptime"};
  for(int i=0;i<5;i++){int y=92+i*32;tx(42,y,rows[i],1,C_WHITE);progress(180,y,210,10,25+i*14,100,C_YELLOW);}
  footer(s);
}

static void alertPanel(const EvoUiState& s, String title, uint16_t col) {
  display.fillScreen(C_BG);
  panel(80,70,320,180,col,C_BG);
  center(80,104,320,title,3,col);
  center(80,158,320,"Low disk space",2,C_WHITE);
  center(80,194,320,"Press OK",1,C_DIM);
}

static void aboutPanel(const EvoUiState& s) {
  display.fillScreen(C_BG); header(s, "ABOUT");
  panel(14,58,452,214,C_DIM);
  center(14,94,452,"EVO RADIO MOD",3,C_CYAN);
  tx(90,146,"Version: 1.2.0",1,C_WHITE);
  tx(90,170,"Display: ILI9488 480x320",1,C_WHITE);
  tx(90,194,"Library: LovyanGFX 1.2.20",1,C_WHITE);
  tx(90,218,"MCU: ESP32-S3",1,C_WHITE);
  footer(s);
}

void evoV12DrawPanel(uint8_t panelId, const EvoUiState& s) {
  switch(panelId) {
    case 0: bootPanel(s); break;
    case 1: mainRadio(s); break;
    case 2: infoPanel(s); break;
    case 3: boomerang(s); break;
    case 4: analogPanel(s, false); break;
    case 5: analogPanel(s, true); break;
    case 6: wingPanel(s); break;
    case 7: clockPanel(s); break;
    case 8: volumePanel(s); break;
    case 9: listPanel(s, "LISTA STACJI", false, false); break;
    case 10: listPanel(s, "WYBOR BANKU", true, false); break;
    case 11: infoPanel(s); break;
    case 12: eqPanel(s); break;
    case 13: fftPanel(s, false); break;
    case 14: fftPanel(s, true); break;
    case 15: bluetoothPlayer(s); break;
    case 16: bluetoothSettings(s); break;
    case 17: sdPlayer(s); break;
    case 18: sdRecorder(s); break;
    case 19: fileBrowser(s); break;
    case 20: wifiPanel(s); break;
    case 21: otaPanel(s); break;
    case 22: sleepPanel(s); break;
    case 23: listPanel(s, "STATION SEARCH", false, false); break;
    case 24: listPanel(s, "FAVORITES", false, true); break;
    case 25: settingsPanel(s); break;
    case 26: systemPanel(s); break;
    case 27: alertPanel(s, "ALERT", C_YELLOW); break;
    case 28: alertPanel(s, "POWER OFF", C_RED); break;
    case 29: alertPanel(s, "SAFE MODE", C_GREEN); break;
    case 30: aboutPanel(s); break;
    default: mainRadio(s); break;
  }
}

// Podgląd 12 paneli na jednym ekranie, pomocniczo do testu.
static void thumbFrame(int x, int y, int w, int h, const char* label) {
  display.fillRoundRect(x, y, w, h, 4, C_BG);
  display.drawRoundRect(x, y, w, h, 4, C_DIM);
  center(x, y + h + 3, w, label, 1, C_WHITE);
}

void evoV12DrawOverviewPage1(const EvoUiState& s) {
  display.fillScreen(C_BG);
  center(0, 8, 480, "EVO RADIO MOD - PODGLAD PANELI", 2, C_WHITE);
  int w=108,h=62,idx=0;
  const char* labels[]={"BOOT","MAIN","INFO","BOOM","ANALOG","CRYSTAL","WING","CLOCK","VOLUME","STATIONS","BANK","INFO2"};
  for(int r=0;r<3;r++) for(int c=0;c<4;c++){int x=14+c*116,y=44+r*88;thumbFrame(x,y,w,h,labels[idx]);idx++;}
}

void evoV12DrawOverviewPage2(const EvoUiState& s) {
  display.fillScreen(C_BG);
  center(0, 8, 480, "EVO RADIO MOD - FUNKCJE I MODULY", 2, C_WHITE);
  int w=108,h=62,idx=0;
  const char* labels[]={"EQ","FFT","ADV","BT PLAY","BT SET","SD PLAY","SD REC","FILES","WIFI","SLEEP","OTA","SYSTEM"};
  for(int r=0;r<3;r++) for(int c=0;c<4;c++){int x=14+c*116,y=44+r*88;thumbFrame(x,y,w,h,labels[idx]);idx++;}
}
