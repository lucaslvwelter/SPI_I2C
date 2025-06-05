#include "mpu6050_func.h"
#include "i2c.h"
#include "usart.h"
#include "direcao.h"

static uint8_t mpu6050Addr = (0x68 << 1);

void mpu6050Init(void) {
    static const uint8_t whoAmIReg = 0x75;
    static const uint8_t pwrMgmt1Reg = 0x6B;
    static const uint8_t configReg = 0x1A;
    static const uint8_t accelConfigReg = 0x1C;
    static const uint8_t gyroConfigReg = 0x1B;
    static unsigned char msgErro[] = "Erro na inicializacao do MPU6050";
    uint8_t check;
    uint8_t data;

    HAL_I2C_Mem_Read(&hi2c1, mpu6050Addr, whoAmIReg, 1, &check, 1, 1000);
    if(check == 0x68) {
        data = 0x08;
        HAL_I2C_Mem_Write(&hi2c1, mpu6050Addr, pwrMgmt1Reg, 1, &data, 1, 1000);
        data = 0x06;
        HAL_I2C_Mem_Write(&hi2c1, mpu6050Addr, configReg, 1, &data, 1, 1000);
        data = 0x00;
        HAL_I2C_Mem_Write(&hi2c1, mpu6050Addr, accelConfigReg, 1, &data, 1, 1000);
        data = 0x00;
        HAL_I2C_Mem_Write(&hi2c1, mpu6050Addr, gyroConfigReg, 1, &data, 1, 1000);
    } else {
        HAL_UART_Transmit(&huart2, msgErro, sizeof(msgErro), 100);
    }
}

void mpu6050ReadAccel(leituraAcel *leitura) { // faz um cálculo para poder pegar a direção do movimento
    static const uint8_t accelXoutHReg = 0x3B;
    uint8_t recData[6];
    HAL_I2C_Mem_Read(&hi2c1, mpu6050Addr, accelXoutHReg, 1, recData, 6, 1000);
    leitura->accelX = (int16_t)(recData[0] << 8 | recData[1]);
    leitura->accelY = (int16_t)(recData[2] << 8 | recData[3]);
    leitura->accelZ = (int16_t)(recData[4] << 8 | recData[5]);
}

void mpu6050ReadGyro(leituraGyro *leitura) {
    static const uint8_t gyroXoutHReg = 0x43;
    uint8_t recData[6];
    HAL_I2C_Mem_Read(&hi2c1, mpu6050Addr, gyroXoutHReg, 1, recData, 6, 1000);
    leitura->gyroX = (int16_t) (recData[0] << 8 | recData [1]);
    leitura->gyroY = (int16_t) (recData[2] << 8 | recData [3]);
    leitura->gyroZ = (int16_t) (recData[4] << 8 | recData [5]);
}

Direcao detectarMovimento(float ax, float ay, float threshold) // verifica qual foi o movimento e se foi válido
{
	if (ay < -threshold) return DIR_ESQUERDA;
	if (ay > threshold) return DIR_DIREITA;
	if (ax > threshold) return DIR_BAIXO;
	if (ax < -threshold) return DIR_CIMA;
	return DIR_NEUTRO;
}
