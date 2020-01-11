#include "ph.h"

float ReadPH(float voltage, PH_InitTypeDef PH)
{	
	// two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
	float slope = (7.0-4.0)/((PH._neutralVoltage-1500.0)/3.0 - (PH._acidVoltage-1500.0)/3.0);  
	float intercept =  7.0 - slope*(PH._neutralVoltage-1500.0)/3.0;
	
	PH._phValue = slope*(voltage - 1500.0)/3.0+intercept;  //y = k*x + b

	return PH._phValue;	
}

void InitPH(PH_InitTypeDef *PH_InitStructure)
{
	PH_InitStructure->_temperature = 25.0;
	PH_InitStructure->_phValue = 7.0;
	PH_InitStructure->_acidVoltage = 2032.44;
	PH_InitStructure->_neutralVoltage = 1500.0;
	PH_InitStructure->_voltage = 1500.0;	
}

void phCalibration(PH_InitTypeDef PH, float voltage)
{
	if((PH._voltage > 1322)&&(PH._voltage < 1678)){        // buffer solution:7.0
			PH._neutralVoltage =  PH._voltage;
	}else if((PH._voltage > 1854)&&(PH._voltage < 2210)){  // buffer solution:4.0
			PH._acidVoltage =  PH._voltage;
	}else{	// not buffer solution or faulty 
     __nop(); 
	}
}

