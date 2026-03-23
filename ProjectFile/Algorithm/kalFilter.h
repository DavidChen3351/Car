#pragma once
#include "motoConfig.h"

#define Nx 2
#define Nu 1
#define Nz 1

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
//Qa for considering acceleration as process noise
//Qv for considering speed as process noise
//k when speed stable,speed / moto input(-1,1) = k  
typedef float u[Nu][1];                
typedef float z[Nz][1];
typedef float R[Nz][Nz];
typedef float K[Nx][Nz];
typedef float iniX[Nx][1];
typedef float iniP[Nx][Nx];
typedef float iniQ[Nx][Nx];
typedef float Qa;
typedef float Qv;
               
typedef struct{                     
    float x[Nx][1];                       
    float F[Nx][Nx];                  
    float G[Nx][Nu];                  
    float P[Nx][Nx];                  
    float Q[Nx][Nx];                                   
    float w[Nx][1];                    
    float v[Nz][1];                    
    float H[Nz][Nx];  
    float k;                                 
}Kal;

void kalInit(Kal *kal,iniX x,iniP P,Qv var,float motoInputPerDamping);
void kalPredict(Kal* kal,u input);
void kalUpdate(Kal *kal,z measure,R cov);