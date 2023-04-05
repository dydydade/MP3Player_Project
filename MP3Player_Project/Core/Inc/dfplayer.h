/*
 * dfplayer.h
 *
 *  Created on: Feb 9, 2023
 *      Author: dydyd
 */

#ifndef INC_DFPLAYER_H_
#define INC_DFPLAYER_H_

#include <stdio.h>
#include "stm32f1xx_hal.h"

# define Start_Byte 0x7E             // 시작 값
# define Version_Byte 0xFF           // 버전 값
# define Command_Length 0x06        // 길이 값
# define End_Byte 0xEF              // 종료 값
# define Acknowledge 0x00           // 피드백 미 수신


void pause_Music();    //일시정지 명령어 0x0E를 파라미터로 전달

void reset_Music();    //일시정지 명령어 0x0E를 파라미터로 전달

void switch_NextMusic(); //next music 명령어 0x01를 파라미터로 전달

void specify_Volume(uint8_t level);        // 볼륨 조절 함수

void specify_Track(int16_t track);      // 트랙 지정 함수

void get_Current_Volume();  // 현재 Volume값 조회

void get_Current_Music();  // 현재 재생중인 곡 번호 조회

void get_Total_File_Count();  // mp3파일 개수 조회

void execute_CMD(uint8_t CMD, uint8_t Par1, uint8_t Par2);     // 시리얼 통신을 통해 실제 명령어구문을 전달하는 함수

uint8_t highByte(int16_t);

uint8_t lowByte(int16_t);

#endif /* INC_DFPLAYER_H_ */
