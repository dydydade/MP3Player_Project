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


void pauseMusic();    //일시정지 명령어 0x0E를 파라미터로 전달

void restartMusic();    //일시정지 명령어 0x0E를 파라미터로 전달

void switchNextMusic(); //next music 명령어 0x01를 파라미터로 전달

void specifyVolume(uint8_t level);        // 볼륨 조절 함수

void specifyTrack(int16_t track);      // 트랙 지정 함수

uint8_t getCurrentVolume();  // 현재 Volume값 조회

uint8_t getCurrentTrackNo();  // 현재 재생중인 곡 번호 조회

uint8_t getTotalFileCount();  // mp3파일 개수 조회

void executeCMD(uint8_t CMD, uint8_t Par1, uint8_t Par2);     // 시리얼 통신을 통해 실제 명령어구문을 전달하는 함수

uint8_t highByte(int16_t);

uint8_t lowByte(int16_t);

#endif /* INC_DFPLAYER_H_ */
