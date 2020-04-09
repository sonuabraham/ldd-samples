/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* Change in X since last message */
dx = (signed char)(((byte_in[0] & 0x03) << 6) | (byte_in[1] & 0x3f));
/* Change in Y since last message */
dy = (signed char)(((byte_in[0] & 0x0c) << 4) | (byte_in[2] & 0x3f));
/* Is the left button pressed? */
left = (byte_in[0] & 0x20 ? 'L' : ' ');
/* Is the right button pressed? */
right = (byte_in[0] & 0x10 ? 'R' : ' ');
