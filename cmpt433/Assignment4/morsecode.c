// Example test driver:

#include <linux/module.h>
#include <linux/miscdevice.h>		// for misc-driver calls.
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/leds.h>
#include <linux/kfifo.h>

// #error Are we building this file?

DEFINE_LED_TRIGGER(morsecode_trigger);
#define MY_DEVICE_FILE  "morse-code"
// Define the toy data:
#define START_CHAR 'A'
#define END_CHAR   'z'
#define DATA_SIZE  (END_CHAR - START_CHAR + 1)
static char data[DATA_SIZE];

// Morsecode functions
#define DOT_TIME 200

/**************************************************************
 * FIFO Support
 *************************************************************/
#define FIFO_SIZE 512	// Must be a power of 2.
#define ECHO_DEVICE_FILE  "echo"
static DECLARE_KFIFO(echo_fifo, char, FIFO_SIZE);

/**************************************************************
 * Morsecode Support
 *************************************************************/
static unsigned short morsecode_codes[] = {
		0xB800,	// A 1011 1
		0xEA80,	// B 1110 1010 1
		0xEBA0,	// C 1110 1011 101
		0xEA00,	// D 1110 101
		0x8000,	// E 1
		0xAE80,	// F 1010 1110 1
		0xEE80,	// G 1110 1110 1
		0xAA00,	// H 1010 101
		0xA000,	// I 101
		0xBBB8,	// J 1011 1011 1011 1
		0xEB80,	// K 1110 1011 1
		0xBA80,	// L 1011 1010 1
		0xEE00,	// M 1110 111
		0xE800,	// N 1110 1
		0xEEE0,	// O 1110 1110 111
		0xBBA0,	// P 1011 1011 101
		0xEEB8,	// Q 1110 1110 1011 1
		0xBA00,	// R 1011 101
		0xA800,	// S 1010 1
		0xE000,	// T 111
		0xAE00,	// U 1010 111
		0xAB80,	// V 1010 1011 1
		0xBB80,	// W 1011 1011 1
		0xEAE0,	// X 1110 1010 111
		0xEBB8,	// Y 1110 1011 1011 1
		0xEEA0,	// Z 1110 1110 101
};

static unsigned short getEncoding(char letter){
	int encodingPosition;
	// printk(KERN_INFO "Letter: %c\n", letter);
	// printk(KERN_INFO "Letter Ascii: %d\n", letter);
	if(letter >= 97 && letter <= 122){
		// LowerCase
		//printk(KERN_INFO "Lower Case\n");
		encodingPosition = letter - 97;
	}
	else if (letter >= 65 && letter <= 90){
		// UpperCase
		//printk(KERN_INFO "Upper Case\n");
		encodingPosition = letter - 65;
	}
	else{
		return 0;
	}
	

	//printk(KERN_INFO "encoding position: %d\n", encodingPosition);

	return morsecode_codes[encodingPosition];
}

static int isValidChar(char letter){
	if((letter >= 97 && letter <= 122) || (letter >= 65 && letter <= 90) || (letter == 32)){
		return 1;
	}
	else{
		return 0;
	}
}


static int led_flashMorsecode(char letter, int consecLetter){
	int i;
	int count = 0;
	unsigned short encoding = 0;
	unsigned short temp = 0;
	unsigned short shift = 0xffff;

	if(letter == ' '){
		led_trigger_event(morsecode_trigger, LED_OFF);
		for(i = 0; i < 7; i++){
			msleep(DOT_TIME);
		}
		return 0;
	}else{
		encoding = getEncoding(letter);	
	}
	
	if(encoding == 0){
		return consecLetter;
	}

	//printk(KERN_INFO "Initial Encoding: %x",encoding);
	if(consecLetter == 1){
		led_trigger_event(morsecode_trigger, LED_OFF);
		for(i = 0; i < 3; i++){
			msleep(DOT_TIME);
		}
	}
	for(i = 0; i < 15; i ++){
		shift = shift >> 1;

		temp = encoding;
		encoding = encoding & shift;
		if(encoding != temp){
			//printk(KERN_INFO "1");
			led_trigger_event(morsecode_trigger, LED_FULL);
			msleep(DOT_TIME);
			count++;
		}
		else{
			//printk(KERN_INFO "0");
			led_trigger_event(morsecode_trigger, LED_OFF);
			msleep(DOT_TIME);
			if(count == 1){
				count = 0;
				if (!kfifo_put(&echo_fifo, '.')) {
					// Fifo full
					return -EFAULT;
				}
			}
			else if(count == 3){
				count = 0;
				if (!kfifo_put(&echo_fifo, '-')) {
					// Fifo full
					return -EFAULT;
				}
			}
			else{
				printk(KERN_INFO "Encountered an error converting letters to morsecode.");
				return -EFAULT;
			}
		}

		if(encoding == 0){
			led_trigger_event(morsecode_trigger, LED_OFF);
			if(count == 1){
				count = 0;
				if (!kfifo_put(&echo_fifo, '.')) {
					// Fifo full
					return -EFAULT;
				}
			}
			else{
				count = 0;
				if (!kfifo_put(&echo_fifo, '-')) {
					// Fifo full
					return -EFAULT;
				}
			}
			break;
		}
	}

	return 1;
}
/******************************************************
 * LED
 ******************************************************/


static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple("morse-code", &morsecode_trigger);
}

static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(morsecode_trigger);
}
 



/******************************************************
 * Initialization of Data Function
 ******************************************************/
static void initialize_data(void)
{
	int i = 0;
	for (i = 0; i < DATA_SIZE; i++) {
		data[i] = i + START_CHAR;
	}
}



/******************************************************
 * Callbacks
 ******************************************************/
static ssize_t echo_read(struct file *file,
		char *buf, size_t count, loff_t *ppos)
{
	// Pull all available data from fifo into user buffer
	int num_bytes_read = 0;

	if (kfifo_to_user(&echo_fifo, buf, count, &num_bytes_read)) {
		return -EFAULT;
	}

	// Can use kfifo_get() to pull out a single value to kernel:
	/*
	int val;
	if (!kfifo_get(&echo_fifo, &val)) {
		// fifo empty.... handle it some how
	}
	// now data is in val... do with it what you will.
	*/

	// printk(KERN_INFO "Val is: %d\n", val);
	// now data is in val... do with it what you will.

	return num_bytes_read;  // # bytes actually read.
}

static ssize_t buffer_write(struct file* file, const char *buff,
		size_t count, loff_t* ppos)
{
	int i;
	int consecLetter = 0;
	//int copied;
	char character;

	//printk(KERN_INFO "demo_ledtrig: Flashing %d times for string.\n", count);

	// Blink once per character (-1 to skip end null)
	for (i = 0; i < count-1; i++) {
		memset(&character, 0, 1);
		// Get the character
		if (copy_from_user(&character, &buff[i], sizeof(character))) {
			return -EFAULT;
		}

		if (isValidChar(character)){
			
			consecLetter = led_flashMorsecode(character, consecLetter);
			//my_led_blink();

			// if (kfifo_from_user(&echo_fifo, &buff[i], sizeof(buff[i]), &copied)) {
			// 	return -EFAULT;
			// }
			if (character == 32){
				if (!kfifo_put(&echo_fifo, ' ')) {
					// Fifo full
					return -EFAULT;
				}
			}
			if(i < count-2){
				if (!kfifo_put(&echo_fifo, ' ')) {
					// Fifo full
					return -EFAULT;
				}
			}
		}

	}

	if (!kfifo_put(&echo_fifo, '\n')) {
			// Fifo full
			return -EFAULT;
		}

	//printk(KERN_INFO "  Finished \n");

	// Return # bytes actually written.
	return count;
}



/******************************************************
 * Misc support
 ******************************************************/
// Callbacks:  (structure defined in /linux/fs.h)
struct file_operations my_fops = {
    .owner = THIS_MODULE,
	.read = echo_read,
	.write = buffer_write
};

// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name = MY_DEVICE_FILE,             // /dev/.... file.
		.fops = &my_fops                    // Callback functions.
};


/******************************************************
 * Driver initialization and exit:
 ******************************************************/
static int __init morsecode_init(void){
    int ret;
	printk(KERN_INFO "----> Morsecode driver init(): file /dev/%s.\n", MY_DEVICE_FILE);

    // Register as a misc driver:
	ret = misc_register(&my_miscdevice);

    // LED:
	led_register();

	INIT_KFIFO(echo_fifo);

    initialize_data();

    return ret;
}

static void __exit morsecode_exit(void)
{
    printk(KERN_INFO "<---- Morsecode driver exit().\n");

    // Unregister misc driver
	misc_deregister(&my_miscdevice);

	// LED:
	led_unregister();
}



// Link our init/exit functions into the kernel's code.
module_init(morsecode_init);
module_exit(morsecode_exit);

// Information about this module:
MODULE_AUTHOR("Jacky Tse, Kenneth Liang");
MODULE_DESCRIPTION("Morse Code Kernel Driver");
MODULE_LICENSE("GPL");
// Important to leave as GPL.


