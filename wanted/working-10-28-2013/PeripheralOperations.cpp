/*
 * PeripheralOperations.cpp
 *
 *  Created on: Aug 5, 2013
 *  Author: sudhir vyasaraja
 */

#include <stdio.h>
#include <stdlib.h>

/*_________________________________________________________________________________________________*/

/*Function performs the proper shifting of laser state matrix
 *such that the lasers fire at proper time. Laser1 and Laser2 are the matrices controlling
 *the lasers whereas BufferLaser holds the state of Laser2 until the next cycle of
 *change to make up for the distance between Laser1 and Laser2
 */
void ChangeLaserStates(int(*LaserState)[16], unsigned int* BufferLaser, unsigned int* Laser1, unsigned int* Laser2)
{
	int i, j;
	int temp1 = 0;
	int temp2 = 0;
	for(i=0; i<8; i++)
	{
		Laser2[i] = BufferLaser[i];									//Load the Laser2 matrix from the previous change cycle
	}
	for(i=0; i<16; i++)
	{
		if(i%2 ==0)
		{
			BufferLaser[temp1] = LaserState[15][i];
			temp1++;
		}
		else
		{
			Laser1[temp2] = LaserState[15][i];						//Laser1 and Laser2 get the alternate bits
			temp2++;
		}
	}
	for (i=0; i<15; i++)
	{
		for (j=0; j<16; j++)
		{
			LaserState[15-i][j] = LaserState[15-i-1][j];			//Shift all the rows one row down
		}
	}
	for (j=0; j<16; j++)											//Populate the first row of LaserState with all 0s
		LaserState[0][j] = 0;
}
/*_________________________________________________________________________________________________*/

/*_________________________________________________________________________________________________*/

