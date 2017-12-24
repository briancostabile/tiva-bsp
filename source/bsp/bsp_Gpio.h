/*============================================================================*/
/**
 * @file bsp_Gpio.h
 * @brief Contains types and prototypes to access the GPIO
 */
#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "bsp_Platform.h"

#include BUILD_INCLUDE_STRING( bsp_Gpio_, PLATFORM )

/*==============================================================================
 *                             Defines
 *============================================================================*/
/*============================================================================*/
/* Common Defines for all GPIO blocks */
#define BSP_GPIO_BASE_ADDR_OFFSET_DATA   0x00000000
#define BSP_GPIO_BASE_ADDR_OFFSET_DIR    0x00000400
#define BSP_GPIO_BASE_ADDR_OFFSET_IS     0x00000404
#define BSP_GPIO_BASE_ADDR_OFFSET_IBE    0x00000408
#define BSP_GPIO_BASE_ADDR_OFFSET_IEV    0x0000040C
#define BSP_GPIO_BASE_ADDR_OFFSET_IM     0x00000410
#define BSP_GPIO_BASE_ADDR_OFFSET_RIS    0x00000414
#define BSP_GPIO_BASE_ADDR_OFFSET_MIS    0x00000418
#define BSP_GPIO_BASE_ADDR_OFFSET_ICR    0x0000041C
#define BSP_GPIO_BASE_ADDR_OFFSET_AFSEL  0x00000420
#define BSP_GPIO_BASE_ADDR_OFFSET_DR2D   0x00000500
#define BSP_GPIO_BASE_ADDR_OFFSET_DR4D   0x00000504
#define BSP_GPIO_BASE_ADDR_OFFSET_DR8D   0x00000508
#define BSP_GPIO_BASE_ADDR_OFFSET_ODR    0x0000050C
#define BSP_GPIO_BASE_ADDR_OFFSET_PUR    0x00000510
#define BSP_GPIO_BASE_ADDR_OFFSET_PDR    0x00000514
#define BSP_GPIO_BASE_ADDR_OFFSET_SLR    0x00000518
#define BSP_GPIO_BASE_ADDR_OFFSET_DEN    0x0000051C
#define BSP_GPIO_BASE_ADDR_OFFSET_LOCK   0x00000520
#define BSP_GPIO_BASE_ADDR_OFFSET_CR     0x00000524
#define BSP_GPIO_BASE_ADDR_OFFSET_AMSEL  0x00000528
#define BSP_GPIO_BASE_ADDR_OFFSET_PCTL   0x0000052C
#define BSP_GPIO_BASE_ADDR_OFFSET_ADCCTL 0x00000530
#define BSP_GPIO_BASE_ADDR_OFFSET_DMACTL 0x00000534

#define BSP_GPIO_UNLOCK_CODE GPIO_LOCK_KEY

/*============================================================================*/
#define BSP_GPIO_DIR_INPUT  0x00
#define BSP_GPIO_DIR_OUTPUT 0x01
typedef uint8_t bsp_Gpio_Dir_t;

/*============================================================================*/
#define BSP_GPIO_INT_SENSE_EDGE  0x00
#define BSP_GPIO_INT_SENSE_LEVEL 0x01
typedef uint8_t bsp_Gpio_IntSense_t;

/*============================================================================*/
#define BSP_GPIO_INT_EDGE_SINGLE 0x00
#define BSP_GPIO_INT_EDGE_DUAL   0x01
typedef uint8_t bsp_Gpio_IntEdge_t;

/*============================================================================*/
#define BSP_GPIO_INT_EVENT_DIR_FALLING 0x00
#define BSP_GPIO_INT_EVENT_DIR_RISING  0x01
typedef uint8_t bsp_Gpio_IntEventDir_t;

/*============================================================================*/
#define BSP_GPIO_INT_TYPE_EDGE_RISING  0x00
#define BSP_GPIO_INT_TYPE_EDGE_FALLING 0x01
#define BSP_GPIO_INT_TYPE_EDGE_BOTH    0x02
#define BSP_GPIO_INT_TYPE_LEVEL_LOW    0x03
#define BSP_GPIO_INT_TYPE_LEVEL_HIGH   0x04
typedef uint8_t bsp_Gpio_IntType_t;

/*============================================================================*/
#define BSP_GPIO_INT_CONTROL_DISABLE 0x00
#define BSP_GPIO_INT_CONTROL_ENABLE  0x01
typedef uint8_t bsp_Gpio_IntControl_t;

/*============================================================================*/
#define BSP_GPIO_PULL_NONE 0x00
#define BSP_GPIO_PULL_UP   0x01
#define BSP_GPIO_PULL_DOWN 0x02
typedef uint8_t bsp_Gpio_Pull_t;

/*============================================================================*/
#define BSP_GPIO_DRIVE_2MA    0x00
#define BSP_GPIO_DRIVE_4MA    0x01
#define BSP_GPIO_DRIVE_8MA    0x02
#define BSP_GPIO_DRIVE_8MA_SC 0x03
typedef uint8_t bsp_Gpio_Drive_t;

/*==============================================================================
 *                             Macros
 *============================================================================*/
/*============================================================================*/
/**
 * Macro to turn a base register and register offset into a register pointer
 */
#define BSP_GPIO_REG( _baseAddr, _regName ) ADDR_TO_REG((_baseAddr) + BSP_GPIO_BASE_ADDR_OFFSET_##_regName)
#define BSP_GPIO_REG_FROM_NAME( _regName, _ioName ) *((volatile uint32_t *)(BSP_GPIO_BASE_ADDR_##_ioName + BSP_GPIO_BASE_ADDR_OFFSET_##_regName))


/**
 * To write to the pin the address of the data register to write to depends on
 * the bit offset
 */
#define BSP_GPIO_DATA_REG_FROM_NAME( _ioName ) *(((volatile uint32_t *)(BSP_GPIO_BASE_ADDR_##_ioName + BSP_GPIO_BASE_ADDR_OFFSET_DATA)) + BSP_GPIO_BIT_MASK_##_ioName)

/*============================================================================*/
/**
 * Macros to get the platform port and mask from the name
 */
#define BSP_GPIO_PORT_ID( _name ) (BSP_GPIO_PORT_ID_##_name)
#define BSP_GPIO_PORT_BASE_ADDR( _name ) (BSP_GPIO_BASE_ADDR_##_name)
#define BSP_GPIO_ALT_FUNC( _name ) (BSP_GPIO_ALT_FUNC_##_name)
#define BSP_GPIO_MASK( _name ) (bsp_Gpio_BitMask_t)(BSP_GPIO_BIT_MASK_##_name)


/*============================================================================*/
/**
 * Macros to set/clear/toggle a single IO based on the defined name
 */
#define BSP_GPIO_OUT_SET_HIGH( _name )                                 \
{                                                                      \
	BSP_GPIO_DATA_REG_FROM_NAME( _name ) |= BSP_GPIO_BIT_MASK_##_name; \
}

#define BSP_GPIO_OUT_SET_LOW( _name )                                   \
{                                                                       \
	BSP_GPIO_DATA_REG_FROM_NAME( _name ) &= ~BSP_GPIO_BIT_MASK_##_name; \
}

#define BSP_GPIO_TOGGLE( _name )                                       \
{                                                                      \
	BSP_GPIO_DATA_REG_FROM_NAME( _name ) ^= BSP_GPIO_BIT_MASK_##_name; \
}

/*============================================================================*/
/**
 * Macro to Read a single IO based on the defined name
 */
#define BSP_GPIO_GET( _name )                                           \
(                                                                       \
	BSP_GPIO_DATA_REG_FROM_NAME( _name ) >> BSP_GPIO_BIT_OFFSET_##_name \
)

/*============================================================================*/
/**
 * Macros to set the direction of single IO based on the defined name
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_OUTPUT( _name )                                     \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( DIR, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INTPUT( _name )                                      \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( DIR, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_DIR( _name, _dir ) \
{                                       \
	if( (_dir) == BSP_GPIO_DIR_INPUT )  \
	{                                   \
		BSP_GPIO_SET_INTPUT( _name );   \
	}                                   \
	else                                \
	{                                   \
		BSP_GPIO_SET_OUTPUT( _name );   \
	}                                   \
}

/*============================================================================*/
/**
 * Macros to set the interrupt type of single IO based on the defined name
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_INT_SENSE_EDGE( _name )                             \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( IS, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INT_SENSE_LEVEL( _name )                           \
{                                                                       \
	BSP_GPIO_REG_FROM_NAME( IS, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INT_SENSE( _name, _sense )  \
{                                                \
	if( (_sense) == BSP_GPIO_INT_SENSE_EDGE )    \
	{                                            \
		BSP_GPIO_SET_INT_SENSE_EDGE( _name );    \
	}                                            \
	else                                         \
	{                                            \
		BSP_GPIO_SET_INT_SENSE_LEVEL( _name );   \
	}                                            \
}

/*============================================================================*/
/**
 * Macros to set the interrupt edge trigger type (single or both) of single IO
 * based on the defined name
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_INT_EDGE_SINGLE( _name )                             \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( IBE, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INT_EDGE_DUAL( _name )                              \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( IBE, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INT_EDGE( _name, _edge )  \
{                                              \
	if( (_edge) == BSP_GPIO_INT_EDGE_SINGLE )  \
	{                                          \
		BSP_GPIO_SET_INT_EDGE_SINGLE( _name ); \
	}                                          \
	else                                       \
	{                                          \
		BSP_GPIO_SET_INT_EDGE_DUAL( _name );   \
	}                                          \
}

/*============================================================================*/
/**
 * Macros to set the interrupt event direction (falling or rising) of single IO
 * based on the defined name
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_INT_EVENT_DIR_FALLING( _name )                       \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( IEV, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INT_EVENT_DIR_RISING( _name )                       \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( IEV, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INT_EVENT_DIR( _name, _event )  \
{                                                    \
	if( (_event) == BSP_GPIO_INT_EVENT_DIR_FALLING ) \
	{                                                \
		BSP_GPIO_SET_INT_EVENT_FALLING( _name );     \
	}                                                \
	else                                             \
	{                                                \
		BSP_GPIO_SET_INT_EVENT_RISING( _name );      \
	}                                                \
}

/*============================================================================*/
/**
 * Macros to set the interrupt mask (disabled or enabled) of single IO
 * based on the defined name
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_INT_CONTROL_DISABLE( _name )                        \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( IM, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INT_CONTROL_ENABLE( _name )                        \
{                                                                       \
	BSP_GPIO_REG_FROM_NAME( IM, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_INT_CONTROL( _name, _control )  \
{                                                    \
	if( (_control) == BSP_GPIO_INT_CONTROL_DISABLE ) \
	{                                                \
		BSP_GPIO_SET_INT_CONTROL_DISABLE( _name );   \
	}                                                \
	else                                             \
	{                                                \
		BSP_GPIO_SET_INT_CONTROL_ENABLE( _name );    \
	}                                                \
}

/*============================================================================*/
/**
 * Macro to get the Raw (before masked) and masked interrupt status of a single
 * named GPIO.
 */
#define BSP_GPIO_GET_INT_RAW_STATUS( _name )                                \
(                                                                           \
	(BSP_GPIO_REG_FROM_NAME( RIS, _name ) & (BSP_GPIO_BIT_MASK_##_name)) >> \
	(BSP_GPIO_BIT_OFFSET_##_name)                                           \
)

#define BSP_GPIO_GET_INT_MASKED_STATUS( _name )                             \
(                                                                           \
	(BSP_GPIO_REG_FROM_NAME( MIS, _name ) & (BSP_GPIO_BIT_MASK_##_name)) >> \
	(BSP_GPIO_BIT_OFFSET_##_name)                                           \
)

/*============================================================================*/
/**
 * Macro to clear the interrupt status of a single named GPIO.
 */
#define BSP_GPIO_INT_CLEAR( _name )                                     \
{                                                                       \
	BSP_GPIO_REG_FROM_NAME( ICR, _name ) = (BSP_GPIO_BIT_MASK_##_name); \
}

/*============================================================================*/
/**
 * Macro to set the function (IO, or Alternative) of a single named GPIO.
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_FUNCTION_IO( _name )                                   \
{                                                                           \
	BSP_GPIO_REG_FROM_NAME( AFSEL, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_FUNCTION_ALT( _name )                                 \
{                                                                          \
	BSP_GPIO_REG_FROM_NAME( AFSEL, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

/*============================================================================*/
/**
 * Macro to set the drive strength of a named GPIO.
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_DRIVE_2MA( _name )                                   \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( DR2R, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_DRIVE_4MA( _name )                                   \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( DR4R, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_DRIVE_8MA_WITH_SLEW_CONTROL( _name )                 \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( DR8R, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
	BSP_GPIO_REG_FROM_NAME( SLR, _name )  |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_DRIVE_8MA_WITHOUT_SLEW_CONTROL( _name )               \
{                                                                          \
	BSP_GPIO_REG_FROM_NAME( DR8R, _name ) |= (BSP_GPIO_BIT_MASK_##_name);  \
	BSP_GPIO_REG_FROM_NAME( SLR, _name )  &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

/*============================================================================*/
/**
 * Macro to set whether an IO is open drain or not of a named GPIO.
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_OPEN_DRAIN( _name )                                 \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( ODR, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_CLEAR_OPEN_DRAIN( _name )                                \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( ODR, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

/*============================================================================*/
/**
 * Macros to set the pull (up or down) of single IO based on the defined name
 * Note: These are not thread safe.
 */
#define BSP_GPIO_SET_PULL_UP( _name )                                    \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( PUR, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_PULL_DOWN( _name )                                  \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( PDR, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_PULL_NONE( _name )                                   \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( PUR, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
	BSP_GPIO_REG_FROM_NAME( PDR, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_SET_PULL( _name, _pull )    \
{                                            \
	if( (_pull) == BSP_GPIO_PULL_UP )        \
	{                                        \
		BSP_GPIO_SET_PULL_UP( _name );       \
	}                                        \
	else if( (_pull) == BSP_GPIO_PULL_DOWN ) \
	{                                        \
		BSP_GPIO_SET_PULL_DOWN( _name );     \
	}                                        \
	else                                     \
	{                                        \
		BSP_GPIO_SET_PULL_NONE( _name );     \
	}                                        \
}

/*============================================================================*/
/**
 * Macros to set the pull (up or down) of single IO based on the defined name
 * Note: These are not thread safe.
 */
#define BSP_GPIO_DIGITAL_CONTORL_DISABLE( _name )                         \
{                                                                         \
	BSP_GPIO_REG_FROM_NAME( DEN, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_DIGITAL_CONTROL_ENABLE( _name )                         \
{                                                                        \
	BSP_GPIO_REG_FROM_NAME( DEN, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}


/*============================================================================*/
/**
 * Macros to lock/unlock the Commit Register which controls when changes to
 * GPIOAFSEL, GPIOPUR, GPIOPDR, and GPIODEN are committed
 */
#define BSP_GPIO_UNLOCK( _name )                                  \
{                                                                 \
	BSP_GPIO_REG_FROM_NAME( LOCK, _name ) = BSP_GPIO_UNLOCK_CODE; \
}

#define BSP_GPIO_LOCK( _name )                          \
{                                                       \
	BSP_GPIO_REG_FROM_NAME( LOCK, _name ) = 0x00000001; \
}


/*============================================================================*/
/**
 * Macro to set the commit bit based on the named GPIO
 */
#define BSP_GPIO_CONFIG_COMMIT( _name )                                 \
{                                                                       \
	BSP_GPIO_REG_FROM_NAME( CR, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}


/*============================================================================*/
/**
 * Macros to set the pull (up or down) of single IO based on the defined name
 * Note: These are not thread safe.
 */
#define BSP_GPIO_ANALOG_SELECT_DISABLE( _name )                             \
{                                                                           \
	BSP_GPIO_REG_FROM_NAME( AMSEL, _name ) &= ~(BSP_GPIO_BIT_MASK_##_name); \
}

#define BSP_GPIO_ANALOG_SELECT_ENABLE( _name )                             \
{                                                                          \
	BSP_GPIO_REG_FROM_NAME( AMSEL, _name ) |= (BSP_GPIO_BIT_MASK_##_name); \
}



/*============================================================================*/
/**
 * Macro to commit changes to locked registers
 * Note: This should be executed in critical section
 */
#define BSP_GPIO_COMMIT( _baseAddr, _mask )                 \
{                                                           \
    BSP_GPIO_REG((_baseAddr), LOCK) = BSP_GPIO_UNLOCK_CODE; \
    BSP_GPIO_REG((_baseAddr), CR)   |= (_mask);             \
    BSP_GPIO_REG((_baseAddr), LOCK) = 0;                    \
}

/*==============================================================================
 *                               Types
 *============================================================================*/
/*============================================================================*/
typedef uint8_t bsp_Gpio_BitMask_t;

/*============================================================================*/
typedef uint8_t bsp_Gpio_AltFuncId_t;

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_Gpio_init( void );

/*============================================================================*/
void
bsp_Gpio_write( bsp_Gpio_PortId_t  portId,
		        bsp_Gpio_BitMask_t mask,
		        bsp_Gpio_BitMask_t val );

/*============================================================================*/
bsp_Gpio_BitMask_t
bsp_Gpio_read( bsp_Gpio_PortId_t  portId,
		       bsp_Gpio_BitMask_t mask );

/*============================================================================*/
void
bsp_Gpio_configAltFunction( bsp_Gpio_PortId_t    portId,
                            bsp_Gpio_BitMask_t   mask,
                            bool_t               analog,
                            bsp_Gpio_AltFuncId_t altFuncId );

/*============================================================================*/
void
bsp_Gpio_configOutput( bsp_Gpio_PortId_t  portId,
                       bsp_Gpio_BitMask_t mask,
                       bool_t             openDrain,
                       bsp_Gpio_Drive_t   drive );

/*============================================================================*/
void
bsp_Gpio_configInput( bsp_Gpio_PortId_t  portId,
                      bsp_Gpio_BitMask_t mask,
                      bool_t             openDrain,
                      bsp_Gpio_Pull_t    pull );

/*============================================================================*/
void
bsp_Gpio_intControl( bsp_Gpio_PortId_t     portId,
                     bsp_Gpio_BitMask_t    mask,
                     bsp_Gpio_IntControl_t control );

/*============================================================================*/
void
bsp_Gpio_intConfig( bsp_Gpio_PortId_t       portId,
                    bsp_Gpio_BitMask_t      mask,
                    bool_t                  dmaTriggerEnable,
                    bool_t                  adcTriggerEnable,
                    bsp_Gpio_IntType_t      type,
                    bsp_Gpio_InputHandler_t callback );

#endif
