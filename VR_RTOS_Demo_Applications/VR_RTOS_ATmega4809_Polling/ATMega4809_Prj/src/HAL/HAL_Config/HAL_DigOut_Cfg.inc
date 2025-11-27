/**********************************************************************\
*
* file: HAL_DigOut_Cfg.inc
*
* description : Demo programm for VR_RTOS
*               Configuration for digital outputs for Demoprogramm VR_RTOS
*
* Datum: 01.02.2025       Autor: Volker Rumm
*
\**********************************************************************/

//*************************
// definition for DigOut_Detail,
// redefine for every project
//*************************
static HAL_DigOut_t DigOut_Detail[] = 
{
	{STR_DIGOUT_LED, 'B', 5, true},
	{STR_DIGOUT_TP1, 'A', 2, false},
	{STR_DIGOUT_TP2, 'A', 3, false},
};

//---------------------------------------------------

