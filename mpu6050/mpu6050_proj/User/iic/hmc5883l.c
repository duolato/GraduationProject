
#include "hmc5883l.h"

uint8_t Hmc5883l_ID_A;
uint8_t Hmc5883l_ID_B;
uint8_t Hmc5883l_ID_C;
float Hmc5883l_Gauss_X;
float Hmc5883l_Gauss_Y;
float Hmc5883l_Gauss_Z;
float Hmc5883l_Yaw;

uint8_t Hmc5883l_Init(void)
{
	uint8_t data;
	Delay_ms(10);//must delay
	if(Hmc5883l_GetDevID())
	{
		return 1;
	}
	if(HMC5883L_ID_A != Hmc5883l_ID_A || HMC5883L_ID_B != Hmc5883l_ID_B || HMC5883L_ID_C != Hmc5883l_ID_C)
	{
		return 2;
	}
	data = 0x78;//samples averaged=8;data output rate=75Hz
	if(I2C1_WriteData(HMC5883L_ADDRESS, CONFIG_REG_A, &data, 1))
	{
		return 3;
	}
	data = 0x00;//��0.88Ga
	if(I2C1_WriteData(HMC5883L_ADDRESS, CONFIG_REG_B, &data, 1))
	{
		return 4;
	}
	data = 0x00;//continuous-measurement mode
	if(I2C1_WriteData(HMC5883L_ADDRESS, MODE_REG, &data, 1))
	{
		return 5;
	}
	return 0;
}

uint8_t Hmc5883l_GetDevID(void)
{
	if(I2C1_ReadData(HMC5883L_ADDRESS, ID_REG_A, &Hmc5883l_ID_A, 1))
	{
		return 1;
	}
	if(I2C1_ReadData(HMC5883L_ADDRESS, ID_REG_B, &Hmc5883l_ID_B, 1))
	{
		return 2;
	}
	if(I2C1_ReadData(HMC5883L_ADDRESS, ID_REG_C, &Hmc5883l_ID_C, 1))
	{
		return 3;
	}
	//printf("0x%x 0x%x 0x%x \r\n", Hmc5883l_ID_A, Hmc5883l_ID_B, Hmc5883l_ID_C);
	return 0;
}

uint8_t Hmc5883l_GetData(void)
{
	uint8_t buff[6];
	/**
	uint8_t data;
	if(I2C1_ReadData(HMC5883L_ADDRESS, STATUS_REG, &data, 1))
	{
		return 1;
	}
	if(0 == (data & 0x01))//RDY=0 nothing to read
	{
		return 2;
	}
	*/
	if(I2C1_ReadData(HMC5883L_ADDRESS, DATA_OUTPUT_X_MSB, buff, 6))
	{
		return 3;
	}
	XData = (buff[0] & 0x00ff)<<8 | buff[1];
	ZData = (buff[2] & 0x00ff)<<8 | buff[3];
	YData = (buff[4] & 0x00ff)<<8 | buff[5];
	Hmc5883l_Gauss_X = XData / FACTOR_FIELD_RANGE_0_88;
	Hmc5883l_Gauss_Y = YData / FACTOR_FIELD_RANGE_0_88;
	Hmc5883l_Gauss_Z = ZData / FACTOR_FIELD_RANGE_0_88;
	//printf("%-5d %-5d %-5d\r\n", XData, YData, ZData);
	//printf("%f %f %f \r\n", Hmc5883l_Gauss_X, Hmc5883l_Gauss_Y, Hmc5883l_Gauss_Z);
	return 0;
}

void Hmc5883l_CalYaw(float pitch, float roll)
{
	float x, y, z;
	x = XData;
	y = YData;
	z = ZData;
	/**
	x = x*cos(pitch) + y*sin(roll)*sin(pitch) - z*cos(roll)*sin(pitch);
	y = y*cos(roll) + z*sin(roll);	
	*/
	//2017-01-28 when pitch and roll don't equal zero yaw is not true
	Hmc5883l_Yaw = atan2(y, x)*180/MATH_PI;//-PI~PI
	//printf("yaw: %f\r\n", Hmc5883l_Yaw);
}
