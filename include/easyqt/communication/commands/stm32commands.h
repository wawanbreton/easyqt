#pragma once

#include "platform/utils/qt/enum.h"

class Stm32Commands : public QObject
{
    Q_OBJECT
    Q_ENUMS(Enum)

    public:
        typedef enum
        {
            None                = 0,

            // Test commands
            TestButtonStart     = 0x010100,
            TestButtonSpeed     = 0x010200,
            TestContactRight    = 0x010300,
            TestContactLeft     = 0x010400,
            TestMotorFrontLeft  = 0x010500,
            TestMotorBackLeft   = 0x010600,
            TestMotorBackRight  = 0x010700,
            TestMotorFrontRight = 0x010800,
            TestLight           = 0x010900,
            TestHmiLight        = 0x010C00, // Obsolete pre-Tortax test
            TestBattery         = 0x010D00,
            TestAudio           = 0x010E00,
            TestBackToolSensors = 0x011000,
            TestDecawave        = 0x011300,
            TestTemperatureAuto = 0x011400,
            TestCompass         = 0x011500,
            TestTemperatureRead = 0x011600,
            TestMotorBackTool   = 0x011700,
            TestContact         = 0x011900,
            TestAdcRead         = 0x011100,
            TestLaser           = 0x011200,
            TestMotor           = 0x011A00,
            TestLexan           = 0x011B00,
            TestLightSensor     = 0x011D00,
            WriteSerialNumber   = 0x014E53,

            // Diag commands
            WriteEeprom         = 0x020100,
            ReadEeprom          = 0x020200,
            ReadRam             = 0x020400,
            WriteRam            = 0x020500,
            ReadDate            = 0x021200,
            WriteDate           = 0x021300,
            ReadTime            = 0x021400,
            WriteTime           = 0x021500,
            UpdateRam           = 0x021B00,
            ReadBatteryLevel    = 0x021D00,
            WriteRamRaw         = 0x021E00,
            ReadRamRaw          = 0x021F00,
            ReadOptionByte      = 0x022000,
            WriteOptionByte     = 0x022100,
            SetSpeeds           = 0x022300,
            Generic             = 0x022400,
            ControllerConnected = 0x022500,
            GetMotorsSpeeds     = 0x022600,
            ReadBleMacAddress   = 0x022900,

            // Sound commands
            RequestSoundDump   = 0x030100,
            DumpSoundPart      = 0x030200,
            EraseSoundFlash    = 0x030300,
            WriteSoundFlash    = 0x030400,
            CheckSoundFlash    = 0x030500,
            RequestFlashSound  = 0x030600,

            // Geoloc commands
            EraseTrackFlash  = 0x040300,
            WriteTrackFlash  = 0x040400,
            CheckTrackFlash  = 0x040500,
            SendPosition     = 0x04aabb,
            SendDistances    = 0x04ccdd,
            SendOdometry     = 0x04eeff,
            SendOrientation  = 0x04aacc,
            SendAcceleration = 0x04aaee,

            // Boot commands
            EnterBootLoader   = 0x050000,
            EraseBootFlash    = 0x050300,
            WriteBootFlash    = 0x050400,
            ChecksumBootFlash = 0x050500,
            WriteBootFlag     = 0x050600,
            ReadBootFlag      = 0x050700,
            FinalizeFlash     = 0x050800,

            // Stimuli commands
            RequestStimuliDump = 0x060100,
            DumpStimuliPart    = 0x060200,
            EraseStimuliFlash  = 0x060300,
            WriteStimuliFlash  = 0x060400,
            CheckStimuliFlash  = 0x060500,
            SetLight           = 0x061000,
            SetSound           = 0x061100,

            // Power management
            RequestSleep = 0x070100,

            // Charging station
            SetPosition     = 0x080100,
            EnteringStation = 0x080200,
            ChargingStarted = 0x080300,
            ChargingDone    = 0x080400,
            StationExit     = 0x080500,

            // GuettaBot
            SetCurrentPosition = 0x090100,
            OdometryData       = 0x090200,
            BumperEvent        = 0x090600,

        } Enum;

    PRINT_ENUM(Stm32Commands)
};
