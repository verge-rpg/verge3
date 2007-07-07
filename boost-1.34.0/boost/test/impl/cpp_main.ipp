//  (C) Copyright Gennadiy Rozental 2001-2005.
//  (C) Copyright Beman Dawes 1995-2001.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: cpp_main.ipp,v $
//
//  Version     : $Revision: 1.7 $
//
//  Description : main function implementation for Program Executon Monitor
// ***************************************************************************

#ifndef BOOST_TEST_CPP_MAIN_IPP_012205GER
#define BOOST_TEST_CPP_MAIN_IPP_012205GER

// Boost.Test
#include <boost/test/execution_monitor.hpp>
#include <boost/test/detail/config.hpp>
#include <boost/test/utils/basic_cstring/io.hpp>

// Boost
#include <boost/cstdlib.hpp>    // for exit codes
#include <boost/config.hpp>     // for workarounds

// STL
#include <iostream>
#include <cstdlib>      // std::getenv

#include <boost/test/detail/suppress_warnings.hpp>

#include <boost/test/detail/suppress_warnings.hpp>

//____________________________________________________________________________//

#ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::getenv; }
#endif

namespace {

struct cpp_main_caller {
    cpp_main_caller( int (*cpp_main_func)( int argc, char* argv[] ), int argc, char** argv ) 
    : m_cpp_main_func( cpp_main_func )
    , m_argc( argc )
    , m_argv( argv ) {}
    
    int operator()() { return (*m_cpp_main_func)( m_argc, m_argv ); }
  
private:
    // Data members    
    int (*m_cpp_main_func)( int argc, char* argv[] );
    int      m_argc;
    char**   m_argv;
};

} // local namespace

// ************************************************************************** //
// **************             prg_exec_monitor_main            ************** //
// ************************************************************************** //

namespace boost {

int BOOST_TEST_DECL
prg_exec_monitor_main( int (*cpp_main)( int argc, char* argv[] ), int argc, char* argv[] )
{
    int result;

    boost::unit_test::const_string p( std::getenv( "BOOST_TEST_CATCH_SYSTEM_ERRORS" ) );
    bool catch_system_errors = p != "no";
        
    try {
        ::boost::execution_monitor ex_mon;
        result = ex_mon.execute( 
            ::boost::unit_test::callback0<int>( cpp_main_caller( cpp_main, argc, argv ) ), catch_system_errors );
        
        if( result == 0 )
            result = ::boost::exit_success;
        else if( result != ::boost::exit_success ) {
            std::cout << "\n**** error return code: " << result << std::endl;
            result = ::boost::exit_failure;
        }
    }
    catch( ::boost::execution_exception const& exex ) {
        std::cout << "\n**** exception(" << exex.code() << "): " << exex.what() << std::endl;
        result = ::boost::exit_exception_failure;
    }
    
    if( result != ::boost::exit_success ) {
        std::cerr << "******** errors detected; see standard output for details ********" << std::endl;
    }
    else {
        //  Some prefer a confirming message when all is well, while others don't
        //  like the clutter.  Use an environment variable to avoid command
        //  line argument modifications; for use in production programs
        //  that's a no-no in some organizations.
        ::boost::unit_test::const_string p( std::getenv( "BOOST_PRG_MON_CONFIRM" ) );
        if( p != "no" ) { 
            std::cerr << std::flush << "no errors detected" << std::endl; 
        }
    }

    return result;
}

} // namespace boost

#if !defined(BOOST_TEST_DYN_LINK) && !defined(BOOST_TEST_NO_MAIN)

// ************************************************************************** //
// **************        main function for tests using lib     ************** //
// ************************************************************************** //

int cpp_main( int argc, char* argv[] );  // prototype for user's cpp_main()

int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
    return ::boost::prg_exec_monitor_main( &cpp_main, argc, argv );
}

//____________________________________________________________________________//

#endif // !BOOST_TEST_DYN_LINK && !BOOST_TEST_NO_MAIN

//____________________________________________________________________________//

#include <boost/test/detail/enable_warnings.hpp>

// ***************************************************************************
//  Revision History :
//  
//  $Log: cpp_main.ipp,v $
//  Revision 1.7  2006/03/19 11:45:26  rogeeff
//  main function renamed for consistancy
//
//  Revision 1.6  2005/12/14 05:27:21  rogeeff
//  cpp_main API modified for DLL
//
// ***************************************************************************

#endif // BOOST_TEST_CPP_MAIN_IPP_012205GER
