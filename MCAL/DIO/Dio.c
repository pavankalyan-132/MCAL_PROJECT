
#include "Port.h"
#include "Dio.h"
#include "Port_Reg.h"

Dio_LevelType Dio_ReadChannel( Dio_ChannelType ChannelId )
{
    uint8 port;
    uint8 pin;
    uint32 base;
    uint32 value;

    port = (ChannelId >> 8U) & 0xFF;
    pin  = (ChannelId) & 0xFF;

    if(pin >= 16U)
        return STD_LOW;

    base = Port_GetBaseAddress(port);

    if(base == NULL_PTR)
        return STD_LOW;

    value = PORT_REG32(base,PORT_IN_OFFSET);

    if((value >> pin) & 1)
        return STD_HIGH;
    else
        return STD_LOW;
}

void Dio_WriteChannel( Dio_ChannelType ChannelId, Dio_LevelType Level )
{
   uint8 port;
   uint8 pin;
   uint32 base;

   port = (ChannelId >> 8U) & 0xFF;
   pin  = (ChannelId) & 0xFF;

   if(pin >= 16U)
   {
       return ;
   }

   base = Port_GetBaseAddress(port);

   if(base == NULL_PTR)
   {
       return ;
   }

   if(Level == STD_HIGH)
   {
       PORT_REG32(base,PORT_OMR_OFFSET) = (1U << pin);


   }
   else if(Level == STD_LOW)
   {
       PORT_REG32(base,PORT_OMR_OFFSET) = (1U << (pin + 16U));
   }

   else
   {
       return;

   }


}

Dio_PortLevelType Dio_ReadPort( Dio_PortType PortId )
{

        uint32 base;
        uint32 value;



        base = Port_GetBaseAddress(PortId);

        if(base == NULL_PTR)
            return STD_LOW;

        value = PORT_REG32(base,PORT_IN_OFFSET);

        return (Dio_PortLevelType)value;

}


void Dio_WritePort( Dio_PortType PortId, Dio_PortLevelType Level )
{
    uint32 base;

    /* Get port base address */
    base = Port_GetBaseAddress(PortId);

    /* Check for valid port */
    if (base == 0U)
    {
        return;
    }

    /* Write the level to the output register */
    PORT_REG32(base, PORT_OUT_OFFSET) = (uint32)Level;
}

Dio_PortLevelType Dio_ReadChannelGroup( const Dio_ChannelGroupType* ChannelGroupIdPtr )
{
    uint32 base;
    uint32 value;

    /* Check for NULL pointer */
    if (ChannelGroupIdPtr == NULL_PTR)
    {
        return (Dio_PortLevelType)0U;
    }

    /* Validate mask and offset */
    if ((ChannelGroupIdPtr->mask == 0U) ||
        (ChannelGroupIdPtr->offset >= 16U))
    {
        return (Dio_PortLevelType)0U;
    }

    /* Get port base address */
    base = Port_GetBaseAddress(ChannelGroupIdPtr->port);

    /* Check for valid port */
    if (base == 0U)
    {
        return (Dio_PortLevelType)0U;
    }

    /* Read input register */
    value = PORT_REG32(base, PORT_IN_OFFSET);

    /* Apply mask and shift to right */
    value = (value & ChannelGroupIdPtr->mask)
            >> ChannelGroupIdPtr->offset;

    return (Dio_PortLevelType)value;
}

void Dio_WriteChannelGroup( const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level )
{
    uint32 base;
    uint32 shiftedLevel;
    uint32 setMask;
    uint32 clearMask;

    /* Check for NULL pointer */
    if (ChannelGroupIdPtr == NULL_PTR)
    {
        return;
    }

    /* Validate mask and offset */
    if ((ChannelGroupIdPtr->mask == 0U) ||
        (ChannelGroupIdPtr->offset >= 16U))
    {
        return;
    }

    /* Get port base address */
    base = Port_GetBaseAddress(ChannelGroupIdPtr->port);

    /* Check for valid port */
    if (base == 0U)
    {
        return;
    }

    /* Shift level according to offset and apply mask */
    shiftedLevel = (((uint32)Level << ChannelGroupIdPtr->offset)
                    & (uint32)ChannelGroupIdPtr->mask);

    /* Bits to be set HIGH */
    setMask = shiftedLevel;

    /* Bits to be cleared LOW */
    clearMask = (~shiftedLevel)
                & (uint32)ChannelGroupIdPtr->mask;

    /* Write set and clear masks to OMR */
    PORT_REG32(base, PORT_OMR_OFFSET) = setMask | (clearMask << 16U);
}
