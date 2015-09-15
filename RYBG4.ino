
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Time.h"

//#include "Adafruit_WS2801.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
#define bufferSize 20
#define ledBuffSize 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

/*
uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 4;
Adafruit_WS2801 strip = Adafruit_WS2801((uint16_t)5, (uint16_t)5, dataPin, clockPin);
*/


char *dows2[] = {"Sunday", "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
bool done = false, serialMode = false;
int bufferPos = 0;
unsigned long NextMillis = 0UL, Millis12 = 0UL;
//     aaaaa
//    f     b
//    f     b
//     ggggg
//    e     c
//    e     c
//     ddddd
            //    abcdefg.  abcdefg.  abcdefg.  abcdefg.  abcdefg.  abcdefg. 
byte digits[] = {B11111100,B01100000,B11011010,B11110010,B01100110,B10110110,
//                  0         1          2        3         4          5
//               abcdefg.  abcdefg.  abcdefg.  abcdefg.  
                B10111110,B11100000,B11111110,B11110110 };
                //  6         7         8        9
                
char leds[ledBuffSize];
char leds2[ledBuffSize];

int ledGreen = 3;
int ledYellow = 5;
int ledRed = 6;
int ledHeartbeat = 8;
unsigned long heartbeat1 = 0UL;
//unsigned long heartbeat2 = 0;
bool heartbeat = false;

void setup() {
  //strip.begin();
  SetRow(4,15);
  //strip.show();
  heartbeat = false;
  heartbeat1 = millis()+1000UL;
  //heartbeat2 = millis()+1100UL;
  pinMode(ledHeartbeat,OUTPUT);
  digitalWrite(ledHeartbeat,HIGH);
  delay(100);
  digitalWrite(ledHeartbeat,LOW);
  pinMode(ledGreen,OUTPUT); pinMode(ledYellow,OUTPUT); pinMode(ledRed,OUTPUT);
  strcpy(leds2,"88:88:88");
  NextMillis = 0UL;
  Millis12 = millis()+12*60*1000UL;
  bufferPos = 0;
  done = false; serialMode = false;
  Serial.begin(9600);
  // put your setup code here, to run once:
  tft.begin();

  testFillScreen();
  fillCircles(7,ILI9341_WHITE);
  //tft.drawPixel(0,0,ILI9341_BLUE);
  
  tft.setCursor(1,tft.height()-24);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(3);
  tft.println("Heimdall"); 
  Serial.println("Setup complete.");
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0,280); 
  tft.print("NoBomma Countdown Clock!"); 
}
unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(ILI9341_BLACK); draw7();
  tft.fillScreen(ILI9341_RED); draw7();
  tft.fillScreen(ILI9341_GREEN); draw7();
  tft.fillScreen(ILI9341_BLUE); draw7();
  tft.fillScreen(ILI9341_BLACK); draw7();
  return micros() - start;
}

void draw7() {
  int w = 9;
  int h = 20;
  for(int i = 0; i<=9; i++ ) {
    Draw7SegementDigit(i*(w+6),205,w,h,i,ILI9341_WHITE);
  }
  Draw7SegementDigits(0,235,(w+6)*5,h,"23:45",ILI9341_WHITE,false);
  delay(1000);
}


char buffer[bufferSize];
char buffer2[bufferSize];

int borderThick = 5;

void fillCircles(int bits,uint16_t borderColor) {
  if(   (bits & 3)!=0 ) {
    digitalWrite(ledHeartbeat,HIGH);
    delay(100);
    digitalWrite(ledHeartbeat,LOW);
  }

  //digitalWrite(ledGreen,LOW); digitalWrite(ledYellow,LOW); digitalWrite(ledRed,LOW);
  //analogWrite(ledGreen,25);  analogWrite(ledYellow,2);  analogWrite(ledRed,2);
  analogWrite(ledGreen,0);  analogWrite(ledYellow,0);  analogWrite(ledRed,0);
  /*
  for(int i=0;i<3;i++) {
    strip.setPixelColor(0,i,0,0,0);
  }
  strip.show();*/
  //tft.fillRect(0,0,7,7,ILI9341_BLACK);
  int w = tft.width();
  int h = tft.height();
  int x1,y1,r,x2,y2,x3,y3;
  x1 = w/2;
  
  y1 = h/12;
  r = h/12-1;
  x1=w-r-2;
  if((bits & 1)!=0) {
    tft.fillCircle(x1,y1,r,ILI9341_RED);
    //tft.fillRect(0,0,7,7,ILI9341_RED);
    //digitalWrite(ledRed,HIGH);
    analogWrite(ledRed,250);
    //strip.setPixelColor(0,0,255,0,0);
  }
  else
    tft.fillCircle(x1,y1,r,ILI9341_BLACK);
  for(int j=0;j<borderThick;j++)
    tft.drawCircle(x1,y1,r-j,borderColor);
  //tft.drawCircle(x1,y1,r-1,borderColor);
  x2=x1;
  x3=x1;
  y2=h/12+h/6;
  y3=h-y1;
  y3=h/12+h/3;
  if((bits & 2)!=0) {
    tft.fillCircle(x2,y2,r,ILI9341_YELLOW);
    //tft.fillRect(0,0,7,7,ILI9341_YELLOW);
    //digitalWrite(ledYellow,HIGH);
    analogWrite(ledYellow,250);
    //strip.setPixelColor(0,1,50,50,0);
  }
  else
    tft.fillCircle(x2,y2,r,ILI9341_BLACK);
  for(int j=0;j<borderThick;j++)
    tft.drawCircle(x2,y2,r-j,borderColor);
  //tft.drawCircle(x2,y2,r-1,borderColor);
  if((bits & 4)!=0){
    //tft.fillRect(0,0,7,7,ILI9341_GREEN);
    tft.fillCircle(x3,y3,r,ILI9341_GREEN);
    //digitalWrite(ledGreen,HIGH);
    analogWrite(ledGreen,250);
    //strip.setPixelColor(0,2,0,50,0);
  }
  else
    tft.fillCircle(x3,y3,r,ILI9341_BLACK);
  for(int j=0; j<borderThick; j++)
    tft.drawCircle(x3,y3,r-j,borderColor);
  //tft.drawCircle(x3,y3,r-1,borderColor);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(0,10);
  tft.setTextColor(borderColor);
  tft.setTextSize(1);
  tft.println("HFD Replication Status"); 

  //strip.show();
}
int testBits = 1;
int repeatCount = 0;

bool buffer2date = false;

int SetLightFromBuffer() {
  char ch;
  int r = 0;
  for(int i=0; i<bufferSize; i++) {
    ch = buffer[i];
    if(ch<=0)
      break;
    if(ch=='\n' || ch=='\r')
      break;

    if(ch=='R' || ch=='r')
      r = r | 1;
    if(ch=='Y' || ch=='y')
      r = r | 2;
    if(ch=='g' || ch=='G') {
      r = r | 4;
    }
    if( bufferPos == 13) {
      // 012345678901
      // YYMMDDHHMMSS  
      // 150525163155
      //setTime(hours, minutes, seconds, days, months, years);
      int hours = GetInt(6,2);
      int minutes = GetInt(8,2);
      int seconds = GetInt(10,2);
      int days = GetInt(4,2);
      int months = GetInt(2,2);
      int years = GetInt(0,2);
      if(years>0) {
        //setTime(GetInt(6,2),GetInt(8,2),GetInt(10,2),GetInt(4,2),GetInt(2,2),GetInt(0,2) );
        buffer[12] = 0;
        setTime(hours, minutes, seconds, days, months, years);
        //Serial.println(buffer);
        sprintf(buffer2,"%02d:%02d:%02d %d/%d/%d",hours, minutes, seconds, months, days, years);
        //Serial.println(buffer2);
        
        buffer[0]=0;
        buffer[1]=0;
        bufferPos = 0;
        buffer2date = false;
        r=-1;
        }
    }
  } 
  
  done = false;
  bufferPos = 0;
  return r;
}

int GetInt(int f, int l ) {
  int r=0;
  for(int i=0;i<l;i++) {
    r *= 10;
    r += buffer[i+f]-'0';
  }
  return r;
}

// The row is 0 through 4 and bits are RYBG but LEDs show GBYR.
void SetRow( int row, int bits ) {
  int r,g,b;
  int bit = 1;
  bool changed = false;
  for(int x = 0; x<4; x++) {
    //strip.setPixelColor(x,row,0,0,0);
    if( (bits & bit ) != 0 ) {
      switch(x) {
        case 0: // green
          g=50; r=0; b=0;
          break;
          
        case 1: // blue
          r=0; g=0; b=50;
          break;
          
        case 2: // yel
          r=50; g=50; b=0;
          break;
          
        case 3: // red
          r=50; g=0; b=0;
          break;
      }
      //strip.setPixelColor(x,row,r,g,b);
      changed = true;
    }
    bit = bit << 1;
  }
  /*
  if(changed)
    strip.show();*/
}


time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss  )
{
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet);         //convert to time_t
}


int bounce = 0;
int lastMinutes = -1;
unsigned long _lastMinutes = 0UL;
int nobommaY = 170;
int smallDateTimeY = 40;
bool WhiteLED = false;
int LastBits = 0;

void loop() {
  if(millis()>heartbeat1) {
    heartbeat1 = millis()+550UL;
    /*
    for(int i=0;i<5; i++)
      strip.setPixelColor(4,i,0,0,0);
    strip.setPixelColor(4,bounce%5,10,10,10);
    strip.show();*/
    bounce++;
    if(bounce>100)
      SetRow(4,0);
    if(bounce<0)
      bounce=0;
  }
  /*
  if(millis()>heartbeat1) {
    if(heartbeat) {
      digitalWrite(ledHeartbeat,HIGH);
      //heartbeat2 = millis()+500UL;
      heartbeat1 = millis()+100UL;
    }
    else {
      digitalWrite(ledHeartbeat,LOW);
      heartbeat1 = millis()+1000UL;
    }
    heartbeat = !heartbeat;
  } 
  else if(millis()>heartbeat1) {
    heartbeat1 = millis()+10000UL;
    digitalWrite(ledHeartbeat,LOW);
    heartbeat2 = millis()+1000UL;
  }
  */
  int bits;
  char chr = ' ';
  if(Serial.available()>0) {
    serialMode = true;
    chr = Serial.read();
    if(chr<0)
      done = true;
    buffer[bufferPos] = chr;
    
    bufferPos++;    
  }
  // put your main code here, to run repeatedly:
  /*
  if(repeatCount==0) {
    delay(3000);
    tft.fillScreen(ILI9341_BLUE);
    //fillCircles(7,ILI9341_WHITE);
    delay(3000);
    //tft.fillScreen(ILI9341_WHITE);
  } else {
    //fillCircles(0,ILI9341_BLACK);
  }*/
  
  
  
  if(chr=='\n')
    done = true;
  if(chr=='\r')
    done = true;
  if(bufferPos>=bufferSize)
    done = true;
  
  if(done) {
    bits = SetLightFromBuffer();
    LastBits = bits & 0xFF;
    if(bits>=0) {
      SetRow(3,CorrectBits(bits));
      Millis12 = millis()+12*60*1000UL;
      fillCircles(bits,ILI9341_WHITE);
      // Set last sync time
      tft.fillRect(0,205,150,24,ILI9341_BLACK);
      sprintf(leds,"%2d:%02d:%02d",hour(), minute(),second());
      Draw7SegementDigits(10,205,tft.width()/2,20,leds,ILI9341_WHITE,false);
      
      tft.fillRect(0,235,tft.width()/2,25,ILI9341_BLACK);
    }
  } else {
    if(!serialMode) {
      fillCircles(testBits,ILI9341_WHITE);
      delay(3000);
      testBits = testBits << 1;
      if(testBits>8)
        testBits = 1;
        
      repeatCount++;
    }
  }
  if(timeStatus()!=timeNotSet) {
    int w = tft.width();
    if(millis()>Millis12) {
      Millis12 = millis()+60*1000UL;
      tft.fillRect(0,235,w/2,25,ILI9341_BLACK);
      tft.setCursor(0,235);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_RED);
      tft.print("Time-out!!");
      fillCircles(0,ILI9341_WHITE);
    }
    if(millis()>NextMillis) {
      WhiteLED = !WhiteLED;
      NextMillis = millis()+333UL;
      
      tft.setCursor(10,50);
      tft.setTextSize(1);
      /*
      if(buffer2date) {
        tft.setTextColor(ILI9341_BLACK);
        buffer2[8]=0;
        tft.println(buffer2);
      } 
      else
        tft.fillRect(10,50,w/2,25,ILI9341_BLACK);    */
      if(lastMinutes==minute()) {
        tft.fillRect(45,smallDateTimeY,20,10,ILI9341_BLACK);
      }
      else {
        tft.fillRect(0,smallDateTimeY,w/2,10,ILI9341_BLACK);
        lastMinutes = minute();
      }
      if(hour()==0 && minute()==0 && second()==0)
        tft.fillRect(10,smallDateTimeY,w/2,120,ILI9341_BLACK);
      
      sprintf(buffer2,"%02d:%02d:%02d %d/%d/%d",hour(), minute(), second(), month(), day(), year());
      //Serial.println(buffer2);
      buffer2date=true;
      tft.setCursor(10,smallDateTimeY);
      tft.setTextColor(ILI9341_WHITE);      
      tft.println(buffer2);    
    
      time_t NoBomma = tmConvert_t(2016,11,8,7,0,0);
      time_t t = now();
      unsigned long seconds = NoBomma-t;
      unsigned long _days = seconds / ( 60UL * 60UL * 24UL );
      unsigned long _seconds = seconds % 60UL;
      unsigned long _minutes = ( seconds / 60UL ) % 60UL ;
      unsigned long _hours = ( seconds / 3600UL ) % 24UL;
      TimeElements tm;
      breakTime(t, tm);  
      tft.setCursor(10,140);
      tft.setTextSize(2);
      int dow = tm.Wday-1;
      if(dow>=0 && dow<=6) {
        int dowl = 9 - strlen(dows2[dow]);
        if(dowl>0 && dowl<6) {
          for(int i=0 ; i<dowl; i++) {
            tft.print(" ");
          }
        }
        tft.println(dows2[dow]);
      }
      else
        tft.println("dow error.");
        
      if(_lastMinutes==_minutes) {
        tft.fillRect(tft.width()/2,nobommaY,40,20,ILI9341_BLACK);
      }
      else {
        tft.fillRect(0,nobommaY,tft.width(),20,ILI9341_BLACK);
        //lastMinutes=minute();
        _lastMinutes = _minutes;
      }
      //tft.show();
      tft.setCursor(10,nobommaY);
      sprintf(buffer2, "%3lu %02lu:%02lu:%02lu", _days, _hours, _minutes, _seconds );
      tft.print(buffer2);
      tft.setTextSize(1);
      sprintf(leds,"%2d-%02d-%02d",month(), day(), year() % 100);
      if(month()<10 && day()<10)
        sprintf(leds,"%d-%d-%d",month(),day(),year());
      Draw7SegementDigits(10,110,tft.width()/2,20,leds,ILI9341_WHITE,false);      

      sprintf(leds,"%2d:%02d:%02d",hour(), minute(),second());
      Draw7SegementDigits(10,60,tft.width()*0.75,40,leds,ILI9341_WHITE,true);
      //lastMinutes = minute();
      if(WhiteLED) {
        if( ( LastBits & 0xF8 ) == 0 ) {
          if( (LastBits & 1) != 0 ) {
            digitalWrite(ledHeartbeat,HIGH);
Serial.print(bits);
Serial.print("===");            
Serial.println(LastBits);
          }
          else
            digitalWrite(ledHeartbeat,LOW);
        }
        else
          digitalWrite(ledHeartbeat,LOW);
      }
      else
        digitalWrite(ledHeartbeat,LOW);
    }
  }
  //lastMinutes = minute();
}

// GYR ----> RYBG
int CorrectBits( int bits ) {
  int r = 0;
  if(bits>=0) {
    if((bits & 4)!=0)
      r+=1;
    if((bits & 2) != 0)
      r+=4;
    if((bits & 1) != 0 )
      r+=8;
  }
  return r; 
}
/*
void Draw7SegementDigits2 ( int16_t x, int16_t y, int16_t w, int16_t h, char *s, int16_t c, bool ClearFirst ) {
  for(int i=0; i<2; i++)
    Draw7SegementDigits( x+i, y+i, w, h, s, c, ClearFirst );
}
*/

void Draw7SegementDigits( int16_t x, int16_t y, int16_t w, int16_t h, char *s, int16_t c, bool ClearFirst ) {
  int CharLength = strlen(s);
  int w1 = w/CharLength;
  int bdr = 7;
  if(CharLength>0 && CharLength<=10) {
    for(int i=0;i<CharLength;i++) {
      char ch = s[i];
      ch = toupper( ch );
      if( ch>='A' && ch<='Z' ) {
      }
      else if( ch>='0' && ch<='9' ) {
        if(ch!=leds2[i] && ClearFirst ) {
          ch = leds2[i];
          ch = ch-'0';
          Draw7SegementDigit2( x+i*w1, y, w1-bdr, h, ch, ILI9341_BLACK );
          ch = s[i];
          ch = ch-'0';
          Draw7SegementDigit2( x+i*w1, y, w1-bdr, h, ch, c );
        }
        else {
          ch = ch-'0';
          Draw7SegementDigit2( x+i*w1, y, w1-bdr, h, ch, c );
        }
      } else {
        if(ch==':') {
          if(ch!=leds2[i]  && ClearFirst ) {
            ch = 10;
            Draw7SegementDigit2( x+i*w1, y, w1-bdr, h, leds2[i], ILI9341_BLACK);
          }
          else {
            ch = 10;
            Draw7SegementDigit2( x+i*w1, y, w1-bdr, h, ch, c );
          }
        }
        if(ch=='-') {
          ch = 11;
          Draw7SegementDigit2( x+i*w1, y, w1-bdr, h, ch, c );
        }
        if(ch==' ') {
          tft.fillRect(x+i*w1,y,w1,h,ILI9341_BLACK);
        }
      }
    }
    if(ClearFirst)
      strcpy(leds2,s);
  }
}

void Draw7SegementDigit2( int16_t x, int16_t y, int16_t w, int16_t h, byte v, int16_t c ) {
  for(int i=0; i<4; i++) 
    Draw7SegementDigit( x+i, y+i, w, h, v, c );
}

void Draw7SegementDigit( int16_t x, int16_t y, int16_t w, int16_t h, byte v, int16_t c ) {
  int h2 = h/2;
  if(v==11) {  /// Handle the dash -
    tft.drawLine(x,y+h2,x+w,y+h2,c);
  }
  else if(v==10) {  // Handle the colon.
    int w2 = w/2;
    int r = w/4;
    tft.fillCircle(x+w2,y+h2-r,r,c);
    tft.fillCircle(x+w2,y+h-r,r,c);
  } else {
    byte bits = digits[v];
    for(int i=0;i<7;i++) {
      if((bits & 128) != 0 ) {  
  //     aaaaa
  //    f     b
  //    f     b
  //     ggggg
  //    e     c
  //    e     c
  //     ddddd
        switch(i) {
          case 0: // a
            tft.drawLine(x,y,x+w,y,c);
            break;
          case 1: // b
            tft.drawLine(x+w,y,x+w,y+h2,c);
            break;
          case 2: // c
            tft.drawLine(x+w,y+h2,x+w,y+h,c);
            break;
          case 3: // d
            tft.drawLine(x,y+h,x+w,y+h,c);
            break;
          case 4: // e
            tft.drawLine(x,y+h2,x,y+h,c);
            break;
          case 5: // f
            tft.drawLine(x,y,x,y+h2,c);
            break;
          case 6: // g
            tft.drawLine(x,y+h2,x+w,y+h2,c);
            break;        
        }
      }      
      bits = bits << 1;
    }
  }
}
