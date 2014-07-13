#ifndef RTP_CHANNEL_H_INCLUDED
#define RTP_CHANNEL_H_INCLUDED

#include <jrtplib3/rtpsession.h>

class VideoSettings;

class RtpChannel
    : public RTPSession
{
public:
    explicit RtpChannel(VideoSettings const & settings);
    ~RtpChannel();

    void sendNals(uint8_t const * buf, size_t len);

    void delay(size_t fps) const
    {
        RTPTime::Wait(RTPTime(1.0 / double(fps)));
    }

    void setupPayloadType(uint8_t pt)
    {
        payloadType_ = pt;
    }

protected:
    void OnAPPPacket(RTCPAPPPacket *, RTPTime const & recvtime, RTPAddress const *);
    void OnBYEPacket(RTPSourceData *);
    void OnBYETimeout(RTPSourceData *);

private:
    size_t   maxPacketSize_;
    uint8_t  payloadType_;
    uint32_t timestampInc_;
};

#endif // RTP_CHANNEL_H_INCLUDED
