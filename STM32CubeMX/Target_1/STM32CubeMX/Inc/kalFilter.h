#pragma once
#include "motoConfig.h"

#define Nx 2
#define Nu 1
#define Nz 1
#define DELTA_T MOTO_CONTROL_PERIOD_S
//x state vector
//z measurement
//F state transition matrix
//u input
//G control matrix
//P estimate covariance
//Q process noise covariance
//R measurement noise covariance
//w process noise
//v measurement noise
//H Observation matrix
//K Kalman gain 
typedef float u[Nu][1];                
typedef float z[Nz][1];
typedef float R[Nz][Nz];
typedef float K[Nx][Nz];
typedef float iniX[Nx][1];
typedef float iniQ[Nx][Nx];               
typedef struct{                     
    float x[Nx][1];                       
    float F[Nx][Nx];                  
    float G[Nx][Nu];                  
    float P[Nx][Nx];                  
    float Q[Nx][Nx];                                   
    float w[Nx][1];                    
    float v[Nz][1];                    
    float H[Nz][Nx];                                   
}Kal;

void kalInit(Kal *kal,iniX x,iniQ Q);
void kalPredict(Kal* kal,u input);
void kalUpdate(Kal *kal,z measure,R cov);