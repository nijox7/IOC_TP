#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <stdint.h>
#include <time.h>
#include <sys/mman.h>


#define MAXServerResquest 1024

//------------------------------------------------------------------------------
// GPIO ACCES
//------------------------------------------------------------------------------

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

#define GPIO_LED0   4
#define GPIO_LED1   17
#define GPIO_BP     18

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
};

struct gpio_s *gpio_regs_virt; 


static void 
gpio_fsel(uint32_t pin, uint32_t fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs_virt->gpfsel[reg] = (gpio_regs_virt->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void 
gpio_write (uint32_t pin, uint32_t val)
{
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    if (val == 1) 
        gpio_regs_virt->gpset[reg] = (1 << bit);
    else
        gpio_regs_virt->gpclr[reg] = (1 << bit);
}

static int gpio_read(int pin)
{
    return (gpio_regs_virt->gplev[pin/32] & (1 << (pin % 32))) != 0;
}

//------------------------------------------------------------------------------
// Access to memory-mapped I/O
//------------------------------------------------------------------------------

#define RPI_PAGE_SIZE           4096
#define RPI_BLOCK_SIZE          4096

static int mmap_fd;

static int gpio_mmap ( void ** ptr )
{
    void * mmap_result;

    mmap_fd = open ( "/dev/mem", O_RDWR | O_SYNC );

    if ( mmap_fd < 0 ) {
        return -1;
    }

    mmap_result = mmap (
        NULL
      , RPI_BLOCK_SIZE
      , PROT_READ | PROT_WRITE
      , MAP_SHARED
      , mmap_fd
      , BCM2835_GPIO_BASE );

    if ( mmap_result == MAP_FAILED ) {
        close ( mmap_fd );
        return -1;
    }

    *ptr = mmap_result;

    return 0;
}

void gpio_munmap ( void * ptr )
{
    munmap ( ptr, RPI_BLOCK_SIZE );
}

void delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

void blink_thread(int *period) {
    uint32_t val = 0;
    int half_period = *period / 2;
    while (1) {
        gpio_write ( GPIO_LED0, val );
        delay ( half_period );
        val = 1 - val;
    }
}

void bp_thread(int* period_arg) {
    // button
    int period = *period_arg;
    int bounce_period = 100;
    int old_val = 1; // default button released
    int val = 1;
    int bp_on = 0;
    // led
    gpio_write(GPIO_LED0, 1);

    // programm loop
    while(1){
        old_val = val;
	    val = gpio_read(GPIO_BP);
        if (old_val != val){
            if (val == 0){
                printf("Button pressed\n");
                gpio_write(GPIO_LED0, 1);
                bp_on = 1;

                
            }
            else {
                printf("Button released\n");
                gpio_write(GPIO_LED0, 0);
                bp_on = 0;
            }
        }

        // waiting bounce period button before take new measure
        // waiting the original period if bounce_period is too short 
        if (bp_on && (period < bounce_period)) delay(bounce_period);
        else delay(period);
    }

    return;
}


int main()
{
    int     f2s, s2f;                                       // fifo file descriptors
    char    *f2sName = "/tmp/f2s_fw";                       // filo names
    char    *s2fName = "/tmp/s2f_fw";                       //
    char    serverRequest[MAXServerResquest];               // buffer for the request
    fd_set  rfds;                                           // flag for select
    struct  timeval tv;                                     // timeout
    tv.tv_sec = 1;                                          // 1 second
    tv.tv_usec = 0;                                         //

    mkfifo(s2fName, 0666);                                  // fifo creation
    mkfifo(f2sName, 0666);

    /* open both fifos */
    s2f = open(s2fName, O_RDWR);                            // fifo openning
    f2s = open(f2sName, O_RDWR);

    uint32_t volatile * gpio_base = 0;
    if ( gpio_mmap ( (void **)&gpio_regs_virt ) < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_LED1, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_BP, GPIO_FSEL_INPUT);

    pthread_t thd;
    if (pthread_create(&thd, NULL, (void*) bp_thread, &period) < 0){
        printf("-- error: cannot create thread.\n");
        exit(1);
    }

    do {
        FD_ZERO(&rfds);                                     // erase all flags
        FD_SET(s2f, &rfds);                                 // wait for s2f

        if (select(s2f+1, &rfds, NULL, NULL, &tv) != 0) {   // wait until timeout
            if (FD_ISSET(s2f, &rfds)) {                     // something to read
                int nbchar;
                if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
                serverRequest[nbchar]=0;
                fprintf(stderr,"%s", serverRequest);
                write(f2s, serverRequest, nbchar);
                // Choix de la commande
                if (strncmp(serverRequest, "led0", 4) == 0){
                    gpio_write(GPIO_LED0, 1 - gpio_read(GPIO_LED0)); // on inverse l'état de la led
                }
                else if (strncmp(serverRequest, "led1", 4) == 0){
                    gpio_write(GPIO_LED1, 1 - gpio_read(GPIO_LED1)); // on inverse l'état de la led
                }
            }
        }
    }
    while (1);

    pthread_join(thd, NULL);
    close(f2s);
    close(s2f);

    return 0;
}
