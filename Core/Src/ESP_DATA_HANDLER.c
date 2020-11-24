/**
  ******************************************************************************

  File:		ESP DATA HANDLER
  Author:   ControllersTech
  Updated:  3rd Aug 2020

  ******************************************************************************
  Copyright (C) 2017 ControllersTech.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  ******************************************************************************
*/



#include "ESP_DATA_HANDLER.h"
#include "UartRingbuffer.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart3;

#define wifi_uart &huart3
//#define server_uart &huart1

#define maxnumberofusers  10  // Max number of users

char buffer[20];
userDetails user[maxnumberofusers];

int usernumber = 0;

extern char time;
extern char date;
extern float temper;
extern float Lux;
extern uint8_t check;
int buzz = 0;
extern uint8_t init_time;
extern uint8_t init_date;
extern int Hours;
extern int Minutes;
extern int Seconds;
extern int Date;
extern int Month;
extern int Year;
int HoursAdd=13;
int MinutesAdd=35;
int SecondAdd=0;
uint8_t HoursAlarm=0;
uint8_t MinutesAlarm=0;
uint8_t SecondAlarm=0;

int sizeofuser (userDetails *user)
{
	int size=0;
	while (user[size].firstname[0] != '\0') size++;
	return size+1;
}


char *home = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<p>Enter the Details in the form below: </p>\n\
		<form action=\"/page1\">\n\
		<label for=\"fname\">First Name:</label><br>\n\
		<input type=\"text\" id=\"fname\" name=\"fname\" value=\"\"><br><br>\n\
		<label for=\"lname\">Last Name:</label><br>\n\
		<input type=\"text\" id=\"lname\" name=\"lname\" value=\"\"><br><br>\n\
		<label for=\"age\">Age:</label><br>\n\
		<input type=\"number\" id=\"age\" name=\"age\" value=\"\"><br><br>\n\
		<label for=\"time\">Age:</label><br>\n\
		<input type=\"number\" id=\"age\" name=\"age\" value=\"\"><br><br>\n\
		<input type=\"submit\" value=\"Submit\">\n\
		</form><br><br>\n\
		<form action=\"/page2\">\n\
		<input type=\"submit\" value=\"View Data\">\n\
		</form>\n\
		<form action=\"/temper\">\n\
		<input type=\"submit\" value=\"View Temperature\">\n\
		</form>\n\
		<form action=\"/lux\">\n\
		<input type=\"submit\" value=\"Light intensity \">\n\
		</form>\n\
		<form action=\"/time\">\n\
		<input type=\"submit\" value=\"Time \">\n\
		</form>\n\
		<form action=\"/led\">\n\
		<input type=\"submit\" value=\"Light Control\">\n\
		</form>\n\
		</body></html>";

char *set_time = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<p>Set the alarm below: </p>\n\
		<form action=\"/time_change\">\n\
		<label for=\"hour\">Hour:</label><br>\n\
		<input type=\"number\" id=\"hour\" name=\"hour\" value=\"\"><br><br>\n\
		<label for=\"minutes\">Minutes:</label><br>\n\
		<input type=\"number\" id=\"minutes\" name=\"minutes\" value=\"\"><br><br>\n\
		<label for=\"seconds\">Seconds:</label><br>\n\\
		<input type=\"number\" id=\"seconds\" name=\"seconds\" value=\"\"><br><br>\n\
		<input type=\"submit\" value=\"Submit\">\n\
		</form><br><br>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Return\">\n\
		</form>\n\
		</body></html>";

char *set_alarm = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<p>Set the time below: </p>\n\
		<form action=\"/alarm_change\">\n\
		<label for=\"hour\">Hour:</label><br>\n\
		<input type=\"number\" id=\"hour\" name=\"hour\" value=\"\"><br><br>\n\
		<label for=\"minutes\">Minutes:</label><br>\n\
		<input type=\"number\" id=\"minutes\" name=\"minutes\" value=\"\"><br><br>\n\
		<input type=\"submit\" value=\"Submit\">\n\
		</form><br><br>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Return\">\n\
		</form>\n\
		</body></html>";

char *alarm_change = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> Alarm Set Successfully </h2>\n\
		<p> Click Below to Return </p>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Return\">\n\
		</form><br><br>\n\
		</body></html>";

char *page1 = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> DATA STORED Successfully </h2>\n\
		<p> Click Below to Submit again </p>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Submit Again\">\n\
		</form><br><br>\n\
		<form action=\"/page2\">\n\
		<input type=\"submit\" value=\"View Data\">\n\
		</form>\n\
		</body></html>";

char *time_change = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> Time Set Successfully </h2>\n\
		<p> Click Below to Return </p>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Return\">\n\
		</form>\n\
		</body></html>";



char *led_on = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> LED Status: ON </h2>\n\
		<p> click below to off LED or go back to Main Page  </p>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Back\">\n\
		</form><br><br>\n\
		<form action=\"/led_off\">\n\
		<input type=\"submit\" value=\"LED OFF\">\n\
		</form>\n\
		</body></html>";

char *led_off = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> LED Status: OFF </h2>\n\
		<p> click below to on LED or go back to Main Page  </p>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Back\">\n\
		</form><br><br>\n\
		<form action=\"/led_on\">\n\
		<input type=\"submit\" value=\"LED ON\">\n\
		</form>\n\
		</body></html>";

char *led;

char *page2_Top = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> DATA CCOLLECTED is Shown BELOW </h2>\n";

char *page2_end = "<p> Click Below to Submit again </p>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Submit again\">\n\
		</body></html>";

char *time_end = "<p> Click Below to Submit again </p>\n\
		<form action=\"/set_time\">\n\
		<input type=\"submit\" value=\"Set Time\">\n\
		</form><br><br>\n\
		<form action=\"/set_alarm\">\n\
		<input type=\"submit\" value=\"Set Alarm\">\n\
		</form><br><br>\n\
		<form action=\"/home\">\n\
		<input type=\"submit\" value=\"Return\">\n\
		</form><br><br>\n\
		</body></html>";

char* temperature_Top = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> Temperature is Shown BELOW </h2>\n";

char* lux_Top = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> Light intensity is Shown BELOW </h2>\n";

char* time_Top = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>ESP8266 Smart Home Driver</h1>\n\
		<h2> Time is Shown BELOW </h2>\n";

char *table = "<style>table {  font-family: arial, sans-serif;\
		border-collapse: collapse;  width: 50%;}\
		td, th {  border: 1px solid #dddddd;\
		text-align: left;  padding: 8px;}tr:nth-child(even)\
		{  background-color: #dddddd;}</style><table><tr><th>Name</th><th>Age</th></tr>";

char *table_temp = "<style>table {  font-family: arial, sans-serif;\
		border-collapse: collapse;  width: 50%;}\
		td, th {  border: 1px solid #dddddd;\
		text-align: left;  padding: 8px;}tr:nth-child(even)\
		{  background-color: #dddddd;}</style><table><tr><th>Temperature</th><th>Unit</th></tr>";

char *table_lux = "<style>table {  font-family: arial, sans-serif;\
		border-collapse: collapse;  width: 50%;}\
		td, th {  border: 1px solid #dddddd;\
		text-align: left;  padding: 8px;}tr:nth-child(even)\
		{  background-color: #dddddd;}</style><table><tr><th>Intensity</th><th>Unit</th></tr>";

char *table_time = "<style>table {  font-family: arial, sans-serif;\
		border-collapse: collapse;  width: 50%;}\
		td, th {  border: 1px solid #dddddd;\
		text-align: left;  padding: 8px;}tr:nth-child(even)\
		{  background-color: #dddddd;}</style><table><tr><th>Time</th></tr>";

/*****************************************************************************************************************************************/

void ESP_Init (char *SSID, char *PASSWD, char *STAIP)
{
	char data[80];

	Ringbuf_init();

	Uart_sendstring("AT+RST\r\n");
	HAL_Delay(2000);

	/********* AT **********/
	//TEST
	Uart_flush();
	Uart_sendstring("AT\r\n");
	while(!(Wait_for("OK\r\n")));


	/********* AT+CWMODE=1 **********/
	//Tryb Wi-Fi 1->Stacja, 2->AP
	Uart_flush();
	Uart_sendstring("AT+CWMODE=1\r\n");
	while (!(Wait_for("OK\r\n")));

	/* Set Static IP Address */
	/********* AT+CWSTAIP=IPADDRESS **********/
	Uart_flush();
	sprintf (data, "AT+CIPSTA=\"%s\"\r\n", STAIP);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")));

	/********* AT+CWJAP="SSID","PASSWD" **********/
	Uart_flush();
	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")));

	/********* AT+CIPMUX **********/
	Uart_flush();
	Uart_sendstring("AT+CIPMUX=1\r\n");
	while (!(Wait_for("OK\r\n")));

	/********* AT+CIPSERVER **********/
	Uart_flush();
	Uart_sendstring("AT+CIPSERVER=1,80\r\n");
	while (!(Wait_for("OK\r\n")));

}


int Server_Send (char *str, int Link_ID)
{
	int len = strlen (str);
	char data[80];
	Uart_flush();
	sprintf (data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	Uart_sendstring(data);
	while (!(Wait_for(">")));
	Uart_sendstring (str);
	while (!(Wait_for("SEND OK")));
	Uart_flush();
	sprintf (data, "AT+CIPCLOSE=%d\r\n",Link_ID);
	Uart_sendstring(data);
	while (!(Wait_for("OK\r\n")));
	return 1;
}

void Server_Handle (char *str, int Link_ID)
{
	char datatosend[4096] = {0};
	if (!(strcmp (str, "/page1")))
	{

		sprintf(datatosend, page1);
		Server_Send(datatosend, Link_ID);
	}


	else if (!(strcmp (str, "/page2")))
	{
		char localbuf[2048];
		sprintf(datatosend, page2_Top);
		strcat (datatosend, table);
		int bufsize = (sizeofuser (user));
		for (int i=0; i<bufsize; i++)
		{
			sprintf (localbuf, "<tr><td>%s %s</td>	<td>%s</td></tr>",user[i].firstname,user[i].lastname,user[i].age);
			strcat (datatosend, localbuf);
		}
		strcat (datatosend, "</table>");
		strcat(datatosend, page2_end);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/set_time")))
	{
		sprintf(datatosend, set_time);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/set_alarm")))
	{
		sprintf(datatosend, set_alarm);
		Server_Send(datatosend, Link_ID);
	}


	else if (!(strcmp (str, "/time_change")))
	{
		sprintf(datatosend, time_change);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/alarm_change")))
	{
		sprintf(datatosend, alarm_change);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/temper")))
	{

		char localbuf[2048];
		sprintf(datatosend, temperature_Top);
		strcat (datatosend, table_temp);
		int bufsize = (sizeofuser (user));
		for (int i=0; i<bufsize; i++)
		{
			sprintf (localbuf, "<tr><td>%f</td><td>°C</td></tr>", temper);
			strcat (datatosend, localbuf);
		}
		strcat (datatosend, "</table>");
		strcat(datatosend, page2_end);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/time")))
	{

		char localbuf[2048];
		sprintf(datatosend, time_Top);
		strcat (datatosend, table_time);
		int bufsize = (sizeofuser (user));
		for (int i=0; i<bufsize; i++)
		{
			sprintf (localbuf, "<tr><td>%i:%i:%i</td></tr>",Hours,Minutes,Seconds);
			strcat (datatosend, localbuf);
		}
		strcat (datatosend, "</table>");
		strcat(datatosend, time_end);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/lux")))
	{

		char localbuf[2048];
		sprintf(datatosend, lux_Top);
		strcat (datatosend, table_lux);
		int bufsize = (sizeofuser (user));
		for (int i=0; i<bufsize; i++)
		{
			sprintf (localbuf, "<tr><td>%f</td><td>Lux</td></tr>", Lux);
			strcat (datatosend, localbuf);
		}
		strcat (datatosend, "</table>");
		strcat(datatosend, page2_end);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/led")))
	{

		sprintf(datatosend, led);
		strcat(datatosend, page2_end);
		Server_Send(datatosend, Link_ID);
	}
	else
	{
		sprintf (datatosend, home);
		Server_Send(datatosend, Link_ID);
	}

}

void Server_Start (void)
{
	char buftostoreheader[128] = {0};
	char Link_ID;
	while (!(Get_after("+IPD,", 1, &Link_ID)));

	Link_ID -= 48;
	while (!(Copy_upto(" HTTP/1.1", buftostoreheader)));

	if (Look_for("/page1", buftostoreheader) == 1)
	{
		GetDataFromBuffer("fname=", "&", buftostoreheader, user[usernumber].firstname);
		GetDataFromBuffer("lname=", "&", buftostoreheader, user[usernumber].lastname);
		GetDataFromBuffer("age=", " HTTP", buftostoreheader, user[usernumber].age);
		usernumber++;
		if (usernumber >9) usernumber = 0;
		Server_Handle("/page1",Link_ID);
		HAL_Delay(500);
	}

	else if (Look_for("/time_change", buftostoreheader) == 1)
	{

		GetDataFromBuffer("hour=", " HTTP", buftostoreheader, user[usernumber].hour);
		HoursAdd = atoi(user[usernumber].hour);
		GetDataFromBuffer("minutes=", "HTTP", buftostoreheader, user[usernumber].minutes);
		MinutesAdd = atoi(user[usernumber].minutes);
		GetDataFromBuffer("seconds=", "HTTP", buftostoreheader, user[usernumber].seconds);
		SecondAdd = atoi(user[usernumber].seconds);
		usernumber++;
		if (usernumber >9) usernumber = 0;
		init_time = 1;
		Server_Handle("/time_change",Link_ID);
		HAL_Delay(500);
	}

	 else if (Look_for("/alarm_change", buftostoreheader) == 1)
	{

		GetDataFromBuffer("minutes=", " HTTP", buftostoreheader, user[usernumber].minutes);
		MinutesAlarm = atoi(user[usernumber].minutes);
		GetDataFromBuffer("hour=", " HTTP", buftostoreheader, user[usernumber].hour);
		HoursAlarm = atoi(user[usernumber].hour);
		usernumber++;
		if (usernumber >9) usernumber = 0;
		buzz = 1;
		Server_Handle("/alarm_change",Link_ID);
		HAL_Delay(500);
	}


	else if (Look_for("/page2", buftostoreheader) == 1)
	{
		Server_Handle("/page2",Link_ID);
		HAL_Delay(500);
	}

	else if (Look_for("/temper", buftostoreheader) == 1)
	{
		Server_Handle("/temper",Link_ID);
		HAL_Delay(500);
	}
	else if (Look_for("/lux", buftostoreheader) == 1)
	{
		Server_Handle("/lux",Link_ID);
		HAL_Delay(500);
	}
	else if (Look_for("/time", buftostoreheader) == 1)
	{
		Server_Handle("/time",Link_ID);
		HAL_Delay(500);
	}

	else if (Look_for("/led", buftostoreheader) == 1)
		{
			if(check == 1)
				{led=led_on;
				check = 0;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
				}
			else
				{led=led_off;
				check = 1;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
				}

			Server_Handle("/led",Link_ID);
			HAL_Delay(500);

		}
	else if (Look_for("/set_time", buftostoreheader) == 1)
	{
		Server_Handle("/set_time",Link_ID);
		HAL_Delay(500);
	}

	else if (Look_for("/set_alarm", buftostoreheader) == 1)
	{
		Server_Handle("/set_alarm",Link_ID);
		HAL_Delay(500);
	}

	else if (Look_for("/home", buftostoreheader) == 1)
	{
		Server_Handle("/home",Link_ID);
		HAL_Delay(500);
	}

	else if (Look_for("/favicon.ico", buftostoreheader) == 1);

	else
	{
		Server_Handle("/ ", Link_ID);
		HAL_Delay(500);
	}
}



