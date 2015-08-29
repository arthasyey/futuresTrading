#include "../include/logging.h"

src::severity_logger< severity_level > lg;

void initLogger(const std::string& name)
{
  logging::add_file_log
    (
     keywords::file_name = name,                                        /*< file name pattern >*/
     keywords::open_mode = (std::ios::out | std::ios::app),
     keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
     keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
     keywords::format = "[%TimeStamp%]: %Message%",				                 /*< log record format >*/
     keywords::auto_flush = true
    );


  logging::aux::add_console_log(std::clog, (keywords::format = "%Message%", keywords::auto_flush = true));

  logging::core::get()->set_filter
    (
     logging::trivial::severity >= logging::trivial::info
    );

  logging::add_common_attributes();

}

void flatLogging(const std::string& str) {
  BOOST_LOG_SEV(lg, info) << str;
}

void flatLogging(int nItems, ...) {
  va_list ap;
  va_start(ap, nItems);
  stringstream ss;
  for (int i = 0; i < nItems; ++i)
  {
    char *t = va_arg(ap, char*);
    ss << t << " ";
  }
  va_end(ap);
  BOOST_LOG_SEV(lg, info) << ss.str();
}
