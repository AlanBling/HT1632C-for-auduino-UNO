/*---------------------------PIN setting---------------------------------------------------*/
#define DAT 7
#define _WR 6
#define _RD 5
#define _CS 4

/*------------------------Define command mode and instruction code------------------------------------------*/
#define MOD_READ			0xC0			//Read data mode
#define MOD_WRITE			0xA0			//Write data mode
#define MOD_RMW				0xA0			//Read modify write mode
#define MOD_CMD				0x80			//Command mode

#define SYS_DIS				0x00			//Turn off the system clock
#define SYS_EN				0x01			//Turn on the system clock
#define LED_OFF				0x02			//Turn off LED display
#define LED_ON				0x03			//Turn on LED display
#define BLINK_OFF			0x08		   	//Closed flashing
#define	BLINK_ON			0X09		   	//Open the flash
#define	SLAVE_MODE			0X10		   	//Slave mode
#define	RC_MASTER_MODE		0X18	   		//RC master mode
#define	EXT_CLK_MASTER_MODE	0X1C   			//External clock master mode
#define COM_OPTION			0X24		   	//16COM
#define PWM_DUTY			0XA0			//PWM Brightness control

/*---------------------------Function declaration---------------------------------------------------*/
void HT1632C_Interface_Init(void);			//Communication interface initialization
void HT1632C_Init(void);					//HT1632C Initialization function
void HT1632C_clr(void);						//Screen clearing function

// Write continuous data from the specified row. 
// CNT is the number of write rows, and p points to the array of data to be sent
void HT1632C_Write_MultiRow(unsigned char row,unsigned char *p,unsigned char cnt);
//Update the row row row row with a row of data, with p pointing to the array of data to be sent
void HT1632C_Update_Onerow(unsigned char row,unsigned char *p);
//Update the entire control data, with p pointing to the array of data to be sent
void HT1632C_Update_OnePage(unsigned char *p);
//Reads a row of data from the specified row row from HT1632C and saves it to the array that Arr points to
void HT1632C_Read_Onerow(unsigned char *Arr,unsigned char row);
//Reading from HT1632C so many lines of CNT starting with the specified row is saved to the array pointed by Arr
void HT1632C_Read_Multirow(unsigned char *Arr,unsigned char row,unsigned char cnt);
//Read the entire control data from HT1632C to the array that the Arr points to
void HT1632C_Read_OnePage(unsigned char *Arr);




/*************************************************************************************
*Funtion name：void HT1632C_Interface_Init(void)
*Discription：Initialize the HT1632C interface
**************************************************************************************/
void HT1632C_Interface_Init(void){
	pinMode(DAT, OUTPUT);
	pinMode(_WR, OUTPUT);
	pinMode(_RD, INPUT);
	pinMode(_CS, OUTPUT);

	digitalWrite(DAT, HIGH);
	digitalWrite(_WR, HIGH);
	digitalWrite(_RD, HIGH);
	digitalWrite(_CS, HIGH);
}


/*************************************************************************************
*Function name：void HT1632C_Writer(unsigned char Data,unsigned char cnt)
*Discription：HT1632C sends the data function. 
			  The high position is in front and the sending digit can be set
**************************************************************************************/
void HT1632C_Writer(unsigned char dat,unsigned char cnt){
	unsigned char i;
	for(i=0; i<cnt; i++){
		digitalWrite(_WR, LOW);
		if(dat&0x80){
			digitalWrite(DAT, HIGH);
		}else{
			digitalWrite(DAT, LOW);
		}
		dat<<=1;
		digitalWrite(_WR, HIGH);
	}
}


/*************************************************************************************
*Function name：unsigned char HT1632C_Reader(unsigned char cnt)
*Discription：HT1632C reads the data function and returns a byte
**************************************************************************************/
unsigned char HT1632C_Reader(unsigned char cnt){
	unsigned char i,dat;
	for(i=0;i<cnt;i++){
		digitalWrite(_RD, LOW);
		digitalWrite(_RD, HIGH);
		
		if(DAT){
			dat=dat<<1;
			dat=dat|0x01;
		}else{
			dat=dat<<1;
		}
	}
	digitalWrite(_RD, HIGH);
	return dat;
}



/*************************************************************************************
*Funtion name：void HT1632C_Read_Onerow(unsigned char *Arr,unsigned char row)
*Discription：Reads the data for the specified row
*			  Arr: read out the array of data to be saved. The first byte is saved to Arr[0] and the second byte is saved to Arr[1].
			  Row: specifies which row of data to read
**************************************************************************************/
void HT1632C_Read_Onerow(unsigned char *Arr,unsigned char row){
	row=(row-1)<<2;
	digitalWrite(_RD, HIGH);
	digitalWrite(_CS, LOW);
	HT1632C_Writer(MOD_READ,3);							//The command code 101
	HT1632C_Writer(row<<1,7);							//7 bit address
	Arr[0]=HT1632C_Reader(8);							//Read 8-bit data
	Arr[1]=HT1632C_Reader(8);	
	digitalWrite(_CS, HIGH);

}


/*************************************************************************************
*Funtion name：void HT1632C_Read_Multirow(unsigned char *Arr,unsigned char row,unsigned char cnt)
*Discription：Reads the specified number of rows from the specified row
* 			  Arr: the array to which the read data is stored, row: read from which line, CNT: the number of rows read
**************************************************************************************/
void HT1632C_Read_Multirow(unsigned char *Arr,unsigned char row,unsigned char cnt){
	
	unsigned char i,j;

	row=(row-1)<<2;
	digitalWrite(_RD, HIGH);
	digitalWrite(_CS, LOW);
	HT1632C_Writer(MOD_READ,3);							//The command code 101
	HT1632C_Writer(row<<1,7);							//7 bit address
	for(i=0,j=0; i<cnt; i++,j++,j++){
		Arr[j]=HT1632C_Reader(8);						//Read 8-bit data
		Arr[j+1]=HT1632C_Reader(8);	
	}
	digitalWrite(_CS, HIGH);

}


/*************************************************************************************
*Function name：void HT1632C_Read_OnePage(unsigned char *Arr)
*Discription：Read the data for an entire module
*			  Arr: the location where the data is stored when it is read
**************************************************************************************/
void HT1632C_Read_OnePage(unsigned char *Arr){
	unsigned char i,j;
	digitalWrite(_RD, HIGH);
	digitalWrite(_CS, LOW);
	HT1632C_Writer(MOD_READ,3);						//The command code 101
	HT1632C_Writer(0,7);							//7 bit address
	for(i=0,j=0; i<24; i++,j++,j++){
		Arr[j]=HT1632C_Reader(8);					//Read 8-bit data
		Arr[j+1]=HT1632C_Reader(8);	
	}
	digitalWrite(_CS, HIGH);

}


/*************************************************************************************
*Funtion name：void HT1632C_Writer_CMD(unsigned char cmd)	
*Discription：HT1632C sends the command function
**************************************************************************************/
void HT1632C_Writer_CMD(unsigned char cmd){
	digitalWrite(_CS, LOW);
	HT1632C_Writer(MOD_CMD,3);			//Command mode code，just send fisrst bit
	HT1632C_Writer(cmd,9);				//Command data，the9th digit is 0
	digitalWrite(_CS, HIGH);
}


/*************************************************************************************
*Funtion name：void HT1632C_Update_Onerow(unsigned char col,unsigned char row,unsigned char c)		
*Discription：Update the specified row
*			  Row: which row, 1-24;*p: the address of the data to be sent
**************************************************************************************/
void HT1632C_Update_Onerow(unsigned char row,unsigned char *p){
	row=(row-1)<<2;							//Calculate the actual address

	digitalWrite(_CS, LOW);
	HT1632C_Writer(MOD_WRITE,3);
	HT1632C_Writer(row<<1,7);
	HT1632C_Writer(*p,8);
	HT1632C_Writer(*(p+1),8);
	digitalWrite(_CS, HIGH);
}


/*************************************************************************************
*Funtion name：void HT1632C_Write_MultiRow(unsigned char row,unsigned char *p,unsigned char cnt)
*Discription：Write continuous data from the specified row, and CNT is the number of write rows
**************************************************************************************/
void HT1632C_Write_MultiRow(unsigned char row,unsigned char *p,unsigned char cnt){
	unsigned char i,addr;

	addr=(row-1)<<2;
	digitalWrite(_CS, LOW);
	HT1632C_Writer(MOD_WRITE,3);
	HT1632C_Writer(addr<<1,7);
	for(i=0;i<cnt;i++){
			HT1632C_Writer(*p,8);
			p++;
			HT1632C_Writer(*p,8);
			p++;
	}
	digitalWrite(_CS, HIGH);
} 


/*************************************************************************************
*Funtion name：void HT1632C_Update_OnePage(unsigned char *p)
*Discription：Update a whole screen of data (48 bytes per screen)
*			  *p passes the address of the data array to be sent
**************************************************************************************/
void HT1632C_Update_OnePage(unsigned char *p){
	unsigned char i;

	digitalWrite(_CS, LOW);
	HT1632C_Writer(MOD_WRITE,3);
	HT1632C_Writer(0,7);
	for(i=0;i<48;i++){
			HT1632C_Writer(*p,8);
			p++;
	}
	digitalWrite(_CS, HIGH);
}


/*************************************************************************************
*Funtion name：void HT1632C_clr(void)	
*Discription：Screen clearing function
**************************************************************************************/
void HT1632C_clr(void){
	unsigned char i;
	digitalWrite(_CS, LOW);
	HT1632C_Writer(MOD_WRITE,3);
	HT1632C_Writer(0x00,7);
	for(i=0;i<48;i++){
		HT1632C_Writer(0x00,8);
	}
	digitalWrite(_CS, HIGH);
}


/*************************************************************************************
*Function name：void HT1632C_clr(void)	
*Discription：HT1632C initialization function
**************************************************************************************/
void HT1632C_Init(void){
	digitalWrite(_CS, HIGH);
	digitalWrite(_WR, HIGH);
	digitalWrite(DAT, HIGH);
	HT1632C_Writer_CMD(SYS_DIS);
	HT1632C_Writer_CMD(COM_OPTION);
	HT1632C_Writer_CMD(RC_MASTER_MODE);
	HT1632C_Writer_CMD(SYS_EN);
	HT1632C_Writer_CMD(PWM_DUTY);
	HT1632C_Writer_CMD(BLINK_OFF);
	HT1632C_Writer_CMD(LED_ON);
}


/*************************************************************************************/


/*********************************The Text ***********************************************/

// unsigned char page[160]={//飞翼开发板//
// 0X00,0X02,0X00,0X02,0X00,0X02,0X00,0X02,0X00,0X02,0X00,0X02,0X00,0X02,0X00,0X02,
// 0X00,0X02,0X03,0XFE,0X0C,0X40,0X10,0XA0,0X21,0X10,0X42,0X08,0XF0,0X00,0X00,0X00,
// 0X20,0X00,0X20,0X09,0X2B,0XEB,0XAA,0XA5,0X6A,0XA5,0X3E,0XA1,0X2A,0XBF,0X2B,0XE0,
// 0X2A,0XA9,0X3E,0XAB,0X6A,0XA5,0XAA,0XA5,0X2B,0XE1,0X20,0X1F,0X20,0X00,0X00,0X00,
// 0X00,0X80,0X80,0X82,0X40,0X82,0X30,0X82,0X0F,0XFE,0X00,0X82,0X00,0X82,0X00,0X82,
// 0X00,0X82,0X00,0X82,0XFF,0XFE,0X00,0X82,0X00,0X82,0X00,0X82,0X00,0X80,0X00,0X00,
// 0X00,0X00,0X20,0X00,0X10,0X18,0X8C,0X16,0X83,0X10,0X80,0XD0,0X41,0XB8,0X46,0X97,
// 0X28,0X90,0X10,0X90,0X28,0X90,0X44,0X92,0X43,0X94,0X80,0X10,0X80,0X00,0X00,0X00,
// 0X04,0X10,0X03,0X10,0X00,0XD0,0XFF,0XFF,0X00,0X90,0X83,0X10,0X60,0X00,0X1F,0XFC,
// 0X80,0X24,0X41,0XE4,0X26,0X24,0X18,0X22,0X26,0X23,0X41,0XE2,0X80,0X00,0X00,0X00
// };




unsigned char page[200]={//EGOTO
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x3F,0xFC,0x21,0x04,0x21,0x04,
0x21,0x04,0x23,0x84,0x20,0x04,0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xF0,0x08,0x08,0x10,0x04,0x20,0x04,0x20,0x04,
0x24,0x04,0x3C,0x38,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x18,0x18,0x20,0x04,0x20,0x04,
0x20,0x04,0x20,0x04,0x18,0x18,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x04,0x00,0x04,0x40,0x04,0x7F,0xFC,
0x40,0x04,0x00,0x04,0x00,0x04,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x18,0x18,0x20,0x04,0x20,0x04,
0x20,0x04,0x20,0x04,0x18,0x18,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};



// unsigned char page[200]={//hello
// 0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x04,0x7F,0xFC,0x41,0x04,0x01,0x00,0x01,0x00,
// 0x01,0x00,0x41,0x04,0x7F,0xFC,0x40,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFC,0x3F,0xFC,0x21,0x04,0x21,0x04,
// 0x21,0x04,0x23,0x84,0x20,0x04,0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 0x40,0x04,0x7F,0xFC,0x40,0x04,0x40,0x00,0x40,0x00,0x40,0x00,0x70,0x00,0x00,0x00,
// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 0x40,0x04,0x7F,0xFC,0x40,0x04,0x40,0x00,0x40,0x00,0x40,0x00,0x70,0x00,0x00,0x00,
// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x18,0x18,0x20,0x04,0x20,0x04,
// 0x20,0x04,0x20,0x04,0x18,0x18,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
// };


/***************************************************************************************/

void setup(){

	unsigned char *p;						 //Defines a pointer to the page array

	p=&page[0];								 //To point a pointer to an array address

	HT1632C_Interface_Init();				 //Initialize the 1632 interface
	HT1632C_Init();							 //Initialize the HT1632
	HT1632C_clr();							 //Clear the screen

	HT1632C_Update_OnePage(p);				 //Displays one page of characters	

	delay(2000);							 //Wait for chips all ready
}



void loop(){

	unsigned char i=0,j=0,tmp=0;
	unsigned char *p;						 //Defines a pointer to the page array

	p=&page[0];								 //To point a pointer to an array address

	for(i=0,j=0;i<80;i++,j++,j++){		 //The entire array has 160 bytes, divided into 80 columns, 2 bytes each
		if(i<56){						 //For each screen displayed, the pointer moves back two bytes (that is, a column)
			HT1632C_Update_OnePage(p+j); 
		}else{								 
			HT1632C_Write_MultiRow(1,p+j,(80-i)-1);			
			HT1632C_Write_MultiRow((80-i),p,24-(80-i)+1);
		}
		delay(300);						 //The moving speed
	}

}