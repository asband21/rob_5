
int moter_a[] = {9,10};
int tel = 0;
int cp_has = 10;
bool brems = false;
int ret = 1;

void setup() {
  // Initialize digital pin 3 as an output.
  pinMode(moter_a[0], OUTPUT);
  pinMode(moter_a[1], OUTPUT);
  digitalWrite(moter_a[0], LOW);
  digitalWrite(moter_a[1], LOW);
  Serial.begin(9600);
  Serial.print("staeter");

}
void koer(int hasj) // 0 er brams
{
  int has = hasj;
  if(has == 0)
  {
    if(brems)
    {
      digitalWrite(moter_a[0], HIGH);
      digitalWrite(moter_a[1], HIGH);
    }
    else
    {
      digitalWrite(moter_a[0], LOW);
      digitalWrite(moter_a[1], LOW);
    }
  }
  else if(has > 0)
  {
    has--;
    digitalWrite(moter_a[0], LOW);
    if(has > 255)
      analogWrite(moter_a[1], 255);
    else
      analogWrite(moter_a[1], has);
  }
  else
  {
    has = 0 - has;
    has--;
    digitalWrite(moter_a[1], LOW);
    if(has > 255)
      analogWrite(moter_a[0], 255);
    else
      analogWrite(moter_a[0], has);
  }
}

void frem_og_tilbage()
{
  //Serial.println(tel);
  tel += ret;
  koer(0);
  if(tel > 300 || tel < -300)
    ret = -1*ret;
  
}

void pc_has()
{
  if(Serial.available() > 0)
  {
    cp_has = Serial.parseInt();
    Serial.println(cp_has);
  }
  koer(cp_has);
}
void loop()
{
  //frem_og_tilbage();
  pc_has();
}
