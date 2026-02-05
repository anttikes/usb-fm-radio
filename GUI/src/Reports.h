#ifndef REPORTS_H
#define REPORTS_H

#include <QtQml/qqmlregistration.h>

enum class CommandIdentifiers_t : uint8_t
{
    CMD_ID_POWER_UP = 0x01,
    CMD_ID_GET_REV = 0x10,
    CMD_ID_POWER_DOWN = 0x11,
    CMD_ID_SET_PROPERTY = 0x12,
    CMD_ID_GET_PROPERTY = 0x13,
    CMD_ID_GET_INT_STATUS = 0x14,
    CMD_ID_PATCH_ARGS = 0x15,
    CMD_ID_PATCH_DATA = 0x16,
    CMD_ID_FM_TUNE_FREQ = 0x20,
    CMD_ID_FM_SEEK_START = 0x21,
    CMD_ID_FM_TUNE_STATUS = 0x22,
    CMD_ID_FM_RSQ_STATUS = 0x23,
    CMD_ID_FM_RDS_STATUS = 0x24,
    CMD_ID_FM_AGC_STATUS = 0x27,
    CMD_ID_FM_AGC_OVERRIDE = 0x28,
    CMD_ID_GPIO_CTL = 0x80,
    CMD_ID_GPIO_SET = 0x81,

    CMD_ID_QUEUE_WAS_RESET = 0xAA,     // TODO: This isn't a real command identifier
    CMD_ID_RADIO_STATUS_REPORT = 0xBB, // TODO: This isn't a real command identifier
};

struct RadioStateReport
{
    Q_GADGET

    Q_PROPERTY(uint8_t currentState MEMBER currentState)
    Q_PROPERTY(qreal currentFrequency MEMBER currentFrequency)

  public:
    uint8_t currentState;
    qreal currentFrequency;
};

struct RSQStatusReport
{
    Q_GADGET

    Q_PROPERTY(bool blendInterrupt MEMBER blendInterrupt)
    Q_PROPERTY(bool multHiInterrupt MEMBER multHiInterrupt)
    Q_PROPERTY(bool multLoInterrupt MEMBER multLoInterrupt)
    Q_PROPERTY(bool snrHiInterrupt MEMBER snrHiInterrupt)
    Q_PROPERTY(bool snrLoInterrupt MEMBER snrLoInterrupt)
    Q_PROPERTY(bool rssiHiInterrupt MEMBER rssiHiInterrupt)
    Q_PROPERTY(bool rssiLoInterrupt MEMBER rssiLoInterrupt)
    Q_PROPERTY(bool softMute MEMBER softMute)
    Q_PROPERTY(bool afcRail MEMBER afcRail)
    Q_PROPERTY(bool validChannel MEMBER validChannel)
    Q_PROPERTY(bool pilot MEMBER pilot)
    Q_PROPERTY(uint8_t stereoBlend MEMBER stereoBlend)
    Q_PROPERTY(uint8_t receivedSignalStrength MEMBER receivedSignalStrength)
    Q_PROPERTY(uint8_t signalToNoiseRatio MEMBER signalToNoiseRatio)
    Q_PROPERTY(uint8_t multipath MEMBER multipath)
    Q_PROPERTY(int8_t frequencyOffset MEMBER frequencyOffset)

  public:
    bool blendInterrupt;
    bool multHiInterrupt;
    bool multLoInterrupt;
    bool snrHiInterrupt;
    bool snrLoInterrupt;
    bool rssiHiInterrupt;
    bool rssiLoInterrupt;
    bool softMute;
    bool afcRail;
    bool validChannel;
    bool pilot;
    uint8_t stereoBlend;
    uint8_t receivedSignalStrength;
    uint8_t signalToNoiseRatio;
    uint8_t multipath;
    int8_t frequencyOffset;
};

#endif // REPORTS_H