#include <video/video_x264_encoder.h>
#include <video/video_device.h>

#include <except/except_video_device.h>
#include <except/except_video_buffer.h>
#include <except/except_video_hang_up.h>

#include <network/rtp_channel.h>

#include <stdexcept>
#include <cstdio>
//------------------------------------------------------------------------------
VideoX264Encoder::VideoX264Encoder(RtpChannel & dest) throw()
    : channel_(dest), fps_(0), enc_(0)
    , reason_(0), arg_(0)
{
    channel_.setupPayloadType(PAYLOAD_TYPE);
}
//------------------------------------------------------------------------------
VideoX264Encoder::~VideoX264Encoder() throw()
{
    if(enc_)
    {
        x264_encoder_close(enc_);
    }
}
//------------------------------------------------------------------------------
void VideoX264Encoder::init(size_t width, size_t height, size_t fps)
{
    if(!enc_)
    {
        fps_ = fps;

        printf("x264 encoder will init %dx%d with %d fps.\n", width, height, fps);

        x264_param_t param;

        x264_param_default_preset(&param, "fast", "zerolatency");
    //set i_keyint_max=5 and b_intra_refresh=0 for perfomance

    // * CpuFlags
        param.i_threads = X264_SYNC_LOOKAHEAD_AUTO ;// * Take an empty buffer deadlock continued use does not guarantee .
    // * Video Options
        param.i_width  = width;  // * width of the image to be encoded .
        param.i_height = height; // * to be encoded image height
        param.i_frame_total = 0; // * do not know the total number of frames coded with 0 .
        param.i_keyint_max  = fps;

    // * Rate control:
//        param.rc.i_rc_method   = X264_RC_CRF;
//        param.rc.f_rf_constant = 25;
//        param.rc.f_rf_constant_max = 35;

    // intra refresh - maybe this should be an option?
    // should help with packet loss resiliency at low bitrates
        param.b_intra_refresh = 0;

    // * Flow parameters
    // just to be safe, make sure there are no b-frames (just in case it may be
    // set by a slower speed preset)
        param.i_bframe = 0;//5;
   /*
        Open-GOP is an encoding technique which increases efficiency.
        Some decoders don't fully support open-GOP streams, which is why it hasn't
        been enabled by default. You should test with all decoders your streams will
        be played on, or (if that's impossible) wait until support is generally available.
    */
        //param.b_open_gop = 0;

        //param.i_bframe_pyramid  = 0;
        param.i_bframe_adaptive = X264_B_ADAPT_TRELLIS; //FIXME

        // * Log parameters directly commented on the line when you do not need to print the coded information
        //param.i_log_level  = X264_LOG_DEBUG;
        // * Rate control parameter

        param.rc.i_bitrate = 1024 * fps;// * bit rate ( bit rate , unit Kbps)

        //RTP max packet size
        //param.i_slice_max_size = 1360;

        //param.b_repeat_headers = 0;

        // if annexb is 1, x264 uses startcodes (0 -> plain size)
        // starting Feb 2010, x264 uses short startcodes in some cases which throws
        // off the packetizer since it assumes the NAL payload is at a constant byte
        // offset from the header
        //param->b_annexb = 0;

    // * Muxing parameters
        param.i_fps_den = 1;   // * frame rate denominator
        param.i_fps_num = fps; // * frame rate of molecular
        param.i_timebase_den = param.i_fps_num;
        param.i_timebase_num = param.i_fps_den;

       // * Set Profile. Use Baseline profile
        x264_param_apply_profile(&param, x264_profile_names[0]);

        enc_ = x264_encoder_open(&param);

        if(!enc_)
        {
            throw std::runtime_error("error open encoder");
        }

        fprintf(stdout, "x264 encoder init success.\n");
    }
    else
    {
        fprintf(stdout, "x264 encoder already init.\n");
    }
}
//------------------------------------------------------------------------------
void VideoX264Encoder::x264PicInit(x264_picture_t & pic, VideoDevice * dev, uint8_t * data)
{
    memset(&pic, 0, sizeof(x264_picture_t));

    size_t const width    = dev->width();
    size_t const nbytesY  = width * dev->height();
    size_t const nbytesUv = nbytesY / 4;

    pic.i_type    = X264_TYPE_AUTO;
    pic.i_qpplus1 = 0;
    pic.img.i_csp = X264_CSP_I420;
    pic.img.i_plane = 3;

    pic.img.plane[0] = data;
    pic.img.plane[1] = data + nbytesY;
    pic.img.plane[2] = data + nbytesY + nbytesUv;

    pic.img.i_stride[0] = width;
    pic.img.i_stride[1] = width / 2;
    pic.img.i_stride[2] = width / 2;

    pic.i_pts = dev->pts(fps_);
}
//------------------------------------------------------------------------------

/*virtual*/
void VideoX264Encoder::data_in(void * devData)
{
    VideoDevice * dev = static_cast<VideoDevice*>(devData);

    VideoBuffer & buffers = dev->buffers();

    VideoBuffer::Data data = buffers.dequeue(dev);

    x264_picture_t picIn;
    x264_picture_t picOut;

    x264PicInit(picIn, dev, data);

    x264_nal_t * pnal = NULL;
    int    nals_count = 0;

    do
    {
        if(x264_encoder_encode(enc_, &pnal, &nals_count, &picIn, &picOut) < 0)
        {
            buffers.enqueue(data, dev, std::nothrow);

            throw std::runtime_error("encoder error");
        }
    }
    while(x264_encoder_delayed_frames(enc_));

    buffers.enqueue(data, dev);

    for(int i = 0; i < nals_count; ++i)
    {
        channel_.sendNals(pnal[i].p_payload, pnal[i].i_payload);
    }
    channel_.delay(fps_);

    dev->nextFrame();
}

/*virtual*/
void VideoX264Encoder::data_out(void *)
{ }

/*virtual*/
void VideoX264Encoder::hang_up(void * devData)
{
    throw ExceptVideoHangUp(static_cast<VideoDevice *>(devData));
}

/*virtual*/
void VideoX264Encoder::error(void * devData, int error)
{
    throw ExceptVideoDevice(error, "", static_cast<VideoDevice *>(devData));
}

/*virtual*/
void VideoX264Encoder::interrupt(uint64_t mes)
{
    union
    {
        uint64_t data;
        uint32_t params[2];

    } message = { mes };

    reason_ = message.params[0];
    arg_    = message.params[1];
}
