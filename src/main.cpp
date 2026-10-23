/**
 * @file main.cpp
 * @author Argore
 * @brief Basic xmake C++ Project Template
 * @version 0.1
 * @date 2025-04-04
 * 
 * @copyright Copyright (c) 2025 Argore
 * 
 */

#include <gui/App.h>

int main( int _argc, char* _argv[] )
{
	std::srand( std::time( 0 ) );

	wxInitAllImageHandlers();
	wxEntry( _argc, _argv );
	return 0;
}

wxIMPLEMENT_APP_NO_MAIN( blazon::Application );
