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
#include <string>
#include <vector>
#include <bitset>

#include <Blazon.h>

namespace blazon {

struct BlazonIndexCard
{
	BlazonIndexCard() = default;
	BlazonIndexCard( size_t _entryIndex, const std::vector<Tag>& _tags ) {
		entryIndex = _entryIndex;
		tags = _tags;

		for ( const Tag& tag : _tags )
			tagIndexMask.set( tag.index, true );
	}

	bool matchesTags( const std::vector<Tag>& _tags ) const
	{
		for ( const auto& tag : _tags )
		{
			if ( !tagIndexMask.test( tag.index ) )
				return false;
		}

		for ( const auto& searchTag : _tags )
		{
			for ( const auto& cardTag : tags )
			{
				if ( cardTag.index != searchTag.index ) // not the same tag
					continue;

				if ( !doesTagsMatch( cardTag, searchTag ) )
					return false;

				break; // tags match, continue to next
			}
		}

		return true;
	}

	size_t entryIndex;
	std::vector<Tag> tags;
	std::bitset<128> tagIndexMask;
};

struct IndexContainer
{
	std::vector<BlazonIndexCard> indexCards;
	
	void addIndexCard( size_t _entryIndex, const std::vector<Tag>& _tags )
	{
		indexCards.push_back( { _entryIndex, _tags } );
	}

	std::vector<size_t> query( const std::vector<Tag>& _searchTags ) 
	{
		std::vector<size_t> results;

		for ( const BlazonIndexCard& card : indexCards )
		{
			if ( card.matchesTags( _searchTags ) )
				results.push_back( card.entryIndex );
		}
		
		return results;
	}
};

void printResults( std::vector<BlazonEntry>& _entries, std::vector<size_t> _results )
{
	for ( size_t r : _results )
		printf( "  %s\n", _entries[ r ].blazon.c_str() );
}

}

int main()
{
	std::vector<blazon::BlazonEntry> entries;
	entries.push_back( { 0, "blue with gold lion rampant" } );
	entries.push_back( { 1, "blue with silver lion rampant" } );
	entries.push_back( { 2, "blue with silver lion" } );
	entries.push_back( { 3, "red with gold bends" } );

	blazon::IndexContainer indexContainer{};
	indexContainer.addIndexCard(
		0,
		{
			blazon::makeTag( "field", { "azure" } ),
			blazon::makeTag( "lion", { "or", "rampant" })
		}
	);
	indexContainer.addIndexCard(
		1,
		{
			blazon::makeTag( "field", { "azure" } ),
			blazon::makeTag( "lion", { "argent", "rampant" })
		}
	);
	indexContainer.addIndexCard(
		2,
		{
			blazon::makeTag( "field", { "azure" } ),
			blazon::makeTag( "lion", { "argent" })
		}
	);
	indexContainer.addIndexCard(
		3,
		{
			blazon::makeTag( "field", { "gules" } ),
			blazon::makeTag( "bends", { "or" } )
		}
	);

	{
		std::vector<size_t> results = 
			indexContainer.query( 
				{ 
					blazon::makeTag( "field", { "azure" } ),
					blazon::makeTag( "lion", { } )
				} );
		printf( "Search results for: [field:azure,lion]\n" );
		printResults( entries, results );
	}
	{
		std::vector<size_t> results = 
			indexContainer.query( 
				{ 
					blazon::makeTag( "field", { "azure" } ),
					blazon::makeTag( "lion", { "argent" })
				} );
		printf( "Search results for: [field:azure,lion:argent]\n" );
		printResults( entries, results );
	}
	{
		std::vector<size_t> results = 
			indexContainer.query( 
				{ 
					blazon::makeTag( "field", { "gules" } )
				} );
		printf( "Search results for: [field:gules]\n" );
		printResults( entries, results );
	}


	return 0;
}
