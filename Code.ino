const int mqxPin = A1;
int buzzer = 5;
int sensorThres = 450;

#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(2,3,4); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void setup()
{
    pinMode(mqxPin, INPUT);
    pinMode(buzzer, OUTPUT);
    Serial.begin(9600);
    Rtc.Begin();
}

void loop()
{
    int analogSensor = analogRead(mqxPin);
    char rokok[20];

    getdate(rokok); // Panggil fungsi getdate dengan parameter rokok
    Serial.print("Output MQ-2 : ");
    Serial.println(analogSensor);

    if (analogSensor > sensorThres)
    {
        tone(buzzer, 1000, 200);
        Serial.println(rokok);
    }
    else
    {
        noTone(buzzer);
    }
    delay(1000); // Delay 1 second for next reading
}

// Mengubah fungsi getdate agar dapat mengisi parameter rokok dengan nilai waktu
void getdate(char *rokok)
{
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now, rokok); // Menambahkan parameter rokok
}

void printDateTime(const RtcDateTime &dt, char *rokok)
{
    snprintf_P(rokok,
               20,
               PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
               dt.Month(),
               dt.Day(),
               dt.Year(),
               dt.Hour(),
               dt.Minute(),
               dt.Second());
    Serial.print("Rokok Time: ");
    Serial.println(rokok);
}

void arg (){
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  if (!Rtc.IsDateTimeValid()) 
    {
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Rtc.SetDateTime(compiled);
    }
}
