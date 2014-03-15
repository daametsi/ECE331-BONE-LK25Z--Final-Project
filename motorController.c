#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <curses.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <poll.h>
#include <signal.h>
#include "SimpleGPIO.h"
#include "XBee.h"

 /****************************************************************
 * Constants
 ****************************************************************/
 
#undef DEBUG
#define GPIO_DIR "/sys/class/gpio"
#define AIN_DIR "/sys/devices/platform/omap/tsc"

#define null NULL


char buf1[1];
char bufResponse[MAX_SERIAL_BUFFER_SIZE];
int fd1, fd2, res1, rightDuty, leftDuty;


FILE *fp;

/****************************************************************
 * Global variables
 ****************************************************************/
int keepgoing = 1;	// Set to 0 when ctrl-c is pressed

/****************************************************************
 * signal_handler
 ****************************************************************/
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
	printf( "Ctrl-C pressed, cleaning up and exiting..\n" );
	keepgoing = 0;
}

/****************************************************************
 * ain_export
 ****************************************************************/
int ain_export(unsigned int ain)
{
	int fd, len;
	char buf[MAX_BUF];

	fd = open(AIN_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", ain);
	write(fd, buf, len);
	close(fd);

	return 0;
}

/****************************************************************
 * ain_export
 ****************************************************************/
int initializeXbee() {
        //The following Canonical example of terminos provided by:
        //http://www.faqs.org/docs/Linux-HOWTO/Serial-Programming-HOWTO.html#AEN125
        /*
          Open modem device for reading and writing and not as controlling tty
          because we don't want to get killed if linenoise sends CTRL-C.
        */
        //fd1 = open(MODEMDEVICE_UART1, O_RDWR | O_NOCTTY );
        fd1 = open(MODEMDEVICE_UART2, O_RDWR | O_NOCTTY);
        if (fd1 <0) {perror(MODEMDEVICE_UART2); exit(-1); }

	//fd2 = open(MODEMDEVICE_UART4, O_RDWR | O_NOCTTY);
        //if (fd2 <0) {perror(MODEMDEVICE_UART4); exit(-1); }
        /*
          now clean the modem line and activate the settings for the port
        */
        //tcflush(fd1, TCIFLUSH);
        //tcsetattr(fd1,TCSANOW,&newtio1);

        return 1;
}

/****************************************************************
 * ain_export
 ****************************************************************/
void* listenForCommands()
{
	int i = 0;
	int timeout = 0;
	//char buffer[1];
	//Main thread loop... never ending
	while (1)
	{
		//printf("writing to buffer");
		//write(fd1,"ABC0(2ABC04T",12);
		//printf("ListenThread .. Now Listening...");

		//res1 = read(fd1,buf1,sizeof(buf1));
		//buf1[res1]=0;             /* set end of string, so we can printf */

		//for(i = 0; i < res1; i++)
		//{
			//Here we're going to bomb on all the carriage returns and new lines
			//because they can easily confuse parsing the data
			//if(buf1[i] == 0xA || buf1[i] == 0xD)
			//{
			//	buf1[i] = 0;
			//}
		//}
		//printf("reading from buffer");
		res1 = read(fd1,buf1,1);
		//if(buffer[0] != buf1[i-1]){
			//buf1[i] = buffer[0]; 
			//parseMessage(buf1,res1);
			//printf("%c\n\r",buf1);
			//i++;

		if(i == 0){
			if(buf1[0] == 'h')
				i++;
			else 
				i = 0;
		}
		else if(i == 1){
			if(buf1[0] == 'i')
				i++;
			else
				i = 0;
		}
		else if (i == 2){
			if(buf1[0] == 'j')
				i++;
			else
				i = 0;
		}
		else if (i == 3){
			//printf("rightDuty:%d",buf1[0]);
                	rightDuty = buf1[0];
			i++;
                }
		else if (i == 4){
                        if(buf1[0] == 't')
                                i++;
                        else
                                i = 0;
                }
		else if (i == 5){
			//printf("leftDuty:%d",buf1[0]);
                        leftDuty = buf1[0];
			i++;
            	}
		else if (i == 6){
                        if(buf1[0] == 'u')
                                i++;
                        else
                                i = 0;
                }
		else if (i == 7){
                        if(buf1[0] == '\n')
                                i++;
                        else
                                i = 0;
                }
		else if(i == 8){
			i = 0;
		}
		/*if(i == 0){
			if(timeout == 5){
				leftDuty = 0;
				rightDuty = 0;
			} else {
				timeout++;
			}
		} else {
			timeout = 0;
		}*/
		//if(buf1[0] == 1){
		//	printf("Out of Range!");
		//}
		//printf("%c\n\r",buf1[0]);	
		//}
	}
	//for(i = 0; i < 1000; i++){
	//	printf("%c\n\r",buf1[i]);
	//}
	
	return null;
}

/****************************************************************
 * Convert int to string
 ****************************************************************/
char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

int kbhit(void){
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

/****************************************************************
 * ain_export
 ****************************************************************/
int sendInt(int value)
{
	//std::string s = "Sending Message Acknowledge : " + AckMessage;
	//addToLog("ListenThread", s, true);

	//The following lines were causing a segmentation fault. Why?
	//char *msg = new char[AckMessage.size()];
	//strcpy(msg, AckMessage.c_str());
	//strcpy(&bufResponse[0], &msg);

	//strcpy(&bufResponse[0], AckMessage.c_str());
	//strcat(bufResponse, "\r");
	//strcat(bufResponse, "\n");
	
	itoa(value,bufResponse);
	strcat(bufResponse,"\n");
	write(fd1,&bufResponse,strlen(bufResponse));

	return 1;
}

/****************************************************************
 * ain_export
 ****************************************************************/
/*int sendChar(char value)
{
        //std::string s = "Sending Message Acknowledge : " + AckMessage;
        //addToLog("ListenThread", s, true);

        //The following lines were causing a segmentation fault. Why?
        //char *msg = new char[AckMessage.size()];
        //strcpy(msg, AckMessage.c_str());
        //strcpy(&bufResponse[0], &msg);

        //strcpy(&bufResponse[0], AckMessage.c_str());
        //strcat(bufResponse, "\r");
        strcat(bufResponse,value);
        strcat(bufResponse,"\n");
        write(fd1,&bufResponse,strlen(bufResponse));

        return 1;
}*/

/****************************************************************
 * set duty cycle for PWM
 ****************************************************************/
int setPWM(double dutyCycle, int toggle, int Dir, int activePin){
	//printf("..running PWM.. Pin: %d\n\r",activePin);
	//printf("..running PWM.. activePin %d dutyCycle %f\n\r",activePin, dutyCycle);
	if(dutyCycle != 0 && activePin != 0){
		if(toggle)
		{
			gpio_set_value(activePin, HIGH);
			//printf("f\n\r",10000*(dutyCycle/100));
			usleep(10000*(dutyCycle/100));
		}
		else
		{
			gpio_set_value(activePin, LOW);
			usleep(10000*((100-dutyCycle)/100));
		}
	} else {
		//gpio_set_value(activePin, LOW);
	}
	//sleep(1);
	return 1;
}

/****************************************************************
 * Intermediary function since you must call void* to use pthread
 ****************************************************************/
void* updatePWM(void* argus){
	PWMThread* args = (PWMThread*) argus;
	int tog = args->toggle;
	while(1){
		tog = !tog;
		double duty = args->dutyCycle;
		//int tog = args->toggle;
		int dir = args->Dir;
		int act = args->activePin; 
		setPWM(duty, tog, dir, act);
	}
	return null;
}

void* getUART(void* argus){
	UARTThread* args = (UARTThread*) argus;
	listenForCommands();
	return null;
}

void stopAll(){
	//leftDuty = 0;
	//rightDuty = 0;	
}

/****************************************************************
 * Initializes PWMs for motor controller
 ****************************************************************/
int initializePWM() {
        //Now export the pins we need
        gpio_export(UART_SELECT_0_GPIO_PIN_NUM);
        gpio_export(UART_SELECT_1_GPIO_PIN_NUM);
        gpio_export(UART_SELECT_2_GPIO_PIN_NUM);
        gpio_export(UART_SELECT_3_GPIO_PIN_NUM);

        //Set the pin's direction
        gpio_set_dir(UART_SELECT_0_GPIO_PIN_NUM, OUTPUT_PIN);
        gpio_set_dir(UART_SELECT_1_GPIO_PIN_NUM, OUTPUT_PIN);
        gpio_set_dir(UART_SELECT_2_GPIO_PIN_NUM, OUTPUT_PIN);
        gpio_set_dir(UART_SELECT_3_GPIO_PIN_NUM, OUTPUT_PIN);

        //Initialize the pin's state
        gpio_set_value(UART_SELECT_0_GPIO_PIN_NUM, LOW);
        gpio_set_value(UART_SELECT_1_GPIO_PIN_NUM, LOW);
        gpio_set_value(UART_SELECT_2_GPIO_PIN_NUM, LOW);
        gpio_set_value(UART_SELECT_3_GPIO_PIN_NUM, LOW);
        return 1;
}
	

void main(int argc, char** argv)
{

	//struct timespec tim, tim2;
	//tim.tv_sec = 1;
	//tim.tv_nsec = 500;
	// initialize XBee
	initializeXbee();
	initializePWM();
	gpio_export(LED_PIN);
        gpio_set_dir(LED_PIN, OUTPUT_PIN);
        gpio_set_value(LED_PIN, HIGH);

	int threadErr;
	char c = (char) 255;
	//char c = 45;
	int leftDir, rightDir;
	//double leftDuty, rightDuty;

	//printf("listening for commands.. %c", c);
	//sendInt(c);
	//listenForCommands();

	PWMThread lPWM;
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, &updatePWM, &lPWM);
	lPWM.toggle = 0;
	lPWM.activePin = 0;
	PWMThread rPWM;
	pthread_create(&tid, &attr, &updatePWM, &rPWM);
        rPWM.toggle = 0;
	rPWM.activePin = 0;
	UARTThread uartScan;
	pthread_create(&tid, &attr, &getUART, &uartScan);
	//threadErr = pthread_join(lPWM.tid, null);

	//if (threadErr != 0) {printf("pthread left join error: %d\n",threadErr); }

	//threadErr = pthread_join(rPWM.tid, null);

        //if (threadErr != 0) {printf("pthread right join error: %d\n",threadErr); }
	//leftDuty = 60;
	//rightDuty = 60;
	while(1)
	{
	/*	if(rightDuty > 0){
			rightDir = 1;
			rPWM.dutyCycle = rightDuty;
			rPWM.activePin = UART_SELECT_3_GPIO_PIN_NUM;
                        gpio_set_value(UART_SELECT_2_GPIO_PIN_NUM, LOW);	
		} else {
			rightDir = 0;
			rPWM.dutyCycle = -rightDuty;
			rPWM.activePin = UART_SELECT_2_GPIO_PIN_NUM;
                        gpio_set_value(UART_SELECT_3_GPIO_PIN_NUM, LOW);
		}
		if(leftDuty > 0){
			leftDir = 1;
			lPWM.dutyCycle = leftDuty;
			lPWM.activePin = UART_SELECT_0_GPIO_PIN_NUM;
                        gpio_set_value(UART_SELECT_1_GPIO_PIN_NUM, LOW);
		} else {
			leftDir = 0;
			lPWM.dutyCycle = -leftDuty;
			lPWM.activePin = UART_SELECT_1_GPIO_PIN_NUM;
                        gpio_set_value(UART_SELECT_0_GPIO_PIN_NUM, LOW);
		}*/
		//write(fd2,"ABC0(2ABC04T\n\r",14);
		//write(fd1,"a",1);
		//printf("hello world");
		//sendInt(c);
		//sendInt(c);
		//listenForCommands();
		
		lPWM.dutyCycle = 60;
		rPWM.dutyCycle = 60;
		leftDir = 1;
		rightDir = 1;
		//set_interface_attribs (fd1, B9600, 0);  // set speed to 9,600 bps, 8n1 (no parity)
		//set_blocking (fd1, 0);                // set no blocking
		//res1 = read(fd1,buf1,sizeof(buf1));
		//buf1[res1]=0;
		//if(
		//leftDuty = 0;
		//leftDir = 1;
		//rightDuty = 0;
		//rightDir = 1;
		//printf("...running loop...\n");
		//printf("leftDuty cycle: %f, rightDuty cycle: %f\n\r",lPWM.dutyCycle, rPWM.dutyCycle);
		/*if(c == 255){
			leftDuty = 0;
                        rightDuty = 0;
		}*/

		WINDOW *w = initscr();
                //timeout(1);
                //while((int)c == 255){
                        //usleep(1);
			//lPWM.dutyCycle = 0;
                        //rPWM.dutyCycle = 0;
		if(kbhit()){
                        c = getchar();
		} else {
                        c = 255;
		}
		if(c == 'c'){
			while(1){
				gpio_set_value(lPWM.activePin, LOW);
				gpio_set_value(rPWM.activePin, LOW);
				lPWM.activePin = 0;
				rPWM.activePin = 0;
			}
		}
                //}
                //endwin();
                //printf("received %c (%d)\n\r", c, (int) c);
		/*if(c == 'w'){
			leftDuty+=0.2;
			rightDuty+=0.2;
		} else if(c == 's'){
			leftDuty-=0.2;
			rightDuty-=0.2;
		} else if(c == 'd'){
			leftDuty+=0.4;
			rightDuty=+0.2;
		} else if(c == 'a'){
			leftDuty+=0.2;
			rightDuty+=0.4;
		} else if(c = 255){
			leftDuty = 0;
			rightDuty = 0;
		}
                        if(lPWM.dutyCycle != leftDuty){
                                lPWM.dutyCycle = leftDuty;
                                lPWM.Dir = leftDir;
                        }
                        if(rPWM.dutyCycle != rightDuty){
                                rPWM.dutyCycle = rightDuty;
                                rPWM.Dir = rightDir;
                        }*/
                        if(leftDir == 1){
                                lPWM.activePin = UART_SELECT_0_GPIO_PIN_NUM;
                                gpio_set_value(UART_SELECT_1_GPIO_PIN_NUM, LOW);
                        } else {
                                lPWM.activePin = UART_SELECT_1_GPIO_PIN_NUM;
                                gpio_set_value(UART_SELECT_0_GPIO_PIN_NUM, LOW);
                        }
                        if(rightDir == 1){
                                rPWM.activePin = UART_SELECT_3_GPIO_PIN_NUM;
                                gpio_set_value(UART_SELECT_2_GPIO_PIN_NUM, LOW);
                        } else {
                                rPWM.activePin = UART_SELECT_2_GPIO_PIN_NUM;
                                gpio_set_value(UART_SELECT_3_GPIO_PIN_NUM, LOW);
                        }
	}
}
