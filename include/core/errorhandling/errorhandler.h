#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

// Static class

class ErrorHandler
{
public:
    enum ExitCode {
        NO_ERROR = 0,
        GENERAL_EXCEPTION = 1,
        UNKNOWN_EXCEPTION = 2,
        LOGIC_ERROR = 10,
        INVALID_ARG = 11,
        OUT_OF_RANGE_ERROR = 12,
        DOMAIN_ERROR = 13,
        LENGTH_ERROR = 14,
        RUNTIME_ERROR = 20,
        SYSTEM_ERROR = 21,
        RANGE_ERROR = 22,
        BAD_ALLOC = 30,
        BAD_CAST = 40,
    };


    static int handleApplicationExceptions();

private:
    ErrorHandler() = delete;
    ~ErrorHandler() = delete;
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler &operator=(const ErrorHandler) = delete;
};

#endif // ERRORHANDLER_H
