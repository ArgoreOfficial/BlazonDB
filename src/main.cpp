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

#include <Blazon.h>

static void queryAndPrintResults( std::vector<blazon::BlazonEntry>& _entries, blazon::IndexContainer& _indexContainer, const std::string& _search )
{
	std::vector<size_t> results = _indexContainer.query( blazon::makeTagListFromString( _search ) );
	printf( "Search results for: [%s]\n", _search.c_str() );
	
	for ( size_t r : results )
		printf( "  %s\n", _entries[ r ].blazon.c_str() );
}


int main()
{
	std::vector<blazon::BlazonEntry> entries;
	entries.push_back( { 0, "azure, a lion rampant or" } );
	entries.push_back( { 1, "azure, a lion rampant argent" } );
	entries.push_back( { 2, "azure, a lion argent" } );
	entries.push_back( { 3, "gules, two bends or" } );
	entries.push_back( { 4, "party per fess gules and or" } );

	blazon::IndexContainer indexContainer{};
	indexContainer.addIndexCard( 0, blazon::makeTagListFromString( "field:azure,lion:or:rampant" ) );
	indexContainer.addIndexCard( 1, blazon::makeTagListFromString( "field:azure,lion:argent:rampant" ) );
	indexContainer.addIndexCard( 2, blazon::makeTagListFromString( "field:azure,lion:argent" ) );
	indexContainer.addIndexCard( 3, blazon::makeTagListFromString( "field:gules,bends:or" ) );
	indexContainer.addIndexCard( 4, blazon::makeTagListFromString( "per_fess:gules:or" ) );

	queryAndPrintResults( entries, indexContainer, "field:azure,lion" );
	queryAndPrintResults( entries, indexContainer, "field:azure,lion:argent" );
	queryAndPrintResults( entries, indexContainer, "field:gules" );
	queryAndPrintResults( entries, indexContainer, "field:gules,bends" );
	queryAndPrintResults( entries, indexContainer, "field:gules,bends:or" );
	queryAndPrintResults( entries, indexContainer, "field:gules,bends:argent" );
	queryAndPrintResults( entries, indexContainer, "field:gules,bends:or:argent" );
	queryAndPrintResults( entries, indexContainer, "per_fess:gules" );
	queryAndPrintResults( entries, indexContainer, "per_fess:gules:or" );
	queryAndPrintResults( entries, indexContainer, "per_fess:or" );
	queryAndPrintResults( entries, indexContainer, "per_fess:or:gules" );
	
	return 0;
}
