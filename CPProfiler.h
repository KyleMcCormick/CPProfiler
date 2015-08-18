/**                             Document Information                         **\
 *                             ----------------------
 *  Document Name   :   CPProfiler.h
 *
 *  Title           :   C++ Profiler
 *
 *  Type            :   C++11/14 header-only class ( + static member inits )
 *
 *  Description     :   This header is designed to allow a user to easily
 *                      profile their C++ program by inserting simple macros
 *                      into their code.
 *
 *  Modifications   :   15    July   2015 - Kyle McCormick
 *                          Initial Draft : Basic functionality implemented
 *
 *                      20    July   2015 - Kyle McCormick
 *                           Macros Added : Added START(), STOP(), and
 *                                          STOP_EXTERNAL_TIMER() macros
 *
 *                      20    July   2015 - Kyle McCormick
 *                           Scope Naming : Added the ability to automatically
 *                                          name timers after their enclosing
 *                                          scope via the macros _START_,
 *                                          _STOP_, etc...
 *
 *                      23    July   2015 - Kyle McCormick
 *                        Static Init Fix : Fixed an error with static
 *                                          object destruction caused by wrongly
 *                                          ordered static initialization
 *
 *                      24    July   2015 - Kyle McCormick
 *                         Verbose Output : Added support for individual timing
 *                                          listings, plus the shorthand '_V'
 *                                          macros
 *
 *                      25    July   2015 - Kyle McCormick
 *                        Pkg Suppression : Added profiler suppression support
 *                                          via the SUPPRESS_PROFILER macro
 *
 *                      25    July   2015 - Kyle McCormick
 *                               Pausable : Made it possible to pause and
 *                                          unpause timers and added appropriate
 *                                          macros
 *
 *                      25    July   2015 - Kyle McCormick
 *                       Class Scope Name : Fixed scope-based naming for methods
 *                                          in a class via the
 *                                          __PRETTY_FUNCTION__ macro and a
 *                                          function `str func_name(str)`
 *                                          (GNU C support only)
 *
 *                      29    July   2015 - Kyle McCormick
 *                        Output Redirect : Added and debugged the
 *                                          set_output_file function and
 *                                          matching macro
 *
 **                                                                          **/


/********************************  TODO LIST  *********************************/
//
// TODO: ensure each error is appropriately thrown
//
// TODO: add "lap()" method?
//
// TODO: add parallel current method sampling ?
//         via stack with ENTER/EXIT macros?
//         or with singular field and ENTRY macros only?
//
// TODO: comment all functions
//
// TODO: create usage document
//
// TODO: write unit test set
//
/******************************************************************************/


#ifndef __CPPROFILER_H__
#define __CPPROFILER_H__


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <cfloat>
#include <cmath>
#include <iomanip>



class profiling_clock {


    /****************************  Private Fields  ****************************/


    std::chrono::high_resolution_clock::time_point timer;

    std::string name;

    std::vector< double > times;

    unsigned int num_samples;

    double total, avg, min, max, paused_time;

    bool running, paused, verbose;


    /***************************  Static Variables  ***************************/


    static bool error_suppress;

    static std::map< std::string, profiling_clock* > clocks;

    static std::string output_filename;


    /***************************  Private Methods  ****************************/


    void quit( std::string Message, int ErrorNo ){
        std::cerr << "\n  Error: "  << Message
                  << " (in timer `" << name << "`)\n";
        error_suppress = true;
        exit( ErrorNo );
    }


    inline profiling_clock(
        char const* const Name,
        bool Verbose   = false
    ){
        #ifndef SUPPRESS_PROFILER
        name        = Name;
        num_samples = 0;
        running     = false;
        paused      = false;
        verbose     = Verbose;
        total       = 0.0;
        min         = DBL_MAX;
        max         = 0.0;
        #endif
    }


    inline profiling_clock()
     : profiling_clock( "[ This Program ]", false )
    {
        start();
    }


    /************************  Private Static Methods  ************************/


    static profiling_clock* master(){
        static profiling_clock* _master = new profiling_clock();
        return _master;
    }


    static void print_report(){
        #ifndef SUPPRESS_PROFILER
        std::ostream* output = output_filename.size()                  ?
                                    new std::ofstream(output_filename) :
                                    &std::cout                         ;
        profiling_clock* M = master();
        M-> stop();
        M-> print( *output );
        for( auto it = M->clocks.begin() ; it != M->clocks.end() ; it++ )
            it->second->print( *output );

        if( output_filename.size() )
            delete output;
        #endif
    }



  public:



    /************************  Public Static Methods  *************************/


    static void set_output_file( std::string Filename ){
        #ifndef SUPPRESS_PROFILER
        output_filename = Filename;
        #endif
    }


    static profiling_clock* get_clock(
        char const* const Name,
        bool Verbose = false
    ){
        #ifndef SUPPRESS_PROFILER
            if( clocks.count( Name ) )
                return clocks[ Name ];
            profiling_clock* new_clock = new profiling_clock( Name, Verbose );
            clocks.insert (
                std::pair< std::string, profiling_clock* > ( Name, new_clock )
            );
            return new_clock;
        #else
            return nullptr;
        #endif
    }


    inline static int init_class(){
        #ifndef SUPPRESS_PROFILER
        static bool already_initialized = false;
        if( already_initialized )
            return 1;
        already_initialized = true;
        #ifdef __GNUC__
        __attribute__((unused))
        #endif
        static profiling_clock* garbage_var = profiling_clock :: master();
        atexit( print_report );
        #endif
        return 0;
    }


    /****************************  Public Methods  ****************************/


    inline void start(){
        #ifndef SUPPRESS_PROFILER

        if( running )
            quit( "'start' was attempted on an already running timer", 161 );

        if( paused )
            quit( "'start' was attempted on a paused timer", 162 );

        running = true;

        paused_time = 0.0;

        timer   = std::chrono::high_resolution_clock::now();

        #endif
    }


    inline void stop(){
        #ifndef SUPPRESS_PROFILER

        using namespace std::chrono;

        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        if( ! running )
            quit( "'stop' was attempted on a timer which wasnt running", 163 );

        if( paused )
            quit( "'stop' was attempted on a paused timer", 164 );

        running = false;

        times.push_back(
            duration_cast< duration< double > > (t2 - timer).count()
            + paused_time
        );

        num_samples++;

        #endif
    }


    inline void pause(){
        #ifndef SUPPRESS_PROFILER

        using namespace std::chrono;

        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        if( ! running)
            quit( "'pause' was attempted on a timer which wasnt running", 165 );

        if( paused )
            quit( "'pause' was attempted on an already paused timer", 166 );

        paused = true;

        paused_time += duration_cast< duration< double > > (t2 - timer).count();

        #endif
    }


    inline void unpause(){
        #ifndef SUPPRESS_PROFILER

        if( ! running )
            quit(
                "'unpause' was attempted on a timer which wasnt running",
                 167
            );

        if( ! paused )
            quit(
                "'unpause' was attempted on a timer which wasnt paused",
                 168
            );

        paused = false;

        timer = std::chrono::high_resolution_clock::now();

        #endif
    }


    inline void print( std::ostream &Output_Device = std::cout ){

        #ifndef SUPPRESS_PROFILER

        if( error_suppress || ! num_samples )
            return;

        min = times[ 0 ];

        for( unsigned int i = 0 ; i < num_samples ; i++ ){
            total += times[i];
            min    = times[i] < min ? times[i] : min;
            max    = times[i] > max ? times[i] : max;
        }

        avg = total / num_samples;

        Output_Device << std::showpoint << std::fixed;

        if( num_samples > 2 ){
            Output_Device << "\n--------------------------------\n\n [ " << name
                          << " ]\n\n #  calls : " << num_samples
                          <<     "\n #  total : " << total
                          << " sec\n #    avg : " << avg
                          << " sec\n #    min : " << min
                          << " sec\n #    max : " << max
                          << " sec\n\n";
        } else {
            std::string num_string[] = {"","One call","Two calls"};
            Output_Device << "--------------------------------\n\n [ "
                          << name     << " ]\n\n #  "
                          << num_string[ num_samples ] << ":\n";
        }

        int width = (int) log10( (double) num_samples + 1 ) + 1;

        if( verbose || ( num_samples < 3 ) ){
            for( unsigned int i = 0 ; i < num_samples ; i++ )
                Output_Device << std::setw(width+2) << i+1
                              << " : " << times[i] << " sec\n";
            Output_Device << '\n';
        }

        #endif
    }

};



/***********************  Static Member Initialization  ***********************/


bool profiling_clock::error_suppress = false;

std::string profiling_clock::output_filename = "";

std::map< std::string, profiling_clock* > profiling_clock::clocks;

static int ignore_this_garbage_variable = profiling_clock::init_class();



/****************************  Ease-of-Use Macros  ****************************/


const char* func_name( std::string Input ){
    // Removes all typename specifiers and parens from the __PRETTY_FUNCTION__
    // string, replaces colons with underscores, and returns the modified string

    int i;
    for( i = 0 ; i < (int) Input.size() && Input[i] != '(' ; i++ )
        if( Input[i] == ':' )
            Input[i] = '_';

    if( i == (int) Input.size() )
        return Input.c_str();

    int left_paren = i;

    for( ; i >= 0 && Input[i] != ' ' ; i-- ) ;

    i++;

    return Input.substr( i, left_paren - i ).c_str();
}



  #ifndef SUPPRESS_PROFILER


    #define SET_PROFILER_OUTPUT( File_Name )  \
        profiling_clock::set_output_file( File_Name );


    // Scope-Named Concise Timer Macros //

    #ifdef __GNUC__
        #define _START_                                                        \
            static auto autotimer_##__FUNCTION__                               \
                = profiling_clock::get_clock(func_name(__PRETTY_FUNCTION__));  \
            autotimer_##__FUNCTION__ -> start();
    #else
        #define _START_                                      \
            static auto autotimer_##__FUNCTION__             \
                = profiling_clock::get_clock(__FUNCTION__);  \
            autotimer_##__FUNCTION__ -> start();
    #endif

    #define _STOP_     autotimer_##__FUNCTION__ -> stop();

    #define _PAUSE_    autotimer_##__FUNCTION__ -> pause();

    #define _UNPAUSE_  autotimer_##__FUNCTION__ -> unpause();



    // Scope-Named Verbose Timer Macros //

    #ifdef __GNUC__
        #define _START_V_                            \
            static auto autotimer_##__FUNCTION__     \
                = profiling_clock::get_clock(        \
                    func_name(__PRETTY_FUNCTION__),  \
                    true                             \
                  );                                 \
            autotimer_##__FUNCTION__ -> start();
    #else
        #define _START_V_                                           \
            static auto autotimer_##__FUNCTION__                    \
                = profiling_clock::get_clock(__FUNCTION__, true );  \
            autotimer_##__FUNCTION__ -> start();
    #endif

    #define _STOP_V_     _STOP_

    #define _PAUSE_V_    _PAUSE_

    #define _UNPAUSE_V_  _UNPAUSE_



    // Custom-Named Concise Timer Macros //

    #define START( Clock_Name )  \
        static auto Clock_Name = profiling_clock::get_clock( #Clock_Name );  \
        Clock_Name -> start();

    #define STOP( Clock_Name )     Clock_Name -> stop();

    #define PAUSE( Clock_Name )    Clock_Name -> pause();

    #define UNPAUSE( Clock_Name )  Clock_Name -> unpause();



    // Custom-Named Verbose Timer Macros //

    #define START_V( Clock_Name )                               \
        static auto Clock_Name                                  \
            = profiling_clock::get_clock( #Clock_Name, true );  \
        Clock_Name -> start();

    #define STOP_V( Clock_Name )     STOP( Clock_Name )

    #define PAUSE_V( Clock_Name )    PAUSE( Clock_Name )

    #define UNPAUSE_V( Clock_Name )  UNPAUSE( Clock_Name )



    // Macros for Timers Started Outside the Current Scope //

    #define STOP_EXTERNAL_TIMER( Clock_Name )  \
        static auto ext_timer_##__COUNTER__ = profiling_clock::get_clock( Clock_Name );  \
        ext_timer_##__COUNTER__ -> stop();

    #define PAUSE_EXTERNAL_TIMER( Clock_Name )  \
        static auto ext_timer_##__COUNTER__ = profiling_clock::get_clock( Clock_Name );  \
        ext_timer_##__COUNTER__ -> pause();

    #define UNPAUSE_EXTERNAL_TIMER( Clock_Name )  \
        static auto ext_timer_##__COUNTER__ = profiling_clock::get_clock( Clock_Name );  \
        ext_timer_##__COUNTER__ -> unpause();


  #else


    #define SET_PROFILER_OUTPUT( File_Name )
    #define _START_
    #define _STOP_
    #define _PAUSE_
    #define _UNPAUSE_
    #define _START_V_
    #define _STOP_V_
    #define _PAUSE_V_
    #define _UNPAUSE_V_
    #define START( Clock_Name )
    #define STOP( Clock_Name )
    #define PAUSE( Clock_Name )
    #define UNPAUSE( Clock_Name )
    #define START_V( Clock_Name )
    #define STOP_V( Clock_Name )
    #define PAUSE_V( Clock_Name )
    #define UNPAUSE_V( Clock_Name )
    #define STOP_EXTERNAL_TIMER( Clock_Name )
    #define PAUSE_EXTERNAL_TIMER( Clock_Name )
    #define UNPAUSE_EXTERNAL_TIMER( Clock_Name )


  #endif


#endif