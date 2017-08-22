#define SEALEVELPRESSURE_HPA (1013.25)


const uint8_t buzzer_pin = D3;

const uint8_t test_pin = D4;

const uint8_t red_led_pin = D7;
const uint8_t green_led_pin = D6;
const uint8_t blue_led_pin = D5;


volatile bool led_ok;
volatile bool led_on;

volatile bool send_info_ok;


const int update_period = 500; // 500 ms

void update();
void ledControl();
void sendInfo();
