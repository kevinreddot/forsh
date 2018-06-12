// If RESOLVE_USERNAME is defined, forsh will call getpwuid() on logging-in
// user. This may require additional setup in a chrooted environment. For more
// details see README.md.
#define RESOLVE_USERNAME

// Logging facility to use for syslog(3). Check "syslog.h" for possible
// values.
#define LOG_FACILITY LOG_AUTH
