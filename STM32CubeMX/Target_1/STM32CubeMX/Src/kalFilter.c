#include "main.h"
#include "kalFilter.h"

void matrixMultiply(float* a, float* b, float* result, uint8_t aRows, uint8_t aCols, uint8_t bCols)
{
    for (uint8_t i = 0; i < aRows; i++)
    {
        for (uint8_t j = 0; j < bCols; j++)
        {
            //result[i * bCols + j] = 0.0f;
            result[i][j] = 0.0f;
            for (uint8_t k = 0; k < aCols; k++)
            {
                //result[i * bCols + j] += a[i * aCols + k] * b[k * bCols + j];
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void kalInit(Kal *kal,iniX x,iniQ Q)
{   
    kal->x[0][0] = x[0][0];
    kal->x[1][0] = x[1][0];

    kal->Q[0][0] = Q[0][0];
    kal->Q[0][1] = Q[0][1];
    kal->Q[1][0] = Q[1][0];
    kal->Q[1][1] = Q[1][1];
}

/*
*@para kal pointer to kal structure
*@para control input,Nu * 1
*xn+1,n = f * xn,n + G * un
*Pn+1,n = F * Pn,n * FT + Q
*/
void kalPredict(Kal* kal,u input)
{
    kal->x[0][0] = kal->x[0][0] + kal->v[0][0]*DELTA_T + 0.5f * input[0][0] *DELTA_T * DELTA_T;
    kal->x[1][0] = kal->x[1][0] + input[0][0]*DELTA_T;

    kal->P[0][0] = kal->P[0][0] + kal->P[1][0]*DELTA_T + kal->P[0][1]*DELTA_T + kal->P[1][1]*DELTA_T*DELTA_T + kal->Q[0][0];
    kal->P[0][1] = kal->P[0][1] + kal->P[1][1]*DELTA_T + kal->Q[0][1];
    kal->P[1][0] = kal->P[1][0] + kal->P[1][1]*DELTA_T + kal->Q[1][0];
    kal->P[1][1] = kal->P[1][1] + kal->Q[1][1];
}

/*
*@para measurement, Nz * 1
*@para cov covariance Matrix,Nz * Nz
*/
void kalUpdate(Kal *kal,z measure,R cov)
{
    K kalGain;

    float k_Middle = kal->P[0][0] + cov[0][0];
    kalGain[0][0] = kal->P[0][0] / k_Middle;
    kalGain[1][0] = kal->P[1][0] / k_Middle;

    float x_Middle = measure[0][0] - kal->x[0][0];
    kal->x[0][0] += kalGain[0][0] * k_Middle;
    kal->x[1][0] += kalGain[1][0] * k_Middle;

    kal->P[0][0] -= kalGain[0][0] * kal->P[0][0];
    kal->P[0][1] -= kalGain[0][0] * kal->P[0][1];
    kal->P[1][0] -= kalGain[1][0] * kal->P[0][0];
    kal->P[1][1] -= kalGain[1][0] * kal->P[0][1];
}

