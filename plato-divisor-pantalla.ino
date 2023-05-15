/* Version 1, codigo con un arduino mega y 2.8" touch screen, el touch screen lo ordene de Amazon.com y  
es este "Elegoo UNO R3 2.8 Inches TFT Touch Screen with SD Card Socket w/ All Technical Data in CD for Arduino UNO R3"
el arduino mega es de la misma compania Elegoo.   Al LED verde le connecte un resistor de 220 ohms para que 
prenda verde y mas importante para que no se queme.   
 */


#include <Adafruit_GFX.h>    // Core graphics library -- vino con mi touch screen
#include <Adafruit_TFTLCD.h> // Hardware-specific library -- vino con mi touch screen per esta para download en el internet
#include <TouchScreen.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // or  Arduino's reset pin

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

//Detalles de el keypad 
#define BUTTON_X 60               // x start for keypad
#define BUTTON_Y 80              // y start for keypad
#define BUTTON_W 90              // button width
#define BUTTON_H 40
#define BUTTON_SPACING_X 12      // spacing between buttons
#define BUTTON_SPACING_Y 4
#define BUTTON_TEXTSIZE 2

// detalles de la caja de numeros del keypad
#define TEXT_X 50
#define TEXT_Y 5
#define TEXT_W 220
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR GREEN

#define YP A3  // Debe ser un analog pin, 
#define XM A2  // Debe ser un analog pin, , 
#define YM 9   // Debe ser un pin digital 
#define XP 8   // Debe ser un pin digital 

int goButton = 47;   //pin que es conectado al GO button - pulsador
                   // connecta un pulsador normal mente abierto entre pin "goButton" y gnd del arduino.
boolean lastButton = HIGH;       // estos variable son para la function antirebote.  
boolean currentButton = HIGH;


#define TS_MINX 144  //  estos son los minimo y maximo x y Y de tu touch screen
#define TS_MAXX 860

#define TS_MINY 126
#define TS_MAXY 888

#define MINPRESSURE 10   //  la pression que le puedes empujar al touch scrren
#define MAXPRESSURE 1000

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);  // para que sirva el touch screen
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

char currentPage;
//boolean buttonEnabled = true;
int X, Y;

int stepperMode; // 0 = divisions, 1 = pasitos, 2 = corre

#define CW HIGH              // defines direction
#define CCW LOW

int     num_divisions = 0;
boolean cur_dir;

boolean CWstatus;    // highlights CW button
boolean CCWstatus;    // CCW is not highlighted

long Num1,Num2,Number = 0;
boolean result = false;

#define motorSTEPpin   33    // output senal para pulsar el driver y mover el motor
#define motorDIRpin    31    // output senal para la direccion del motor
#define potAnalogPin   A15

volatile int counterPrev, counter = 0; //This variable will increase or decrease depending on the rotation of encoder

#define encoderA 18
#define encoderB 19

#define pulsesPerRevolution 600  // pulsos del encoder. 

#define fast 100    // Velocidad rapida de approach
#define slow 2000   // Velocidad lenta de correccion

#define approachTolerance 5

float theStart;
int theStop = 0;
float theRealStop;
float dif;

int cur_pos = 0;


void setup() {
    Serial.begin(9600);
    Serial.print("Starting...");
    tft.reset();
    pinMode (motorSTEPpin, OUTPUT);
    pinMode (motorDIRpin, OUTPUT);

    uint16_t identifier = 0x9341;   // espcifico para mi touch screen

    tft.begin(identifier);
    tft.setRotation(1);        // esta es la orientacion de la pantalla
    tft.fillScreen(BLACK);        // para borrar toda la pantalla

    pinMode(goButton,INPUT_PULLUP);  //estado es High (1), cuando oprimes el pulsador el estado cambia a low (0) connected to ground.  

    drawrotaryTablesub1();
    currentPage = '1';

    CWstatus = 1;    // highlights CW button
    CCWstatus = 0;    // CCW is not highlighted

    pinMode(encoderA, INPUT_PULLUP); 
    pinMode(encoderB, INPUT_PULLUP);

    //Setting up interrupt
    //A rising pulse from encodenren activated ai0().
    attachInterrupt(digitalPinToInterrupt(encoderA), ai0, RISING);
    //B rising pulse from encodenren activated ai1().
    attachInterrupt(digitalPinToInterrupt(encoderB), ai1, RISING);

    theRealStop = counter;
}


void loop() 
{ 
    if(currentPage =='1') 
    {
        TSPoint p = ts.getPoint();  //Get touch point
        
        p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);  /// orientation of x and y are potrait, see serial output
        p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

        if (p.z > ts.pressureThreshhold) 
        {  
            Serial.print("X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.print(p.y);
            Serial.print("\n");
            
            if(p.x>13 && p.x<84 && p.y>7 && p.y<229 ) // The user has pressed inside Divisiones button
            {
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                //Erase the screen
                tft.fillScreen(BLACK);
                currentPage = '2';
                stepperMode = 0;
                drawDivisiones();       //draws screen 2 with divisions as title 
                drawDirection();
            }
            
            if(p.x>120 && p.x<188 && p.y>9 && p.y<228 ) // The user has pressed inside Corre button
            {
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                //Erase the screen
                tft.fillScreen(BLACK);
                currentPage = '5';
                drawVelocidad();
                stepperMode = 2;
                drawDirection();
            }
        }
    }

    if (currentPage =='2') { 

        if (CWstatus ==1)
            cur_dir = CW;
        if (CCWstatus ==1)
            cur_dir = CCW;  

        currentButton = debounce(lastButton);

        if (lastButton == HIGH && currentButton == LOW)             // every time you push the button once
        {      
            if (stepperMode==0)   // if you are in the divisions menu
            {  
                dif = (pulsesPerRevolution * 2.00) / Number;

                if (cur_dir == CCW) {

                    theRealStop = theRealStop + dif;
                    theStop = round(theRealStop);
                    move_motor(theStop, CCW);
                
                } else if (cur_dir == CW) {

                    theRealStop = theRealStop - dif;
                    theStop = round(theRealStop);
                    move_motor(theStop, CW);

                }

                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                tft.setCursor(265, 170);                                  
                tft.setTextSize(3);
                tft.setTextColor(BLACK);  // clears the previous number
                tft.print(cur_pos);

                if (CWstatus == 1) cur_pos++;
                if (CCWstatus == 1) cur_pos--;
                if (cur_pos == Number || cur_pos == (-Number)) cur_pos = 0;
            
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                tft.setCursor(193, 170);   //display current position
                tft.setTextColor(YELLOW);
                tft.setTextSize(3);
                tft.print("Pos:");
                
                tft.setCursor(265, 170);                                  
                tft.setTextSize(3);
                tft.setTextColor(YELLOW);
                tft.print(cur_pos);
            }
                    
            if (stepperMode==2)   // if you are in the Run menu
            {}
        }        
        
        lastButton = currentButton;
        
        TSPoint p = ts.getPoint();  //Get touch point
        
        p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);  /// orientation of x and y are potrait, see serial output
        p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

        if (p.z > ts.pressureThreshhold) 
        {
            Serial.print("X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.print(p.y);
            Serial.print("\n");
            
            if(p.x>116 && p.x<193 && p.y>10 && p.y<129 && CCWstatus == 0 ) // The user has pressed inside CW+
            {
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);

                if (CWstatus == 1)  // if Clockwise is alrady highlighted then set zero and unhighlight when pressed
                {   
                    CWstatus =0;
                    drawCWunpressed();
                    delay(300);     
                }
                else if (CWstatus ==0) {
                    CWstatus = 1;
                    drawCWpressed();
                    delay(300);     
                }
            }
            
            if(p.x>218 && p.x<289 && p.y>10 && p.y<131 && CWstatus == 0) // The user has pressed inside CCW+
            {
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                if (CCWstatus == 1) { 
                    CCWstatus =0;
                    drawCCWunpressed();
                    delay(300);     
                }
                else if (CCWstatus ==0) {
                    CCWstatus = 1;
                    drawCCWpressed();
                    delay(300);     
                }
            }

            if(p.x>112 && p.x<196 && p.y>150 && p.y<229 ) // The user has pressed inside keypad
            {
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                currentPage = '6'; 
                tft.fillScreen(BLACK);
                drawKeypad(); 
                cur_pos = 0;                       // when keypad is pressed, current position and number of divisions is reset
                num_divisions = 0; 
            }

            if ((p.x>=302) && (p.x<=325) &&(p.y>=199) && (p.y<=239)) // user has pressed BACK BUTTON
            {
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                currentPage = '1'; // Indicates we are at home screen
                tft.fillScreen(BLACK);
                drawrotaryTablesub1(); // Draws #1 screen
            } 
        }
    }

    if(currentPage =='5')
    { 
        int potReading = analogRead(potAnalogPin);
        int runSpeed = map(potReading, 0, 1023, 500, 15000);

        if (CWstatus ==1)
            cur_dir = CW;
        if (CCWstatus ==1)
            cur_dir = CCW;  
        
        move_motorRun(runSpeed, cur_dir,currentPage);
            
            TSPoint p = ts.getPoint();  //Get touch point
        
        p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);  /// orientation of x and y are potrait, see serial output
        p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

        if (p.z > ts.pressureThreshhold) {
            Serial.print("X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.print(p.y);
            Serial.print("\n");
            
            if(p.x>116 && p.x<193 && p.y>10 && p.y<129 && CCWstatus == 0 ) // The user has pressed inside CW+
            {
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                if (CWstatus == 1) {    // if Clockwise is alrady highlighted then set zero and unhighlight when pressed
                    CWstatus =0;
                    drawCWunpressed();
                    delay(300);     
                } else if (CWstatus ==0) {
                    CWstatus = 1;
                    drawCWpressed();
                    delay(300);     
                }
            }
            
            if(p.x>218 && p.x<289 && p.y>10 && p.y<131 && CWstatus == 0) // The user has pressed inside CCW+
            {
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                if (CCWstatus == 1){ 
                    CCWstatus =0;
                    drawCCWunpressed();
                    delay(300);     
                }
                else if (CCWstatus ==0){
                    CCWstatus = 1;
                    drawCCWpressed();
                    delay(300);     
                }
            }

            if ((p.x>=302) && (p.x<=325) &&(p.y>=199) && (p.y<=239)) // user has pressed BACK BUTTON
                {
                    pinMode(XM, OUTPUT);
                    pinMode(YP, OUTPUT);
                    currentPage = '1'; // Indicates we are at home screen
                    tft.fillScreen(BLACK);
                    drawrotaryTablesub1(); // Draws #1 screen
                } 
        }
    }  
    
    if(currentPage =='6')           // keypad
    {
        TSPoint p = ts.getPoint();  //Get touch point
        
        p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);  /// orientation of x and y are potrait, see serial output
        p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);
        if (p.z > ts.pressureThreshhold) 
        {
            X = p.x;
            Y = p.y;
            DetectButtons();            // awaits inputs to keypad
            pinMode(XM, OUTPUT);
            pinMode(YP, OUTPUT);
            if(result == true)     //send has been pressed
                DisplayResult();  
            delay(300);    
        }
    }

    checkPos(theStop);
}


void drawrotaryTablesub1()
{
    //  "Divisiones menu button"
    tft.fillRect(20,20, 270, 50, ILI9341_ORANGE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRect(20,20,270,50,WHITE);
    tft.setCursor(70,30);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.print("Divisiones");

    //  "Pasitos menu button"
    tft.fillRect(20,90, 270, 50, ILI9341_ORANGE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRect(20,90,270,50,WHITE);
    tft.setCursor(90,105);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.print("Corre");

    //  "Back Button"
    tft.fillRoundRect(250,215, 50, 20,5, BLUE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRoundRect(250,215, 50, 20,5,WHITE);
    tft.setCursor(262,218);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("<-");
}


void drawDivisiones() 
{
      //  "Cuantas"
    tft.fillRect(20,20, 270, 50, ILI9341_ORANGE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRect(20,20,270,50,WHITE);
    tft.setCursor(38,35);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("Cuantas Divisiones?"); 

    //  display no frame:
    tft.drawRect(190,90,100,60,CYAN);

    //  "Back Button"
    tft.fillRoundRect(250,215, 50, 20,5, BLUE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRoundRect(250,215, 50, 20,5,WHITE);
    tft.setCursor(262,218);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("<-");
}


void drawPasitos ()
{
    //  "Cuantas"
    tft.fillRect(20,20, 270, 50, ILI9341_ORANGE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRect(20,20,270,50,WHITE);
    tft.setCursor(38,35);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("Cuantos pasos?");

    //  display no frame:
    tft.drawRect(190,90,100,60,CYAN);

    //  "Back Button"
    tft.fillRoundRect(250,215, 50, 20,5, BLUE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRoundRect(250,215, 50, 20,5,WHITE);
    tft.setCursor(262,218);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("<-");
}


void drawVelocidad ()
{
    //  "Cuantas"
    tft.fillRect(20,20, 270, 50, ILI9341_ORANGE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRect(20,20,270,50,WHITE);
    tft.setCursor(38,35);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.print("Que volocidad?");

    //  display no frame:
    tft.drawRect(190,90,100,60,CYAN);
    
    //  "Back Button"
    tft.fillRoundRect(250,215, 50, 20,5, BLUE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRoundRect(250,215, 50, 20,5,WHITE);
    tft.setCursor(262,218);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("<-");
}

void drawKeypad()
{
  //  esta es la funcion para dibujar el keypad- numeros
    Adafruit_GFX_Button buttons[12];
    char buttonlabels[12][4] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "Snd", "0", "Clr" };
    uint16_t buttoncolors[12] = {ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                                ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                                ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                                ILI9341_DARKGREEN, ILI9341_BLUE, ILI9341_RED};

    for (uint8_t row=0; row<4; row++) {
    for (uint8_t col=0; col<3; col++) {
      buttons[col + row*3].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                 BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row*3], ILI9341_WHITE,
                  buttonlabels[col + row*3], BUTTON_TEXTSIZE); 
      buttons[col + row*3].drawButton();
    }
  }
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);
}


void drawCWunpressed()
{
    tft.fillRect(20,90, 150, 50, ILI9341_ORANGE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRect(20,90,150,50,WHITE);
    tft.setCursor(30,105);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.print("CW +");
}


void drawCWpressed()
{
    tft.fillRect(20,90, 150, 50, BLUE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRect(20,90,150,50,WHITE);
    tft.setCursor(30,105);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.print("CW +");
}
  

void drawCCWunpressed()
{
    tft.fillRect(20,160, 150, 50, ILI9341_ORANGE);  // (x location, y location, widgth, height, color) or rectangle fill
    tft.drawRect(20,160,150,50,WHITE);
    tft.setCursor(30,175);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.print("CCW -");
}


void drawCCWpressed()
{
    tft.fillRect(20,160, 150, 50, BLUE);  // Turns CCW+ button blue
    tft.drawRect(20,160,150,50,WHITE);
    tft.setCursor(30,175);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.print("CCW -");
}


void drawDirection()
{
    if (CWstatus == 0)        // draws direction buttons CW or CCW with active in blue
       drawCWunpressed();

    if (CWstatus == 1)
    {
        drawCWpressed();
        cur_dir = CW;
    }

    if (CCWstatus ==0)
        drawCCWunpressed();

    if (CCWstatus == 1)
    {
        drawCCWpressed();
        cur_dir = CCW;
    }
}


void DetectButtons()              // esta funcion es para detectar si empujas un numero y guardar esa informacion
{
    result = true;
    if(Y<76&&Y>6)//DetectingButtonsonColumn1
    {
        if(X>268&&X<323)//             SEND Button pressed return to # screen and display result
        {
            Serial.println("ButtonSend");
            pinMode(XM, OUTPUT);
            pinMode(YP, OUTPUT);
            currentPage = '2'; // Indicates we are at home screen
            tft.fillScreen(BLACK);
            if (stepperMode == 0)   // divisiones
                drawDivisiones (); 
            drawDirection(); //draws the dir buttons and their status
            result = false;
            tft.setCursor(215,110);   //display result in no.2 screen
            tft.setTextSize(3);
            tft.setTextColor(GREEN);
            tft.println(Number);
            Serial.println(Number);
        }

        if(X>202&&X<259)//IfButton7ispressed
        {
            Serial.println("Button7");
            if(Number==0)
                Number=7;
            else
                Number=(Number*10)+7;//Pressedtwice
        }

        if(X>133&&X<193)//IfButton4ispressed
        {
            Serial.println("Button4");
            if(Number==0)
                Number=4;
            else
                Number=(Number*10)+4;//Pressedtwice
        }

        if(X>67&&X<127)//IfButton1ispressed
        {
            Serial.println("Button1");
            if(Number==0)
                Number=1;
            else
                Number=(Number*10)+1;//Pressedtwice
        }
    }
    if(Y>86&&Y<162)//DetectingButtonsonColumn2
    {
        if(X>268&&X<323)
        {
            Serial.println("Button0");//Button0isPressed
            if(Number==0)
                Number=0;
            else
                Number=(Number*10)+0;//Pressedtwice
        }

        if(X>202&&X<259)
        {
            Serial.println("Button8");
            if(Number==0)
                Number=8;
            else
                Number=(Number*10)+8;//Pressedtwice
        }

        if(X>133&&X<193)
        {
            Serial.println("Button5");
            if(Number==0)
                Number=5;
            else
                Number=(Number*10)+5;//Pressedtwic
        }

        if(X>67&&X<127)
        {
            Serial.println("Button2");
            if(Number==0)
                Number=2;
            else
                Number=(Number*10)+2;//Pressedtwic
        }
    }

    if(Y>174&&Y<245)//DetectingButtonsonColumn3
    {
        if(X>268&&X<323)
        {
            Serial.println("ButtonClear");
            Number=Num1=Num2=0;result=false;
            pinMode(XM, OUTPUT);
            pinMode(YP, OUTPUT);
            tft.fillRect(TEXT_X+10, TEXT_Y+10, TEXT_W-20, TEXT_H-20, BLACK); // clear the number display
        }

        if(X>202&&X<259)
        {
            Serial.println("Button9");
            if(Number==0)
                Number=9;
            else
                Number=(Number*10)+9;//Pressedtwice
        }
        if(X>133&&X<193)
        {
            Serial.println("Button6");
            if(Number==0)
                Number=6;
            else
                Number=(Number*10)+6;//Pressedtwice
        }
        if(X>67&&X<127)
        {
            Serial.println("Button3");
            if(Number==0)
                Number=3;
            else
                Number=(Number*10)+3;//Pressedtwice
        }
    }
}


void DisplayResult()
{
    tft.setCursor(80,20);
    tft.setTextSize(3);
    tft.setTextColor(WHITE);
    tft.println(Number);
    Serial.println(Number);
}


boolean debounce(boolean last)         // la funcion antirebote
{
    boolean current = digitalRead(goButton);
    if (last != current)
    {
        delay(5);
        current = digitalRead(goButton);
    }
    return current; 
}


 // esta funcion es para mover el motor con el 10K potentiometer. 
void move_motorRun(int velocidad, boolean dir, char Page)  // page = 5 for run mode
{
    digitalWrite(motorDIRpin, !dir);        // set Direction
  
    if (Page =='5')   
    {
        digitalWrite(motorSTEPpin,HIGH);  
        delayMicroseconds(velocidad);       // pulse the motor
        digitalWrite(motorSTEPpin,LOW);
        delayMicroseconds(velocidad);            // wait betweeen pulses
    }  
    
    return;  
}


void move_motor(int end, boolean dir)
{
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    approach(end, dir);
    correct(end);
    delay(500);
    return;  
} 


void approach(int end, boolean dir) {
  int pulsewidth = fast;
  int motorspeeddelay = fast;

  if (dir == CW) {
    digitalWrite(motorDIRpin, LOW);
    while (counter < (end - approachTolerance)) {
      // These four lines result in 1 step:
      digitalWrite(motorSTEPpin, HIGH);
      delayMicroseconds(pulsewidth);
      digitalWrite(motorSTEPpin, LOW);
      delayMicroseconds(motorspeeddelay);
    }

    digitalWrite(motorDIRpin, HIGH);
    while (counter > end) {
      // These four lines result in 1 step:
      digitalWrite(motorSTEPpin, HIGH);
      delayMicroseconds(pulsewidth);
      digitalWrite(motorSTEPpin, LOW);
      delayMicroseconds(motorspeeddelay);
    }
  } else if (dir == CCW) {
    digitalWrite(motorDIRpin, HIGH);
    while (counter > (end + approachTolerance)) {
      // These four lines result in 1 step:
      digitalWrite(motorSTEPpin, HIGH);
      delayMicroseconds(pulsewidth);
      digitalWrite(motorSTEPpin, LOW);
      delayMicroseconds(motorspeeddelay);
    }

    digitalWrite(motorDIRpin, LOW);
    while (counter < end) {
      // These four lines result in 1 step:
      digitalWrite(motorSTEPpin, HIGH);
      delayMicroseconds(pulsewidth);
      digitalWrite(motorSTEPpin, LOW);
      delayMicroseconds(motorspeeddelay);
    }
  }

}


void correct(int end) {
  int pulsewidth = slow;
  int motorspeeddelay = slow;

  while ((counter < (end)) || (counter > (end))) {
    digitalWrite(motorDIRpin, LOW);
    while (counter < end) {
      // These four lines result in 1 step:
      digitalWrite(motorSTEPpin, HIGH);
      delayMicroseconds(pulsewidth);
      digitalWrite(motorSTEPpin, LOW);
      delayMicroseconds(motorspeeddelay);
    }

    digitalWrite(motorDIRpin, HIGH);
    while(counter > end) {
      // These four lines result in 1 step:
      digitalWrite(motorSTEPpin, HIGH);
      delayMicroseconds(pulsewidth);
      digitalWrite(motorSTEPpin, LOW);
      delayMicroseconds(motorspeeddelay);
    }
  }
}


void checkPos(int target) {
  if ((target < (counter)) || (target > (counter)))  {
    correct(target);
  }
}


void ai0() {
  // ai0 is activated if encoderA is going from LOW to HIGH
  // Check encoderB to determine the direction
  if (digitalRead(encoderB)==LOW) {
    counter++;
  } else {
    counter--;
  }
}
   

void ai1() {
  // ai0 is activated if encoderB is going from LOW to HIGH
  // Check with encoderA to determine the direction
  if (digitalRead(encoderA)==LOW) {
    counter--;
  } else {
    counter++;
  }
}