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

#include <stdio.h>
#include <fstream>

#include <util.h>

#include <Blazon.h>
#include <nlohmann/json.hpp>

int main()
{
	std::srand( std::time( 0 ) );

	blazon::BlazonDatabase db{};
	db.loadIndexTable();
		
	// Test
	{
		std::vector<std::string> results = db.queryIndices( blazon::makeTagListFromString( "field:azure" ) );
		for ( const std::string& r : results )
		{
			blazon::BlazonEntry entry = db.getEntry( r );
			printf( "%s\n", entry.blazon.c_str() );
		}
	}

//	blazon::BlazonEntry testBlazon( "azure, three crowns or two over one", "field:azure,crown:or:three:2_over_1", "The Kingdom of Sweden lesser arms" );
//	db.createEntry( testBlazon );
//	db.cacheEntry( testBlazon );

//	db.generateIndexTable();

	return 0;
}
