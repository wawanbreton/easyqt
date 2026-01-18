#pragma once

namespace BluetoothClientState
{
    enum Enum
    {
        Idle,
        WaitingForDevice,
        Pairing,
        Connecting,
        Connected,
        PairingFailed,
        ConnectionFailed,
        NotAvailable,
    };
}
