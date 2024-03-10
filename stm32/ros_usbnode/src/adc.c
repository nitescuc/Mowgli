/****************************************************************************
* Title                 :   adc module
* Filename              :   adc.c
* Author                :   Nekraus
* Origin Date           :   01/04/2023
* Version               :   1.0.0

*****************************************************************************/
/** \file adc.c
 *  \brief
 *
 */
/******************************************************************************
 * Includes
 *******************************************************************************/
#include "main.h"
#include "perimeter.h"
#include "adc.h"
#include <math.h>
/******************************************************************************
 * Module Preprocessor Constants
 *******************************************************************************/
const float f_RTO = 10000;
const float beta = 3380;

/******************************************************************************
 * Module Preprocessor Macros
 *******************************************************************************/

/******************************************************************************
 * Module Typedefs
 *******************************************************************************/
typedef enum
{
    ADC2_CHANNEL_CURRENT = 0,
    ADC2_CHANNEL_CHARGEVOLTAGE,
    ADC2_CHANNEL_BATTERYVOLTAGE,
    ADC2_CHANNEL_CHARGERINPUTVOLTAGE,
    ADC2_CHANNEL_NTC,
    ADC2_CHANNEL_MAX,
} ADC2_channelSelection_e;

/******************************************************************************
 * Module Variable Definitions
 *******************************************************************************/
TIM_HandleTypeDef TIM2_Handle; // Time Base for ADC
ADC_HandleTypeDef ADC2_Handle;
RTC_HandleTypeDef hrtc = {0};

ADC2_channelSelection_e adc2_eChannelSelection = ADC2_CHANNEL_CURRENT;

volatile uint16_t adc_u16BatteryVoltage       = 0;
volatile uint16_t adc_u16Current              = 0;
volatile uint16_t adc_u16ChargerVoltage       = 0;
volatile uint16_t adc_u16ChargerInputVoltage  = 0;
volatile uint16_t adc_u16Input_NTC            = 0;

float battery_voltage;
float charge_voltage;
float current;
float current_without_offset;
float ntc_voltage;
float blade_temperature;
float chargerInputVoltage;

union FtoU ampere_acc;
union FtoU charge_current_offset;

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
void adc2_SetChannel(ADC2_channelSelection_e channel);

/******************************************************************************
 *  Public Functions
 *******************************************************************************/

/**
 * @brief TIM2 Initialization Function
 *
 * Used to start ADC every 250µs
 *
 * @param None
 * @retval None
 */
void TIM2_Init(void)
{

    /* USER CODE BEGIN TIM2_Init 0 */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    TIM2_Handle.Instance = TIM2;
    TIM2_Handle.Init.Prescaler = 18 - 1; // 72Mhz -> 4Mhz
    TIM2_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM2_Handle.Init.Period = 1000 - 1; /*1khz*/
    TIM2_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TIM2_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_OC_Init(&TIM2_Handle) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&TIM2_Handle, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&TIM2_Handle, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
    sConfigOC.Pulse = 5;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_OC_ConfigChannel(&TIM2_Handle, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */
}

/**
 * @brief ADC Initialization Function
 *
 * Init GPIO to read
 *
 * @param None
 * @retval None
 */
void ADC2_Init(void)
{
    __HAL_RCC_ADC2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /**ADC1 GPIO Configuration
    PA1     ------> Charge Current
    PA2     ------> Charge Voltage
    PA3     ------> Battery Voltage
    PA7     ------> Charger Voltage
    PC2     ------>  Blade NTC
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */

    /** Common config
     */
    ADC2_Handle.Instance = ADC2;
    ADC2_Handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
    ADC2_Handle.Init.ContinuousConvMode = DISABLE;
    ADC2_Handle.Init.DiscontinuousConvMode = DISABLE;
    ADC2_Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_CC2;
    ADC2_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    ADC2_Handle.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&ADC2_Handle) != HAL_OK)
    {
        Error_Handler();
    }

    adc2_eChannelSelection = ADC2_CHANNEL_CURRENT;
    adc2_SetChannel(adc2_eChannelSelection);

    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

    // calibrate  - important for accuracy !
    HAL_ADCEx_Calibration_Start(&ADC2_Handle);
    HAL_ADC_Start_IT(&ADC2_Handle);
    HAL_TIM_OC_Start(&TIM2_Handle, TIM_CHANNEL_2);

    /* USER CODE BEGIN RTC_MspInit 0 */
    __HAL_RCC_PWR_CLK_ENABLE();
    /* USER CODE END RTC_MspInit 0 */
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
    /* USER CODE BEGIN RTC_MspInit 1 */
    HAL_PWR_EnableBkUpAccess();

    ampere_acc.u[0] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
    ampere_acc.u[1] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);

    charge_current_offset.u[0] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
    charge_current_offset.u[1] = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
}

/**
 * @brief ADC Input Function
 *
 * get the raw data and transform to human readeable values (V,A,T)
 *
 * @param None
 * @retval None
 */

void ADC_input(void)
{
    float l_fTmp;

    /* battery volatge calculation */
    l_fTmp = ((float)adc_u16BatteryVoltage / 4095.0f) * 3.3f * 10.09 + 0.6f;
    battery_voltage = 0.2 * l_fTmp + 0.8 * battery_voltage;

     /*charger voltage calculation */
    l_fTmp = ((float)adc_u16ChargerVoltage / 4095.0f) * 3.3f * 16;
    charge_voltage = 0.8 * l_fTmp + 0.2 * charge_voltage;

    /*charge current calculation */
    l_fTmp = (((float)adc_u16Current / 4095.0f) * 3.3f - 2.5f) * 100 / 12.0;
    current_without_offset =   0.8 * l_fTmp + 0.2 * current_without_offset;          

    /*remove offset*/
    current = current_without_offset - charge_current_offset.f;

    /*blade motor temperature calculation */
    l_fTmp = (adc_u16Input_NTC/4095.0f)*3.3f;
    ntc_voltage = 0.5*l_fTmp + 0.5*ntc_voltage;

    /*calculation for NTC temperature*/
    l_fTmp = ntc_voltage * 10000;               //Resistance of RT
    l_fTmp = log(l_fTmp / f_RTO);
    l_fTmp = (1 / ((l_fTmp / beta) + (1 / (273.15+25)))); //Temperature from thermistor
    blade_temperature = l_fTmp - 273.15;                 //Conversion to Celsius  

    /* Input voltage from the external supply*/
    l_fTmp = (adc_u16ChargerInputVoltage / 4095.0f) * 3.3f * (32 / 2);
    chargerInputVoltage = 0.5 * l_fTmp + 0.5 * chargerInputVoltage;

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
#ifdef OPTION_PERIMETER
    if (hadc == &ADC_Handle)
    {
        PERIMETER_vITHandle();
    }
#endif

    if (hadc == &ADC2_Handle)
    {
        uint16_t l_u16Rawdata = ADC2_Handle.Instance->DR;

        switch (adc2_eChannelSelection)
        {
        case ADC2_CHANNEL_CURRENT:
            adc_u16Current = l_u16Rawdata;
            break;

        case ADC2_CHANNEL_CHARGEVOLTAGE:
            adc_u16ChargerVoltage = l_u16Rawdata;
            break;

        case ADC2_CHANNEL_BATTERYVOLTAGE:
            adc_u16BatteryVoltage = l_u16Rawdata;
            break;

        case ADC2_CHANNEL_CHARGERINPUTVOLTAGE:
            adc_u16ChargerInputVoltage = l_u16Rawdata;
            break;

        case ADC2_CHANNEL_NTC:
            adc_u16Input_NTC = l_u16Rawdata;

            break;

        case ADC2_CHANNEL_MAX:
        default:
            /* should not get here */
            break;
        }

        adc2_eChannelSelection++;
        if (adc2_eChannelSelection == ADC2_CHANNEL_MAX)
            adc2_eChannelSelection = ADC2_CHANNEL_CURRENT;
        adc2_SetChannel(adc2_eChannelSelection);

        HAL_ADC_Start_IT(&ADC2_Handle);
    }
}



/******************************************************************************
 *  Private Functions
 *******************************************************************************/

void adc2_SetChannel(ADC2_channelSelection_e channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    switch (channel)
    {
    case ADC2_CHANNEL_CURRENT:
        sConfig.Channel = ADC_CHANNEL_1; // PA1 Charge Current
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
        if (HAL_ADC_ConfigChannel(&ADC2_Handle, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        break;

    case ADC2_CHANNEL_CHARGEVOLTAGE:
        sConfig.Channel = ADC_CHANNEL_2; // PA2 Charge Voltage
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
        if (HAL_ADC_ConfigChannel(&ADC2_Handle, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        break;

    case ADC2_CHANNEL_BATTERYVOLTAGE:
        sConfig.Channel = ADC_CHANNEL_3; // PA3 Battery
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
        if (HAL_ADC_ConfigChannel(&ADC2_Handle, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        break;

    case ADC2_CHANNEL_CHARGERINPUTVOLTAGE:
        sConfig.Channel = ADC_CHANNEL_7; // PA7 Charger Input voltage
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
        if (HAL_ADC_ConfigChannel(&ADC2_Handle, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        break;

    case ADC2_CHANNEL_NTC:
        sConfig.Channel = ADC_CHANNEL_13; // PC2
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
        if (HAL_ADC_ConfigChannel(&ADC2_Handle, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        break;

    case ADC2_CHANNEL_MAX:
    default:
        /* should not get here */
        sConfig.Channel = ADC_CHANNEL_3; // PA3 Battery
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
        if (HAL_ADC_ConfigChannel(&ADC2_Handle, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }
        break;
    }
}
