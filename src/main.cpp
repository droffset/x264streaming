#include <video/video_grabber.h>
#include <video/video_settings.h>
#include <video/video_x264_encoder.h>

#include <network/rtp_channel.h>

#include <signal.h>

VideoGrabber * grabber = 0;

void exit_handler(int)
{
    if(grabber)
    {
        fprintf(stdout, "\n<CTRL-C> received, exiting...\n");
        grabber->shutdown();
        grabber = 0;
    }
}

int main()
{
    try
    {
        VideoSettings settings;

        settings.open("video.cfg");

        VideoGrabber grabber(settings);

        ::grabber = &grabber;

        ::signal(SIGINT,  &exit_handler);
        ::signal(SIGKILL, &exit_handler);
        ::signal(SIGTERM, &exit_handler);
        ::signal(SIGQUIT, &exit_handler);

        RtpChannel channel(settings);

        VideoX264Encoder encoder(channel);

        grabber.exec(encoder);
    }
    catch(std::exception const & e)
    {
        printf("ERROR: [%s]\n", e.what());
    }
}
