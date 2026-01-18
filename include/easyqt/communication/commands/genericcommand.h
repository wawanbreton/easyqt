#pragma once

namespace GenericCommand
{
    typedef enum
    {
        StartNavigation = 0x00010003,
        StopNavigation  = 0x00010006,
        SetIndexTraj    = 0x00020000,
        SetParamLog     = 0x00030000,
        StartStop       = 0x00040000,
    } Enum;
}
