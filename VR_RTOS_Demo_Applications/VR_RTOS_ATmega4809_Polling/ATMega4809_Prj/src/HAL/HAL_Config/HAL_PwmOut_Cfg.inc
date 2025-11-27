/**********************************************************************\
*
* file: HAL_PwmOut_Cfg.inc
*
* description : Demo programm for VR_RTOS
*               Configuration for pwm outputs for Demoprogramm VR_RTOS
*
* Datum: 01.02.2025       Autor: Volker Rumm
*
\**********************************************************************/


//*************************
// definition for PwmOut_Detail,
// redefine for every project
//*************************
static HAL_PwmOut_t PwmOut_Detail[] = 
{
	{STR_PWMOUT_LED, 'B', 4, true,  0},
	//{STR_PWMOUT_TP1, 'D', 0, false, 0},
	//{STR_PWMOUT_TP2, 'D', 1, false, 0},
};

//---------------------------------------------------
