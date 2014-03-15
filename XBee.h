// Header file for XBee helper functions
// Josh Dugan, Stephen Shinn, Matt Moravec

#include <pthread.h>

// Function prototypes
int receiveIntXbee();
int sendIntXbee(int value);
int sendShortXbee(short value);
short receiveShortXbee();
int initializeUARTS();
int initializeXbee();
int sendInt(int value);
char* iota(int i, char b[]);
void* updatePWM(void* args);
int setPWM(double dutyCycle, int toggle, int Dir, int activePin);


typedef struct PWMThread{
	pthread_t tid;
	double dutyCycle;
	int Dir;
	int toggle;
	int activePin;
} PWMThread;

typedef struct UARTThread{
	pthread_t tid;
	char c;
} UARTThread;


//UART Settings
#define BAUDRATE_UART2 B9600
#define BAUDRATE_UART4 B9600
#define MAX_SERIAL_BUFFER_SIZE 7
/* change this definition for the correct port */
#define MODEMDEVICE_UART2 "/dev/ttyO2"
#define MODEMDEVICE_UART4 "/dev/ttyO4"
#define MODEMDEVICE_I2C "/dev/i2c-1"

#define FREQ_KHZ				1


#define UART_SELECT_0_GPIO_PIN_NUM 		44		//P8_11
#define UART_SELECT_1_GPIO_PIN_NUM 		45		//P8_12
#define UART_SELECT_2_GPIO_PIN_NUM 		46		//P8_14
#define UART_SELECT_3_GPIO_PIN_NUM              47              //P8_15
#define LED_PIN					116		//P9_41
