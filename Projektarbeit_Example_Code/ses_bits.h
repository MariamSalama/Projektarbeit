/*
 * ses_bits.h
 *
 *  Created on: 19.05.2016
 *      Author: Chris Kahlefendt
 */

#ifndef SES_BITS_H_
#define SES_BITS_H_

/* Set Bit */
#define SET_BIT(reg, bit) ((reg) |= (1 << (bit)))

/* Delete Bit */
#define CLEAR_BIT(reg, bit) ((reg) &= (unsigned)~(1 << (bit)))

/* Toggle Bit */
#define TOGGLE_BIT(reg,bit) ((reg) ^= (1 << (bit)))

/* Check Bit */
#define BIT_IS_SET(reg, bit) ((reg) & (1 << (bit)))
#define BIT_IS_CLEAR(reg, bit) !bit_is_set(reg, bit)



#endif /* SES_BITS_H_ */
