#include "main.h"
#include "kalFilter.h"

#define DELTA_T MOTO_CONTROL_PERIOD_S
#define DELTA_T_SQUARE  (DELTA_T * DELTA_T)
#define DELTA_T_CUBE    (DELTA_T * DELTA_T * DELTA_T)  
#define DELTA_T_QUARE   (DELTA_T * DELTA_T * DELTA_T * DELTA_T)

void matrixMultiply(float* a, float* b, float* result, uint8_t aRows, uint8_t aCols, uint8_t bCols)
{
    for (uint8_t i = 0; i < aRows; i++)
    {
        for (uint8_t j = 0; j < bCols; j++)
        {
            result[i * aCols + j] = 0.0f;
            for (uint8_t k = 0; k < aCols; k++)
            {
                result[i * aCols + j] += a[i * aCols + k] * b[k * bCols + j];
            }
        }
    }
}

void matrixAdd(float* a, float* b, float* result, uint8_t rows, uint8_t cols)
{
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            result[i * cols + j] = a[i * cols + j] + b[i * cols + j];
        }
    }
}

void kalInit(Kal *kal,iniX x,iniP P,Qv var,float motoInputPerDamping)
{   
    kal->x[0][0] = x[0][0];
    kal->x[1][0] = x[1][0];

    kal->P[0][0] = P[0][0];
    kal->P[0][1] = P[0][1];
    kal->P[1][0] = P[1][0];
    kal->P[1][1] = P[1][1];

    kal->Q[0][0] = DELTA_T_SQUARE * var * var;
    kal->Q[0][1] = DELTA_T * var * var;
    kal->Q[1][0] = DELTA_T * var * var;
    kal->Q[1][1] = var * var;

    kal->k = motoInputPerDamping;
}

/*
*@para kal pointer to kal structure
*@para control input,Nu * 1
*x(n+1,n) = f * x(n,n) + G * un
*P(n+1,n) = F * P(n,n) * FT + Q
*/
void kalPredict(Kal* kal,u input)
{
    float v = kal->x[1][0];
    //
    kal->x[0][0] += v * DELTA_T;
    kal->x[1][0] = v + (kal->k * input[0][0] - v) * DELTA_T;

    //covariance prediction

    float P00 = kal->P[0][0];
    float P01 = kal->P[0][1];
    float P10 = kal->P[1][0];
    float P11 = kal->P[1][1];

    float F11 = 1.0f - DELTA_T;

    kal->P[0][0] = P00 + DELTA_T*(P10 + P01) + DELTA_T*DELTA_T*P11 + kal->Q[0][0];
    kal->P[0][1] = P01 + DELTA_T*P11*F11 + kal->Q[0][1];
    kal->P[1][0] = P10 + DELTA_T*P11*F11 + kal->Q[1][0];
    kal->P[1][1] = F11*F11*P11 + kal->Q[1][1];
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
    kal->x[0][0] += kalGain[0][0] * x_Middle;
    kal->x[1][0] += kalGain[1][0] * x_Middle;

    kal->P[0][0] -= kalGain[0][0] * kal->P[0][0];
    kal->P[0][1] -= kalGain[0][0] * kal->P[0][1];
    kal->P[1][0] -= kalGain[1][0] * kal->P[0][0];
    kal->P[1][1] -= kalGain[1][0] * kal->P[0][1];
}

