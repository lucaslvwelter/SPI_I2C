/*
 * mpu6050_func.h
 *
 *  Created on: May 22, 2025
 *      Author: 22000206
 */

#ifndef INC_MPU6050_FUNC_H_
#define INC_MPU6050_FUNC_H_

#include "main.h"
#include "direcao.h"

typedef struct { // pega os movimentos do usuário
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
} leituraAcel;

typedef struct leituraGyro {
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
} leituraGyro;

void mpu6050Init(void);
void mpu6050ReadAccel(leituraAcel *leitura); // faz a leitura a partir dos movimentos do usuário
void mpu6050ReadGyro(leituraGyro *leitura);
Direcao detectarMovimento(float ax, float ay, float threshold); // faz a detecção do movimento

#endif /* MPU6050_H */
