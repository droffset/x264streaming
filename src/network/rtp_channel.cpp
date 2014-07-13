#include <network/rtp_channel.h>
//------------------------------------------------------------------------------
#include <video/video_settings.h>
//------------------------------------------------------------------------------
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtcpapppacket.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
//------------------------------------------------------------------------------
#include <except/except_rtp_channel.h>
//------------------------------------------------------------------------------
RtpChannel::RtpChannel(VideoSettings const & settings)
    : RTPSession(), payloadType_(0)
{
    VideoSettings::Rtp const & rtp = settings.rtp();

    maxPacketSize_ = rtp.maxPacketSize();
    timestampInc_  = rtp.timestampInc();

    RTPUDPv4TransmissionParams transmission;
    // Port base
    transmission.SetPortbase(rtp.portBase());
    transmission.SetRTPSendBuffer(rtp.sendBufferSize());

    RTPSessionParams session;
    // Sets the timestamp unit for our own data.
    session.SetOwnTimestampUnit(rtp.timestampUnit());
    // Maximum packet size
    session.SetMaximumPacketSize(maxPacketSize_ + 12);
    // Session bandwidth in bytes per second (default is 10000 bytes per second).
    session.SetSessionBandwidth(rtp.bandwidth());

    /// Session create
    int status = Create(session, &transmission);
    if(status < 0)
    {
        throw ExceptRtpChannel(RTPGetErrorString(status));
    }
    /// Address
    RTPIPv4Address addr(ntohl(inet_addr(rtp.ip())), rtp.port());
    status = AddDestination(addr);
    if(status < 0)
    {
        throw ExceptRtpChannel(RTPGetErrorString(status));
    }
}

RtpChannel::~RtpChannel()
{
    BYEDestroy(RTPTime(3,0), 0,0);
}
//------------------------------------------------------------------------------
void RtpChannel::sendNals(uint8_t const * pSendbuf, size_t len)
{
    if(len <= maxPacketSize_)
    {
        int const status = SendPacket(pSendbuf, len, payloadType_, true, timestampInc_);
        if(status < 0)
        {
            throw ExceptRtpChannel(RTPGetErrorString(status));
        }
    }
    else
    {
        int const last = len % maxPacketSize_;
        int const n    = len / maxPacketSize_ + (last > 0);

        int i = 0;
        for(int count = n - 1; i < count; ++i)
        {
            int const status = SendPacket(&pSendbuf[i * maxPacketSize_], maxPacketSize_, payloadType_, false, 0);
            if(status < 0)
            {
                throw ExceptRtpChannel(RTPGetErrorString(status));
            }
        }
        int const status = SendPacket(&pSendbuf[i * maxPacketSize_], last, payloadType_, true, timestampInc_);
        if(status < 0)
        {
            throw ExceptRtpChannel(RTPGetErrorString(status));
        }
    }
}
//------------------------------------------------------------------------------
void RtpChannel::OnAPPPacket(RTCPAPPPacket *, RTPTime const &, RTPAddress const *)
{ }
void RtpChannel::OnBYEPacket(RTPSourceData *)
{ }
void RtpChannel::OnBYETimeout(RTPSourceData *)
{ }
//------------------------------------------------------------------------------
