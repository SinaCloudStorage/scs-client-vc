#ifndef __LOG4CPLUS_SWITCH_H
#define __LOG4CPLUS_SWITCH_H

#include <tchar.h>

#ifndef RECORD_LOG
#define _LOG4CPLUS_LOGGING_MACROS_HEADER_
#define LOG4CPLUS_TRACE(a,b)
#define LOG4CPLUS_DEBUG(a,b)
#define LOG4CPLUS_INFO(a,b)
#define LOG4CPLUS_WARN(a,b)
#define LOG4CPLUS_ERROR(a,b)
#define LOG4CPLUS_FATAL(a,b)

#define LOG4CPLUS_INIT(param)
#define LOG4CPLUS_DECLARE(logger)
#define LOG4CPLUS_IMPLEMENT(logger, name)
#define LOG4CPLUS_CLASS_DECLARE(logger)
#define LOG4CPLUS_CLASS_IMPLEMENT(classname, logger, name)

#define LOG4CPLUS_THIS_TRACE(a,b)
#define LOG4CPLUS_THIS_DEBUG(a,b)
#define LOG4CPLUS_THIS_INFO(a,b)
#define LOG4CPLUS_THIS_WARN(a,b)
#define LOG4CPLUS_THIS_ERROR(a,b)
#define LOG4CPLUS_THIS_FATAL(a,b)

#define LOG_CLASS_DECLARE()			
#define LOG_CLASS_IMPLEMENT(class_name, class_name_string) 

#define	LOG_TRACE( x )			
#define	LOG_DEBUG( x )			
#define	LOG_INFO( x )			
#define	LOG_WARN( x )		
#define	LOG_ERROR( x )			
#define LOG_FATAL( x )			

#define	LOG_THIS_TRACE( x )		
#define	LOG_THIS_DEBUG( x )		
#define	LOG_THIS_INFO( x )		
#define	LOG_THIS_WARN( x )		
#define	LOG_THIS_ERROR( x )		
#define LOG_THIS_FATAL( x )		


#else // !defined(RECORD_LOG)

#ifndef PRODUCT_LOG
#else
#define LOG4CPLUS_DISABLE_DEBUG
#define LOG4CPLUS_DISABLE_TRACE
#endif

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

#define LOG4CPLUS_INIT(param)	log4cplus::PropertyConfigurator::doConfigure(param)
#define LOG4CPLUS_DECLARE(logger)			extern log4cplus::Logger logger
#define LOG4CPLUS_IMPLEMENT(logger, name)	log4cplus::Logger logger = log4cplus::Logger::getInstance(name)
#define LOG4CPLUS_CLASS_DECLARE(logger)						static log4cplus::Logger logger
#define LOG4CPLUS_CLASS_IMPLEMENT(classname, logger, name)	log4cplus::Logger classname::logger = log4cplus::Logger::getInstance(name)

#define LOG4CPLUS_THIS_TRACE(a,b)	LOG4CPLUS_TRACE(a, _T('[') << this << _T(']') << b )
#define LOG4CPLUS_THIS_DEBUG(a,b)	LOG4CPLUS_DEBUG(a, _T('[') << this << _T(']') << b )
#define LOG4CPLUS_THIS_INFO(a,b)	LOG4CPLUS_INFO(a, _T('[') << this << _T(']') << b )
#define LOG4CPLUS_THIS_WARN(a,b)	LOG4CPLUS_WARN(a, _T('[') << this << _T(']') << b )
#define LOG4CPLUS_THIS_ERROR(a,b)	LOG4CPLUS_ERROR(a, _T('[') << this << _T(']') << b )
#define LOG4CPLUS_THIS_FATAL(a,b)	LOG4CPLUS_FATAL(a, _T('[') << this << _T(']') << b )

//////////////////////////////////////////////
#define LOG_CLASS_DECLARE()			LOG4CPLUS_CLASS_DECLARE( s_logger )
#define LOG_CLASS_IMPLEMENT(class_name, class_name_string) LOG4CPLUS_CLASS_IMPLEMENT( class_name, s_logger, _T("ss_")##class_name_string )

#define	LOG_TRACE( x )			LOG4CPLUS_TRACE( s_logger, x )
#define	LOG_DEBUG( x )			LOG4CPLUS_DEBUG( s_logger, x )
#define	LOG_INFO( x )			LOG4CPLUS_INFO( s_logger, x )
#define	LOG_WARN( x )			LOG4CPLUS_WARN( s_logger, x )
#define	LOG_ERROR( x )			LOG4CPLUS_ERROR( s_logger, x )
#define LOG_FATAL( x )			LOG4CPLUS_FATAL( s_logger, x )

#define	LOG_THIS_TRACE( x )		LOG4CPLUS_TRACE( s_logger, _T("[") << this << _T("] ") << x )
#define	LOG_THIS_DEBUG( x )		LOG4CPLUS_DEBUG( s_logger, _T("[") << this << _T("] ") << x )
#define	LOG_THIS_INFO( x )		LOG4CPLUS_INFO( s_logger, _T("[") << this << _T("] ") << x )
#define	LOG_THIS_WARN( x )		LOG4CPLUS_WARN( s_logger, _T("[") << this << _T("] ") << x )
#define	LOG_THIS_ERROR( x )		LOG4CPLUS_ERROR( s_logger, _T("[") << this << _T("] ") << x )
#define LOG_THIS_FATAL( x )		LOG4CPLUS_FATAL( s_logger, _T("[") << this << _T("] ") << x )
//////////////////////////////////////////////

#endif // RECORD_LOG
#endif // __LOG4CPLUS_SWITCH_H