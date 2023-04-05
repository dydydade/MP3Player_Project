/*
 * dfplayer.c
 *
 *  Created on: Feb 9, 2023
 *      Author: dydyd
 */

#include <stdio.h>
#include "dfplayer.h"
#include "main.h"

extern UART_HandleTypeDef huart1;
extern uint8_t data[10];
extern uint8_t pauseStatus;

void pause_Music()
{
  execute_CMD(0x0E, 0x00, 0x00);     //일시정지 명령어 0x0E를 파라미터로 전달
}

void restart_Music()
{
	  execute_CMD(0x0D, 0x00, 0x00);     //restart 명령어 0x0D를 파라미터로 전달
}

void switch_NextMusic()
{
	  execute_CMD(0x01, 0x00, 0x00);     //next music 명령어 0x01를 파라미터로 전달
}

void specify_Volume(uint8_t level)        // 볼륨 조절 함수
{
  execute_CMD(0x06, 0x00, level);     //볼륨조절 명령어 0x06과 볼륨레벨을 파라미터로 전달
}

void specify_Track(int16_t track)      // 트랙 지정 함수
{
  execute_CMD(0x03, highByte(track), lowByte(track));      // 트랙재생 명령어 0x03과 재생될 트랙을 파라미터로 전달
  pauseStatus = 0;
}

void get_Current_Volume()  // 현재 Volume값 조회
{
  execute_CMD(0x43, highByte(0x00), lowByte(0x00));
  HAL_UART_Receive(&huart1, data, sizeof(data), 1000);
  printf("%d", data[6]);
}

void get_Current_Music()  // 현재 재생중인 곡 번호 조회
{
  execute_CMD(0x4C, highByte(0x00), lowByte(0x00));
  HAL_UART_Receive(&huart1, data, sizeof(data), 1000);
  printf("%d", data[6]);
}


void get_Total_File_Count()
{
  execute_CMD(0x48, highByte(0x00), lowByte(0x00));      // mp3파일 개수 조회 명령어 0x48과 재생될 트랙을 파라미터로 전달
  HAL_UART_Receive(&huart1, data, sizeof(data), 1000);
  printf("%d", data[6]);
}

void execute_CMD(uint8_t CMD, uint8_t Par1, uint8_t Par2)     // 시리얼 통신을 통해 실제 명령어구문을 전달하는 함수
{

  int16_t checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);   //체크섬계산
  // 보낼 명령어 구문을 배열에 저장
  uint8_t Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge, Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte};
  HAL_UART_Transmit(&huart1, Command_line, sizeof(Command_line), 1000);
//  for (byte k=0; k<10; k++)     // 명령어 구문을 시리얼 통신을 송신
//  {
//    mySerial.write(Command_line[k]);
//  }
}

uint8_t highByte(int16_t input)
{
	return (input>>8) & 0xff;
}

uint8_t lowByte(int16_t input)
{
	return input & 0xff;
}
