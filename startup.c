/*
 * 	  startup.c
 */
 
__attribute__((naked)) 
__attribute__((section (".start_section")) )

void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}

// Port Definitions
#define SysTick  ((volatile unsigned int *) 0xE000E010)
#define STK_CTRL ((volatile unsigned int *) SysTick + 0x0)
#define STK_LOAD ((volatile unsigned int *) SysTick + 0x4)
#define STK_VAL  ((volatile unsigned int *) SysTick + 0x8)

#define PORT_BASE   ((volatile unsigned int  *) 0x40021000)
/* Definitions of Word Adresses for Initiation */
#define portModer   ((volatile unsigned int  *) PORT_BASE + 0x0)
#define portOtyper  ((volatile unsigned int  *) PORT_BASE + 0x4)
#define portOspeedr ((volatile unsigned int  *) PORT_BASE + 0x8)
#define portPupdr   ((volatile unsigned int  *) PORT_BASE + 0xC)
/* Definitions of Byte Adresses for Data and Controlregister */
#define portIdrLow  ((volatile unsigned char *) PORT_BASE + 0x10)
#define portIdrHigh ((volatile unsigned char *) PORT_BASE + 0x11)
#define portOdrLow  ((volatile unsigned char *) PORT_BASE + 0x14)
#define portOdrHigh ((volatile unsigned char *) PORT_BASE + 0x15)
/* Definitions for Bit Positions in Controlregister */
#define B_E         ((volatile unsigned char *) 0x40)
#define B_SELECT    ((volatile unsigned char *) 0x4)  
#define B_RW        ((volatile unsigned char *) 0x2)
#define B_RS        ((volatile unsigned char *) 0x1)

typedef struct
{
    char x,y;
} POINT, *PPOINT;

typedef struct
{
    POINT p0, p1;
} LINE, *PLINE;


__attribute__((naked))
void graphic_initialize(void)
{
    __asm volatile (".HWORD 0xDFF0\n");
    __asm volatile ("BX LR\n");
}

__attribute__((naked))
void graphic_clear_screen(void)
{
    __asm volatile (".HWORD 0xDFF1\n");
    __asm volatile ("BX LR\n");
}

__attribute__((naked))
void graphic_pixel_set(int x, int y)
{
    __asm volatile (".HWORD 0xDFF2\n");
    __asm volatile ("BX LR\n");
}

__attribute__((naked))
void graphic_pixel_clear(int x, int y)
{
    __asm volatile (".HWORD 0xDFF3\n");
    __asm volatile ("BX LR\n");
}




// Method to delay 250 ns
void delay_250ns(void)
{
    // System Clock: 168*10^6 Hz
    // 250 nanosekunder: 250*168*10^6*10^-9 = 42 = 0x2A
    *STK_CTRL = 0;      // Reset SysTick
    *STK_LOAD = 0x2A-1; // Load value (N klockcykler ges av räknarvärde N-1)
    *STK_VAL  = 0;      // Reset counter
    *STK_CTRL = 5;      // Prepare SysTick (STK_LOAD to STK_VAL)
    
    while ((*STK_CTRL & 0x00010000) == 0)
    {
        //*STK_VAL = *STK_VAL - 1;
    }
    
    *STK_CTRL = 0;
}

// Method to delay variable mikroseconds
void delay_mikro(unsigned int us)
{
    for (int i = 1; i <= us; i++)
    {
        #ifdef SIMULATOR
            ms = ms/1000;
            ms++;
       
        #endif
            delay_250ns();
            delay_250ns();
            delay_250ns();
            delay_250ns();
    }
}

// Method to delay variable millieseconds
void delay_milli(unsigned int ms)
{
    for (int i = 1; i <= ms; i++)
    {
        #ifdef SIMULATOR
            ms = ms/1000;
            ms++;
        
        #endif
            delay_mikro(4);
    }
}

void main(void)
{
    int i;
    graphic_initialize();
    graphic_clear_screen();
    for (i=1; i <= 128; i++)        // Draw horizontal line
        graphic_pixel_set(i,10);
    for (i=1; i <= 64; i++)         // Draw vertical line
        graphic_pixel_set(10,i);
    delay_milli(500);               // Wait 0.5 seconds
    for (i=1; i<=128; i++)
        graphic_pixel_clear(i,10);  // Remove horizontal line
    for (i=1; i <= 64; i++)
        graphic_pixel_clear(10, i); // Remove vertical line
}

