#ifndef EXCEPT_VIDEO_SETTINGS_H_INCLUDED
#define EXCEPT_VIDEO_SETTINGS_H_INCLUDED

#include <exception>
#include <string>

#include <libconfig.h>

class ExceptVideoSettings
    : public std::exception
{
public:
    explicit ExceptVideoSettings(::config_t * cfg)
        : std::exception()
        //config_error_* functions is a macro across documentation :(
        , line_( config_error_line(cfg) )
        , file_( config_error_file(cfg) )
        , text_( config_error_text(cfg) )
    { }

    ~ExceptVideoSettings() throw() {}

    char const * what() const throw()
    {
        return text_.c_str();
    }

    int line() const throw()
    {
        return line_;
    }

    char const * file() const throw()
    {
        return file_.c_str();
    }

private:
    int         line_;
    std::string file_;
    std::string text_;
};

#endif // EXCEPT_VIDEO_SETTINGS_H_INCLUDED
