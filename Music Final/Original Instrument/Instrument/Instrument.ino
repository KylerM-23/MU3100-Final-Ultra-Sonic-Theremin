#include <Tone.h>
#include <math.h>

//define pin consts
#define SLEP 4 //Left Sensor Echo Pin
#define SLTP 5 //Left Sensor Trigger Pin
#define SREP 7 // Right Sensor Echo Pin
#define SRTP 6 //Right Sensor Trigger Pin

#define LINC A2 //Left INC
#define LUP A3 //Left Up/Down
#define LCS A5 //Right CS
#define RINC 10 // Right INC
#define RUP 9 //Right Up/Down
#define RCS A4 // Right CS

#define LBUT 3 //Left Function Switch Button
#define LFOOT A0 //Left Foot Pedal
#define RBUT 2 //Right Function Switch Sensor
#define RFOOT A1 //Right Foot Pedal

Tone ls; // Left Speaker
Tone rs; //Right Speaker

#define ls_pin 12
#define rs_pin 8

// defines variables
long duration; // variable for the duration of sound wave travel
double d1, d2;
int f1 = 120, f2 = 120;
double dmax = 35;
int p_mode = 0;
int Lmode = 0;
int Rmode = 0;
int volume = 40;
int v = 0;

int getData(int mode, double d,double dmax, double qmin, double qmax)
    {
    switch (mode)
    {
      case 0:
        return linearData(d,dmax, qmin, qmax);
        break;

      case 1:
        return logData(d,dmax, qmin, qmax);
        break;

      case 2:
        return expData(d,dmax, qmin, qmax);
        break;

      default:
        return 0;
      }
    }

int linearData(double d,double dmax, double qmin, double qmax)
    {
    int f = 0, A = (qmax-qmin)/(dmax-3);
    if (d<=3)
      {
      f = qmin;
      }
      else if (3<d<dmax)
      {
        f = (A * ((d-3)+qmin));
        }
      return f ;
    }

int logData(double d,double dmax, double qmin, double qmax)
    {
    int f = 0, A = (qmax-qmin)/log(dmax-2);
    if (d<=3)
      {
      f = qmin;
      }
      else if (3<d<dmax)
      {
        f = (A * log10((double)d-2)+qmin);
        }
      return f ;
    }

int expData(double d,double dmax, double qmin, double qmax)
    {
    int f = 0, A = (dmax-3)/log((qmax/qmin));
    if (d<=3)
      {
      f = qmin;
      }
      else if (3<d<dmax)
      {
        f = qmin*exp((d-3)/A);
        }
      return f ;
    }

double getDistance(int tpin, int epin)
    {
    digitalWrite(tpin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(tpin, HIGH);
    delayMicroseconds(10);
    digitalWrite(tpin, LOW);
    duration = pulseIn(epin, HIGH);
    return duration * 0.034 / 2;
    }

void changeR(int csp,int incp,int upp,bool way)
  {
    digitalWrite(csp, LOW);
    int x = LOW;
    x = way ? HIGH: LOW;
    digitalWrite(upp, x);
    digitalWrite(incp, HIGH);
    delayMicroseconds(5);
    digitalWrite(incp, LOW);
    digitalWrite(upp, LOW);
    return;
    }

void changeVolume(int current, int next, int csp,int incp,int upp)
{
 int n = next - current;
 bool way = n>0 ? true: false;
 n = abs(n);
  for (int i =0; i<n; i++)
  {
    changeR(csp,incp,upp,way);
    }
  }

void play_tone(Tone x, int f)
{
  if (f <=0)
  {
    x.stop();
    }

   else
   {
    x.play(f);
    }
  }

 void OneSpeaker()
    {      
    if(digitalRead(LBUT == HIGH))
    {
      Lmode = Lmode++ >1 ? 0: Lmode++;
      }

    if(digitalRead(RBUT == HIGH))
    {
      Rmode = Rmode++ >1 ? 0: Rmode++;
      }

    if(digitalRead(LFOOT == LOW))
    {
      d1 = getDistance(SLTP,SLEP);
      f1 = getData(Lmode,d1, dmax, 120,5000);
      f1 = f1 >5000 ? 0: f1; 
      }

    if(digitalRead(RBUT == LOW))
    {
      d2 = getDistance(SRTP,SREP);
       v = volume;
       volume = getData(Rmode,d2, dmax, 20,80);
       volume = volume >80 ? 20: volume; 
       changeVolume(v,volume,LCS, LINC, LUP);
      changeVolume(v,volume,RCS, RINC, RUP);
      }
     
    play_tone(ls,f1);
    play_tone(rs,f1);
    }
    
void TwoSpeaker()
    {      
    if(digitalRead(LBUT == HIGH))
    {
      Lmode = Lmode++ >1 ? 0: Lmode++;
      }

    if(digitalRead(RBUT == HIGH))
    {
      Rmode = Rmode++ >1 ? 0: Rmode++;
      }

    if(digitalRead(LFOOT == LOW))
    {
      d1 = getDistance(SLTP,SLEP);
      f1 = getData(Lmode,d1, dmax, 120,5000);
      f1 = f1 >5000 ? 0: f1; 
      }

    if(digitalRead(RBUT == LOW))
    {
      d2 = getDistance(SRTP,SREP);
      f2 = getData(Rmode,d2, dmax, 120,5000);
      f2 = f2 >5000 ? 0: f2; 
      }
     
    play_tone(ls,f1);
    play_tone(rs,f2);
    
    Serial.print("Freq1:");
    Serial.print(f1);
    Serial.print("\n");
   
    Serial.print("Freq2:");
    Serial.print(f2);
    Serial.print("\n");
    
    }

void setup() {
  //Sensor Pin Setup
  pinMode(SLTP, OUTPUT); 
  pinMode(SLEP, INPUT); 
  pinMode(SRTP, OUTPUT); 
  pinMode(SREP, INPUT);

  pinMode(LINC, OUTPUT); 
  pinMode(LUP, OUTPUT); 
  pinMode(RINC, OUTPUT); 
  pinMode(RUP, OUTPUT);

  pinMode(LBUT, INPUT); 
  pinMode(LFOOT, INPUT); 
  pinMode(RBUT, INPUT); 
  pinMode(RFOOT, INPUT);
  
  //Speaker Setup 
  ls.begin(ls_pin);
  rs.begin(rs_pin);

  changeVolume(v,volume,LCS, LINC, LUP);
  changeVolume(v,volume,RCS, RINC, RUP);
  Serial.begin(9600); 
  
}
    
void loop() 
{  
  switch (p_mode)
  {
    case 1:
      break;
    
    default:
      TwoSpeaker();
    }
  
  
}
