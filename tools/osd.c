//
//  OSD.c
//  DecklinkVideoGenerator
//
//  Created by Steven Toth on 7/11/12.
//  Copyright (c) 2012 Kernel Labs Inc. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "osd.h"

#define ALPHA_BACKGROUND 0

typedef unsigned char u8;

struct letter_t {
	u8 *ptr;
	u8 data[8];
} charset[] = 
{
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /*     */        { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* ' ' */          { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '!' */          { 0, { 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00 }, },
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000000 */
    /* 00000000 */
    /* 00000100 */
    /* 00000000 */
    /* '"' */          { 0, { 0x0a, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00001010 */
    /* 00001010 */
    /* 00001010 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '#' */          { 0, { 0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a, 0x00 }, },
    /* 00001010 */
    /* 00001010 */
    /* 00011111 */
    /* 00001010 */
    /* 00011111 */
    /* 00001010 */
    /* 00001010 */
    /* 00000000 */
    /* '$' */          { 0, { 0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04, 0x00 }, },
    /* 00000100 */
    /* 00001111 */
    /* 00010100 */
    /* 00001110 */
    /* 00000101 */
    /* 00011110 */
    /* 00000100 */
    /* 00000000 */
    /* '%' */          { 0, { 0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03, 0x00 }, },
    /* 00011000 */
    /* 00011001 */
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00010011 */
    /* 00000011 */
    /* 00000000 */
    /* '&' */          { 0, { 0x0c, 0x12, 0x14, 0x08, 0x15, 0x12, 0x0d, 0x00 }, },
    /* 00001100 */
    /* 00010010 */
    /* 00010100 */
    /* 00001000 */
    /* 00010101 */
    /* 00010010 */
    /* 00001101 */
    /* 00000000 */
    /* ''' */          { 0, { 0x0c, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00001100 */
    /* 00000100 */
    /* 00001000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '(' */          { 0, { 0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02, 0x00 }, },
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00001000 */
    /* 00001000 */
    /* 00000100 */
    /* 00000010 */
    /* 00000000 */
    /* ')' */          { 0, { 0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00 }, },
    /* 00001000 */
    /* 00000100 */
    /* 00000010 */
    /* 00000010 */
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00000000 */
    /* '*' */          { 0, { 0x00, 0x04, 0x15, 0x0e, 0x15, 0x04, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000100 */
    /* 00010101 */
    /* 00001110 */
    /* 00010101 */
    /* 00000100 */
    /* 00000000 */
    /* 00000000 */
    /* '+' */          { 0, { 0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000100 */
    /* 00000100 */
    /* 00011111 */
    /* 00000100 */
    /* 00000100 */
    /* 00000000 */
    /* 00000000 */
    /* ',' */          { 0, { 0x00, 0x00, 0x00, 0x00, 0x0c, 0x04, 0x08, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00001100 */
    /* 00000100 */
    /* 00001000 */
    /* 00000000 */
    /* '-' */          { 0, { 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00011111 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '.' */          { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00001100 */
    /* 00001100 */
    /* 00000000 */
    /* '/' */          { 0, { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000001 */
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00010000 */
    /* 00000000 */
    /* 00000000 */
    /* '0' */          { 0, { 0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010011 */
    /* 00010101 */
    /* 00011001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* '1' */          { 0, { 0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e, 0x00 }, },
    /* 00000100 */
    /* 00001100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00001110 */
    /* 00000000 */
    /* '2' */          { 0, { 0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00000001 */
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00011111 */
    /* 00000000 */
    /* '3' */          { 0, { 0x1f, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0e, 0x00 }, },
    /* 00011111 */
    /* 00000010 */
    /* 00000100 */
    /* 00000010 */
    /* 00000001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* '4' */          { 0, { 0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02, 0x00 }, },
    /* 00000010 */
    /* 00000110 */
    /* 00001010 */
    /* 00010010 */
    /* 00011111 */
    /* 00000010 */
    /* 00000010 */
    /* 00000000 */
    /* '5' */          { 0, { 0x1f, 0x10, 0x1e, 0x01, 0x01, 0x11, 0x0e, 0x00 }, },
    /* 00011111 */
    /* 00010000 */
    /* 00011110 */
    /* 00000001 */
    /* 00000001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* '6' */          { 0, { 0x06, 0x08, 0x10, 0x1e, 0x11, 0x11, 0x0e, 0x00 }, },
    /* 00000110 */
    /* 00001000 */
    /* 00010000 */
    /* 00011110 */
    /* 00010001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* '7' */          { 0, { 0x1f, 0x01, 0x02, 0x04, 0x04, 0x04, 0x04, 0x00 }, },
    /* 00011111 */
    /* 00000001 */
    /* 00000010 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000000 */
    /* '8' */          { 0, { 0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010001 */
    /* 00001110 */
    /* 00010001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* '9' */          { 0, { 0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x0c, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010001 */
    /* 00001111 */
    /* 00000001 */
    /* 00000010 */
    /* 00001100 */
    /* 00000000 */
    /* ':' */          { 0, { 0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x0c, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00001100 */
    /* 00001100 */
    /* 00000000 */
    /* 00001100 */
    /* 00001100 */
    /* 00000000 */
    /* 00000000 */
    /* ';' */          { 0, { 0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x04, 0x08, 0x00 }, },
    /* 00000000 */
    /* 00001100 */
    /* 00001100 */
    /* 00000000 */
    /* 00001100 */
    /* 00000100 */
    /* 00001000 */
    /* 00000000 */
    /* '<' */  { 0, { 0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02, 0x00 }, },
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00010000 */
    /* 00001000 */
    /* 00000100 */
    /* 00000010 */
    /* 00000000 */
    /* '=' */  { 0, { 0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00011111 */
    /* 00000000 */
    /* 00011111 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '>' */  { 0, { 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x00 }, },
    /* 00001000 */
    /* 00000100 */
    /* 00000010 */
    /* 00000001 */
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x0e, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00000001 */
    /* 00000010 */
    /* 00000100 */
    /* 00000000 */
    /* 00000100 */
    /* 00000000 */
    /* '@' */  { 0, { 0x0e, 0x11, 0x01, 0x0d, 0x15, 0x15, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00000001 */
    /* 00001101 */
    /* 00010101 */
    /* 00010101 */
    /* 00001110 */
    /* 00000000 */
    /* 'A' */  { 0, { 0x0e, 0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00011111 */
    /* 00010001 */
    /* 00010001 */
    /* 00000000 */
    /* 'B' */  { 0, { 0x1e, 0x09, 0x09, 0x0e, 0x09, 0x09, 0x1e, 0x00 }, },
    /* 00011110 */
    /* 00001001 */
    /* 00001001 */
    /* 00001110 */
    /* 00001001 */
    /* 00001001 */
    /* 00011110 */
    /* 00000000 */
    /* 'C' */  { 0, { 0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010000 */
    /* 00010000 */
    /* 00010000 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* 'D' */  { 0, { 0x1e, 0x09, 0x09, 0x09, 0x09, 0x09, 0x1e, 0x00 }, },
    /* 00011110 */
    /* 00001001 */
    /* 00001001 */
    /* 00001001 */
    /* 00001001 */
    /* 00001001 */
    /* 00011110 */
    /* 00000000 */
    /* 'E' */  { 0, { 0x1f, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x1f, 0x00 }, },
    /* 00011111 */
    /* 00010000 */
    /* 00010000 */
    /* 00011111 */
    /* 00010000 */
    /* 00010000 */
    /* 00011111 */
    /* 00000000 */
    /* 'F' */  { 0, { 0x1f, 0x10, 0x10, 0x1e, 0x10, 0x10, 0x10, 0x00 }, },
    /* 00011111 */
    /* 00010000 */
    /* 00010000 */
    /* 00011110 */
    /* 00010000 */
    /* 00010000 */
    /* 00010000 */
    /* 00000000 */
    /* 'G' */  { 0, { 0x0e, 0x11, 0x10, 0x13, 0x11, 0x11, 0x0f, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010000 */
    /* 00010011 */
    /* 00010001 */
    /* 00010001 */
    /* 00001111 */
    /* 00000000 */
    /* 'H' */  { 0, { 0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11, 0x00 }, },
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00011111 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00000000 */
    /* 'I' */  { 0, { 0x0e, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00001110 */
    /* 00000000 */
    /* 'J' */  { 0, { 0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0c, 0x00 }, },
    /* 00000111 */
    /* 00000010 */
    /* 00000010 */
    /* 00000010 */
    /* 00000010 */
    /* 00010010 */
    /* 00001100 */
    /* 00000000 */
    /* 'K' */  { 0, { 0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11, 0x00 }, },
    /* 00010001 */
    /* 00010010 */
    /* 00010100 */
    /* 00011000 */
    /* 00010100 */
    /* 00010010 */
    /* 00010001 */
    /* 00000000 */
    /* 'L' */  { 0, { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x00 }, },
    /* 00010000 */
    /* 00010000 */
    /* 00010000 */
    /* 00010000 */
    /* 00010000 */
    /* 00010000 */
    /* 00011111 */
    /* 00000000 */
    /* 'M' */  { 0, { 0x11, 0x1b, 0x15, 0x15, 0x11, 0x11, 0x11, 0x00 }, },
    /* 00010001 */
    /* 00011011 */
    /* 00010101 */
    /* 00010101 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00000000 */
    /* 'N' */  { 0, { 0x11, 0x19, 0x19, 0x15, 0x13, 0x13, 0x11, 0x00 }, },
    /* 00010001 */
    /* 00011001 */
    /* 00011001 */
    /* 00010101 */
    /* 00010011 */
    /* 00010011 */
    /* 00010001 */
    /* 00000000 */
    /* 'O' */  { 0, { 0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* 'P' */  { 0, { 0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10, 0x00 }, },
    /* 00011110 */
    /* 00010001 */
    /* 00010001 */
    /* 00011110 */
    /* 00010000 */
    /* 00010000 */
    /* 00010000 */
    /* 00000000 */
    /* 'Q' */  { 0, { 0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x1d, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010101 */
    /* 00010010 */
    /* 00011101 */
    /* 00000000 */
    /* 'R' */  { 0, { 0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11, 0x00 }, },
    /* 00011110 */
    /* 00010001 */
    /* 00010001 */
    /* 00011110 */
    /* 00010100 */
    /* 00010010 */
    /* 00010001 */
    /* 00000000 */
    /* 'S' */  { 0, { 0x0e, 0x11, 0x10, 0x0e, 0x01, 0x11, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00010001 */
    /* 00010000 */
    /* 00001110 */
    /* 00000001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* 'T' */  { 0, { 0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00 }, },
    /* 00011111 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000000 */
    /* 'U' */  { 0, { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e, 0x00 }, },
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* 'V' */  { 0, { 0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04, 0x00 }, },
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00001010 */
    /* 00000100 */
    /* 00000000 */
    /* 'W' */  { 0, { 0x11, 0x11, 0x11, 0x15, 0x15, 0x1b, 0x11, 0x00 }, },
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010101 */
    /* 00010101 */
    /* 00011011 */
    /* 00010001 */
    /* 00000000 */
    /* 'X' */  { 0, { 0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11, 0x00 }, },
    /* 00010001 */
    /* 00010001 */
    /* 00001010 */
    /* 00000100 */
    /* 00001010 */
    /* 00010001 */
    /* 00010001 */
    /* 00000000 */
    /* 'Y' */  { 0, { 0x11, 0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x00 }, },
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00001010 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000000 */
    /* 'Z' */  { 0, { 0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f, 0x00 }, },
    /* 00011111 */
    /* 00000001 */
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00010000 */
    /* 00011111 */
    /* 00000000 */
    /* '[' */  { 0, { 0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00001000 */
    /* 00001000 */
    /* 00001000 */
    /* 00001000 */
    /* 00001000 */
    /* 00001110 */
    /* 00000000 */
    /* '\' */  { 0, { 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00 }, },
    /* 00010000 */
    /* 00001000 */
    /* 00000100 */
    /* 00000010 */
    /* 00000001 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* ']' */  { 0, { 0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e, 0x00 }, },
    /* 00001110 */
    /* 00000010 */
    /* 00000010 */
    /* 00000010 */
    /* 00000010 */
    /* 00000010 */
    /* 00001110 */
    /* 00000000 */
    /* '^' */  { 0, { 0x04, 0x0a, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000100 */
    /* 00001010 */
    /* 00010001 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '_' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00011111 */
    /* 00000000 */
    /* '`' */  { 0, { 0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00010000 */
    /* 00001000 */
    /* 00000100 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 'a' */  { 0, { 0x00, 0x00, 0x0e, 0x01, 0x0f, 0x11, 0x0f, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00001110 */
    /* 00000001 */
    /* 00001111 */
    /* 00010001 */
    /* 00001111 */
    /* 00000000 */
    /* 'b' */  { 0, { 0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x1e, 0x00 }, },
    /* 00010000 */
    /* 00010000 */
    /* 00010110 */
    /* 00011001 */
    /* 00010001 */
    /* 00010001 */
    /* 00011110 */
    /* 00000000 */
    /* 'c' */  { 0, { 0x00, 0x00, 0x0e, 0x11, 0x10, 0x11, 0x0e, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00001110 */
    /* 00010001 */
    /* 00010000 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* 'd' */  { 0, { 0x01, 0x01, 0x0d, 0x13, 0x11, 0x11, 0x0f, 0x00 }, },
    /* 00000001 */
    /* 00000001 */
    /* 00001101 */
    /* 00010011 */
    /* 00010001 */
    /* 00010001 */
    /* 00001111 */
    /* 00000000 */
    /* 'e' */  { 0, { 0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0e, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00001110 */
    /* 00010001 */
    /* 00011111 */
    /* 00010000 */
    /* 00001110 */
    /* 00000000 */
    /* 'f' */  { 0, { 0x02, 0x05, 0x04, 0x0e, 0x04, 0x04, 0x04, 0x00 }, },
    /* 00000010 */
    /* 00000101 */
    /* 00000100 */
    /* 00001110 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000000 */
    /* 'g' */  { 0, { 0x00, 0x0d, 0x13, 0x13, 0x0d, 0x01, 0x0e, 0x00 }, },
    /* 00000000 */
    /* 00001101 */
    /* 00010011 */
    /* 00010011 */
    /* 00001101 */
    /* 00000001 */
    /* 00001110 */
    /* 00000000 */
    /* 'h' */  { 0, { 0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x11, 0x00 }, },
    /* 00010000 */
    /* 00010000 */
    /* 00010110 */
    /* 00011001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00000000 */
    /* 'i' */  { 0, { 0x04, 0x00, 0x0c, 0x04, 0x04, 0x04, 0x0e, 0x00 }, },
    /* 00000100 */
    /* 00000000 */
    /* 00001100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00001110 */
    /* 00000000 */
    /* 'j' */  { 0, { 0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0c, 0x00 }, },
    /* 00000010 */
    /* 00000000 */
    /* 00000110 */
    /* 00000010 */
    /* 00000010 */
    /* 00010010 */
    /* 00001100 */
    /* 00000000 */
    /* 'k' */  { 0, { 0x08, 0x08, 0x09, 0x0a, 0x0c, 0x0a, 0x09, 0x00 }, },
    /* 00001000 */
    /* 00001000 */
    /* 00001001 */
    /* 00001010 */
    /* 00001100 */
    /* 00001010 */
    /* 00001001 */
    /* 00000000 */
    /* 'l' */  { 0, { 0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e, 0x00 }, },
    /* 00001100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00001110 */
    /* 00000000 */
    /* 'm' */  { 0, { 0x00, 0x00, 0x1a, 0x15, 0x15, 0x15, 0x15, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00011010 */
    /* 00010101 */
    /* 00010101 */
    /* 00010101 */
    /* 00010101 */
    /* 00000000 */
    /* 'n' */  { 0, { 0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00010110 */
    /* 00011001 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00000000 */
    /* 'o' */  { 0, { 0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00001110 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00001110 */
    /* 00000000 */
    /* 'p' */  { 0, { 0x00, 0x16, 0x19, 0x19, 0x16, 0x10, 0x10, 0x00 }, },
    /* 00000000 */
    /* 00010110 */
    /* 00011001 */
    /* 00011001 */
    /* 00010110 */
    /* 00010000 */
    /* 00010000 */
    /* 00000000 */
    /* 'q' */  { 0, { 0x00, 0x0d, 0x13, 0x13, 0x0d, 0x01, 0x01, 0x00 }, },
    /* 00000000 */
    /* 00001101 */
    /* 00010011 */
    /* 00010011 */
    /* 00001101 */
    /* 00000001 */
    /* 00000001 */
    /* 00000000 */
    /* 'r' */  { 0, { 0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00010110 */
    /* 00011001 */
    /* 00010000 */
    /* 00010000 */
    /* 00010000 */
    /* 00000000 */
    /* 's' */  { 0, { 0x00, 0x00, 0x0f, 0x10, 0x1e, 0x01, 0x1f, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00001111 */
    /* 00010000 */
    /* 00011110 */
    /* 00000001 */
    /* 00011111 */
    /* 00000000 */
    /* 't' */  { 0, { 0x08, 0x08, 0x1c, 0x08, 0x08, 0x09, 0x06, 0x00 }, },
    /* 00001000 */
    /* 00001000 */
    /* 00011100 */
    /* 00001000 */
    /* 00001000 */
    /* 00001001 */
    /* 00000110 */
    /* 00000000 */
    /* 'u' */  { 0, { 0x00, 0x00, 0x12, 0x12, 0x12, 0x12, 0x0d, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00010010 */
    /* 00010010 */
    /* 00010010 */
    /* 00010010 */
    /* 00001101 */
    /* 00000000 */
    /* 'v' */  { 0, { 0x00, 0x00, 0x11, 0x11, 0x11, 0x0a, 0x04, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00010001 */
    /* 00010001 */
    /* 00010001 */
    /* 00001010 */
    /* 00000100 */
    /* 00000000 */
    /* 'w' */  { 0, { 0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00010001 */
    /* 00010001 */
    /* 00010101 */
    /* 00010101 */
    /* 00001010 */
    /* 00000000 */
    /* 'x' */  { 0, { 0x00, 0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00010001 */
    /* 00001010 */
    /* 00000100 */
    /* 00001010 */
    /* 00010001 */
    /* 00000000 */
    /* 'y' */  { 0, { 0x00, 0x00, 0x11, 0x11, 0x13, 0x0d, 0x01, 0x0e }, },
    /* 00000000 */
    /* 00000000 */
    /* 00010001 */
    /* 00010001 */
    /* 00010011 */
    /* 00001101 */
    /* 00000001 */
    /* 00001110 */
    /* 'z' */  { 0, { 0x00, 0x00, 0x1f, 0x02, 0x04, 0x08, 0x1f, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00011111 */
    /* 00000010 */
    /* 00000100 */
    /* 00001000 */
    /* 00011111 */
    /* 00000000 */
    /* '{' */  { 0, { 0x02, 0x04, 0x04, 0x08, 0x04, 0x04, 0x02, 0x00 }, },
    /* 00000010 */
    /* 00000100 */
    /* 00000100 */
    /* 00001000 */
    /* 00000100 */
    /* 00000100 */
    /* 00000010 */
    /* 00000000 */
    /* '|' */  { 0, { 0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04, 0x00 }, },
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000000 */
    /* 00000100 */
    /* 00000100 */
    /* 00000100 */
    /* 00000000 */
    /* '}' */  { 0, { 0x08, 0x04, 0x04, 0x02, 0x04, 0x04, 0x08, 0x00 }, },
    /* 00001000 */
    /* 00000100 */
    /* 00000100 */
    /* 00000010 */
    /* 00000100 */
    /* 00000100 */
    /* 00001000 */
    /* 00000000 */
    /* '~' */  { 0, { 0x08, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00001000 */
    /* 00010101 */
    /* 00000010 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* '?' */  { 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, },
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
    /* 00000000 */
};

int vc8x0_display_init(struct vc8x0_display_context *ctx)
{
	memset(ctx, 0, sizeof(*ctx));
    
	ctx->plotwidth = 8; /* The 8bit char is rendered as 32x32 */
	ctx->plotheight = ctx->plotwidth;
    
	ctx->currx = 0;
	ctx->curry = 0;
    
	return 0;
}

static int vc8x0_display_render_moveto(struct vc8x0_display_context *ctx, int x, int y)
{
	ctx->ptr = ctx->frame + (x * (ctx->plotwidth * 2));
	ctx->ptr += (y * (ctx->stride * ctx->plotheight));
    
	ctx->currx = x;
	ctx->curry = y;
    
	/* Black */
	ctx->bg[0] = 0x00;
	ctx->bg[1] = 0x00;
    
	/* Slight off white, closer to grey */
	ctx->fg[0] = 0x04;
	ctx->fg[1] = 0x04;
    
	return 0;
}

int vc8x0_display_render_character(struct vc8x0_display_context *ctx, u8 letter)
{
	u8 line;
    
	if (letter > 0x9f)
		return -1;

	int xmultiplier = ctx->plotwidth / 8;
	int ymultiplier = ctx->plotheight / 8;
	for (int i = 0; i < 8; i++) {
       
		for (int y = 0; y < ymultiplier; y++) { 
			line = charset[ letter ].data[ i ];
			for (int j = 0; j < 8; j++) {
				if (line & 0x80) {
					/* font color */
					for (int x = 0; x < xmultiplier; x++) {
						*(ctx->ptr++) = ctx->fg[0];
						*(ctx->ptr++) = ctx->fg[1];
					}
				} else {
					/* background color */
					/* Complete black background */
					for (int x = 0; x < xmultiplier; x++) {
						*(ctx->ptr++) = ctx->bg[0];
						*(ctx->ptr++) = ctx->bg[1];
					}
				}
       	        
				line <<= 1;
			}
			ctx->ptr += (ctx->stride - (ctx->plotwidth * 2));
		}
	}
    
	return 0;
}

static int vc8x0_display_render_ascii(struct vc8x0_display_context *ctx, u8 letter, int x, int y)
{
	if (letter > 0x9f)
		return -1;
    
	vc8x0_display_render_moveto(ctx, x, y);
	vc8x0_display_render_character(ctx, letter);
    
	return 0;
}

int vc8x0_display_render_string(struct vc8x0_display_context *ctx, const char *s, int len, int x, int y)
{
	int i;
    
	for (i = 0; i < len; i++)
		vc8x0_display_render_ascii(ctx, *(s + i), x + i, y);
    
	return 0;
}

int vc8x0_display_render_reset(struct vc8x0_display_context *ctx, u8 *ptr, int width, long stride)
{
	if (!ptr)
		return -1;

	ctx->width = width;

	switch (stride) {
        case 640:
        case 800:
        case 720:
        case 768:
        case 1024:
        case 1280:
        case 1400:
        case 1440:
        case 1536: /* YUV422p10 NTSC stride */
        case 1600:
        case 1680:
        case 1920:
        case 2048: /* Black Magic Extreme 3D 2K formats */
        case 2560: /* YUV422p10 1280 stride */
        case 3840: /* YUV422p10 1920 stride */
        case 5120: /* RGBA */
            break;
        default:
            printf("%s() illegal stride %ld\n", __func__, stride);
            return -1;
	}
    
	ctx->ptr = ptr;
	ctx->frame = ptr;
	ctx->stride = (int)stride;
    
	ctx->plotwidth = 16;
	ctx->plotheight = 16;
    
	vc8x0_display_render_moveto(ctx, 0, 0);
    
	return 0;
}

