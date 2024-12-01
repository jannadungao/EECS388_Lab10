#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"
#define SERVO_PULSE_MAX 2400
#define SERVO_PULSE_MIN 544
#define SERVO_PERIOD 20000


void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document
    // Setup
   int red_gpio = RED_LED;
   int green_gpio = GREEN_LED;
   uint16_t dist = 0;
    // read data
    if ('Y' == ser_read(0) && 'Y' == ser_read(0)) {
        uint8_t lower_bit = ser_read(0); // read byte 3
        uint8_t upper_bit = ser_read(0); // read byte 4
        dist = (upper_bit << 4) + lower_bit;
    }
    // action/led
    if (dist > 200) {
        gpio_write(green_gpio, ON);
        gpio_write(red_gpio, OFF);
    } else if (dist <= 200 && dist > 100) {
        gpio_write(green_gpio, ON);
        gpio_write(red_gpio, ON);
    } else if (dist <= 100 && dist > 60) {
        gpio_write(green_gpio, OFF);
        gpio_write(red_gpio, ON);
    } else if (dist <= 60 ) {
        gpio_write(red_gpio, ON);
        gpio_write(green_gpio, OFF);
        delay(50);
        gpio_write(red_gpio, OFF);
        delay(50);
    }
   
}

int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.
    char data[5];
    int angle;
    while (ser_isready(1)){    // devid is 1 because that's the raspberry pi
        ser_readline(1, 5, data);
        sscanf(data, "%d", &angle);
        return angle;
    }
    return -1000;
}

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task
    int change = (SERVO_PULSE_MAX - SERVO_PULSE_MIN) / 180;
    int delay = SERVO_PULSE_MIN + (change*pos);
    gpio_write(gpio, ON);
    delay_usec(delay);
    gpio_write(gpio, OFF);
    delay = SERVO_PERIOD - delay; 
    delay_usec(delay);
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");
    int angle = 0;
    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int new_angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        if (new_angle != -1000) {
            angle = new_angle;
        }
        printf("\nangle=%d", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            /*if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }*/
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            steering(gpio, angle);
        }

    }
    return 0;
}
