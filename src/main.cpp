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

#include <Blazon.h>
#include <nlohmann/json.hpp>

static void queryAndPrintResults( blazon::BlazonDatabase& _indexContainer, const std::string& _search )
{
	std::vector<size_t> results = _indexContainer.queryIndices( blazon::makeTagListFromString( _search ) );
	printf( "Search results for: [%s]\n", _search.c_str() );
	
	for ( size_t r : results )
		printf( "  %s\n", _indexContainer.entries[ r ].blazon.c_str() );
}


int main()
{
	blazon::BlazonDatabase indexContainer{};
	indexContainer.addEntry( "azure, a lion rampant or",     "field:azure,lion:or:rampant" );
	indexContainer.addEntry( "azure, a lion rampant argent", "field:azure,lion:argent:rampant" );
	indexContainer.addEntry( "azure, a lion argent",         "field:azure,lion:argent" );
	indexContainer.addEntry( "gules, two bends or",          "field:gules,bends:or" );
	indexContainer.addEntry( "party per fess gules and or",  "per_fess:gules:or" );

	queryAndPrintResults( indexContainer, "field:azure,lion" );
	queryAndPrintResults( indexContainer, "field:azure,lion:argent" );
	queryAndPrintResults( indexContainer, "field:gules" );
	queryAndPrintResults( indexContainer, "field:gules,bends" );
	queryAndPrintResults( indexContainer, "field:gules,bends:or" );
	queryAndPrintResults( indexContainer, "field:gules,bends:argent" );
	queryAndPrintResults( indexContainer, "field:gules,bends:or:argent" );
	queryAndPrintResults( indexContainer, "per_fess:gules" );
	queryAndPrintResults( indexContainer, "per_fess:gules:or" );
	queryAndPrintResults( indexContainer, "per_fess:or" );
	queryAndPrintResults( indexContainer, "per_fess:or:gules" );
	
	// JSON Test
	{
		std::vector<size_t> results = indexContainer.queryIndices( blazon::makeTagListFromString( "field:azure,lion:argent" ) );
		for ( size_t r : results )
		{
			nlohmann::json json = indexContainer.entries[ r ];
			printf( "JSON:\n%s\n", json.dump().c_str() );
		}
	}

	std::vector<unsigned long long> indextable = indexContainer.generateIndexTable();
	{
		std::ofstream file( "index_table.dat", std::ios::out | std::ios::binary );
		size_t size = indextable.size();
		file.write( (char*)&size, sizeof( size_t ) );
		file.write( (char*)indextable.data(), sizeof( unsigned long long ) * indextable.size() );
		file.close();
	}

	{
		std::ifstream file( "index_table.dat", std::ios::binary );
		std::vector<unsigned long long> indextable2;

		size_t size;
		file.read( (char*)&size, sizeof( size_t ) );
		indextable2.resize( size );
		file.read( (char*)indextable2.data(), sizeof( unsigned long long ) * size );

		file.close();
	}

	return 0;
}
