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

/*
int main( int _argc, char* _argv[] )
{
	std::srand( std::time( 0 ) );

	wxInitAllImageHandlers();
	return wxEntry( _argc, _argv );
}
*/

int APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow )
{
	std::srand( std::time( 0 ) );

	wxInitAllImageHandlers();
	return wxEntry( hInst, hInstPrev, cmdline, cmdshow );
}

wxIMPLEMENT_APP_NO_MAIN( blazon::Application );
