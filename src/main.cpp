#include <Arduino.h>
#include <DHT.h>
#include <aREST.h>
#include <avr/wdt.h>
#include <TimedAction.h>

const int pin_dht = 9;
const int pin_led = 9;
const int timer_dht = 2000;

void update_params();
int ledControl(String command);

TimedAction timed_action_DHT = TimedAction(timer_dht, update_params);

aREST rest = aREST();
DHT dht(pin_dht, DHT11);

float temperature;
float humidity;

void setup(void)
{
  dht.begin();
  pinMode(6, OUTPUT);

  Serial.begin(115200);

  rest.variable("temperature", &temperature);
  rest.variable("humidity", &humidity);

  rest.function("led", ledControl);

  rest.set_id("2");
  rest.set_name("serial");
  rest.set_end_of_comunication_character(127);
  rest.enable_force_send_http_headers();

  wdt_enable(WDTO_4S);
}

void loop()
{
  rest.handle(Serial);
  timed_action_DHT.check();
  wdt_reset();
}

int ledControl(String command)
{
  int state = command.toInt();

  digitalWrite(pin_led, state);
  return 1;
}

void update_params()
{
  float temperature_test = dht.readTemperature();
  temperature = isnan(temperature_test) ? 0 : temperature_test;

  float humidity_test = dht.readHumidity();
  humidity = isnan(humidity_test) ? 0 : humidity_test;
}