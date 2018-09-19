/*
 * RCC.h
 *
 *  Created on: Jan. 6 2017
 *      Author: Mitchell Larson
 */

#ifndef RCC_H
#define RCC_H

#include <stdint.h>

#define DBP 8
#define RCC_RTCCLKSource 8
#define LSE_ON 1
#define RTC_ENABLE 15

typedef struct{
	uint32_t CR;
	uint32_t PLLCFGR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t AHB1RSTR;
	uint32_t AHB2RSTR;
	uint32_t AHB3RSTR;
	const uint32_t blank1;
	uint32_t APB1RSTR;
	uint32_t APB2RSTR;
	const uint32_t blank2;
	const uint32_t blank3;
	uint32_t AHB1ENR;
	uint32_t AHB2ENR;
	uint32_t AHB3ENR;
	const uint32_t blank4;
	uint32_t APB1ENR;
	uint32_t APB2ENR;
	const uint32_t blank5;
	const uint32_t blank6;
	uint32_t AHB1LPENR;
	uint32_t AHB2LPENR;
	uint32_t AHB3LPENR;
	const uint32_t blank7;
	uint32_t APB1LPENR;
	uint32_t APB2LPENR;
	const uint32_t blank8;
	const uint32_t blank9;
	uint32_t BDCR;
	uint32_t CSR;
	const uint32_t blank10;
	const uint32_t blank11;
	uint32_t SSCGR;
	uint32_t PLLI2SCFGR;
	uint32_t PLLSAICFGR;
	uint32_t DCKCFGR;
	uint32_t CKGATENR;
	uint32_t DCKCFGR2;
} RCC_Struct;

#endif
