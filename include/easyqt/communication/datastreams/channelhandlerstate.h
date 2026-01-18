#pragma once

namespace ChannelHandlerState
{
    enum Enum
    {
        Idle,
        Connecting,
        Configuring,
        Connected,
        Failed
    };
}
