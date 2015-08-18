//#define SUPPRESS_PROFILER
#include "CPProfiler.h"

using namespace std;


class profiler_tester{

    void do_work( int input ){
        _START_
        int x = 325782345 ^ (input || input << 5);
        for( int i = 0 ; i < 1234567 ; i++ )
            x ^= (i >> 3) ^ i ^ ( i << 5 );
        cout << "x = " << x << '\n';
        _STOP_
    }

  public:

    std::vector< std::vector<int> > run_test(){
        _START_V_
        for( int i = 0 ; i < 9 ; i++ )
            do_work(i*i);
        _STOP_V_
        return std::vector< std::vector<int> >();
    }

    void starter(){
        _START_V_
    }

}PT;


void func(){

    int x = 13453663;

    for( int i = 0 ; i < 12 ; i++ ){

        _START_

        for( int j = 0 ; j < 18556537 ; j++ ){
            x ^= x << 3;
            x ^= x >> 7;
        }
        cout << ( x ^ (x >> 5)) << '\n';

        _STOP_
    }

    cout << '\n';
}


void func2(){

    int x = 89237462;

    for( int i = 0 ; i < 12 ; i++ ){

        START_V( func2_timer )

        PAUSE_V( func2_timer )

        for( int j = 0 ; j < 60234876 ; j++ ){
            x ^= x << 5;
            x ^= x >> 3;
        }
        cout << ( x ^ (x >> 7)) << '\n';

        UNPAUSE_V( func2_timer )

        for( int j = 0 ; j < 6234876 ; j++ ){
            x ^= x << 5;
            x ^= x >> 3;
        }

        STOP_V( func2_timer )
    }

    cout << '\n';
}


void stopper(){
    STOP_EXTERNAL_TIMER( "profiler_tester__starter" )
}


int main( int argc, char* argv[] ){

    cout << "\n\n\n";

    func();

    START( custom_timer )

    func2();

    STOP( custom_timer )

    for( int i = 0 ; i < 3 ; i++ )
        PT.run_test();

    cout << '\n';

    int y  = 5;
    for( int i = 0 ; i < 8 ; i++ ){
        PT.starter();
        for( int j = 0 ; j < 1234567 ; j++ )
            y ^= y << (y & 5);
        cout << "y = " << y << '\n';
        stopper();
    }

    SET_PROFILER_OUTPUT( "output_test.txt" );
}