#include "core/errorhandling/errorhandler.h"

#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>
#include <typeinfo>


int ErrorHandler::handleApplicationExceptions()    // static public
{
    try
    {
        throw;
    }
    catch (std::range_error &_except)
    {
        std::cerr << "RANGE ERROR (Application): " << _except.what();
        return RANGE_ERROR;
    }
    catch (std::system_error &_except)
    {
        std::cerr << "SYSTEM ERROR (Application): " << _except.what();
        return SYSTEM_ERROR;
    }
    catch (std::runtime_error &_except)
    {
        std::cerr << "GENERAL RUNTIME ERROR (Application): " << _except.what();
        return RUNTIME_ERROR;
    }
    catch (std::invalid_argument &_except)
    {
        std::cerr << "INVALID ARGUMENT (Application): " << _except.what();
        return INVALID_ARG;
    }
    catch (std::out_of_range &_except)
    {
        std::cerr << "OUT OF RANGE ERROR (Application): " << _except.what();
        return OUT_OF_RANGE_ERROR;
    }
    catch (std::domain_error &_except)
    {
        std::cerr << "DOMAIN ERROR (Application): " << _except.what();
        return DOMAIN_ERROR;
    }
    catch (std::length_error &_except)
    {
        std::cerr << "LENGTH ERROR (Application): " << _except.what();
        return LENGTH_ERROR;
    }
    catch (std::logic_error &_except)
    {
        std::cerr << "LOGIC ERROR (Application): " << _except.what();
        return LOGIC_ERROR;
    }
    catch (std::bad_alloc &_except)
    {
        std::cerr << "BAD ALLOCATION (Application): " << _except.what();
        return BAD_ALLOC;
    }
    catch (std::bad_cast &_except)
    {
        std::cerr << "BAD CAST (Application): " << _except.what();
        return BAD_CAST;
    }
    catch (std::exception &_except)
    {
        std::cerr << "GENERAL EXCEPTION (Application): " << _except.what();
        return GENERAL_EXCEPTION;
    }
    catch (...)
    {
        std::cerr << "UNKNOWN EXCEPTION (Application): ";
        return UNKNOWN_EXCEPTION;
    }
    return NO_ERROR;
}
