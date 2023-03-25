#include <Servo.h>
Servo servo_carb;

int Relay_fuel = 5; 
int Relay_starter = 6;
unsigned long lastflash;
int RPM; 
int start_count = 0;

void setup()
{
 pinMode(Relay_fuel, OUTPUT);
 pinMode(Relay_starter, OUTPUT);
 digitalWrite(Relay_starter,HIGH);
  servo_carb.attach(4); //підключаємо сервопривід карбюратораз
  servo_carb.write(0); //задаємо початкове положення збагачувача карбюратора як повністю відкритий
  Serial.begin(9600); //відкриваємо послідовний порт
	attachInterrupt(0,sens,RISING); //підключаємо переривання
}

void sens() {
	RPM=60/((float)(micros()-lastflash)/1000000);  //розрахунок обертів двигуна
	lastflash=micros();  //запам'ятати час останнього обороту
}

void loop() {
if ((micros()-lastflash)>2000000){ //якщо сигналу обертів немає більше 2 секунд
		RPM=0;  //вважаємо що вони дорівнюють 0
}
if (RPM == 0 && start_count == 3) { //цикл помилки запуска генератора. якщо оберти двигуна дорівнюють 0 та спроби запуску використані - виконуємо цикл
    digitalWrite(Relay_fuel, HIGH); //реле перекриває подачу палива та вимикая запалювання генератора.
}
if (RPM == 0 && start_count < 3) { //цикл запуску двигуна (Коли оберти двигуна дорівнюють 0 та спроби запуску менше 3)
    delay(3000); //затримка перед закриттям збагачувача карбюратора
    servo_carb.write(180); //закриттям збагачувача карбюратора сервоприводом
    delay(1000); //затримка перед включенням стартера
    digitalWrite(Relay_starter, LOW); //реле 2 запускає стартер
    delay(2000); //час роботи стартера
    digitalWrite(Relay_starter, HIGH); //реле 2 зупиняє стартер
    delay(3000); //затримка на стабілізацію обертів двигуна або його зупинку після невдалого запуску
    start_count++; //збільшуємо лічильник спроб на 1
}
int current_position = servo_carb.read(); //зчитуємо поточне положення сервоприводу який керує збагачувачем
if (RPM > 100 && current_position > 0) {  //якщо збагачувач не відкритий повністю, а двигур працює то виконуємо цикл відприття
      delay(1000); //затримка перед відкриттям на половину
      servo_carb.write(90); //відкриття на половину
      delay(5000); //затримка на прогрів двигуна перед повним відкриттям
      servo_carb.write(0); //повне відкриття збагачувача      
}

  //Serial.println(RPM);
  delay(200);

}