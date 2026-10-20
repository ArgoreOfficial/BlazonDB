#include "Blazon.h"

#include <sstream>
#include <math.h>

#include "hash.h"

bool blazon::BlazonEntry::matchesTags( const std::vector<Tag>& _tags, bool _skipBitset ) const
{
	if ( !_skipBitset )
	{
		for ( const auto& tag : _tags )
		{
			if ( !tagBitset.test( tag.index ) )
				return false;
		}
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

void blazon::BlazonDatabase::addEntry( const std::string& _blazon, const std::vector<Tag>& _tags, const std::string& _description )
{
	BlazonEntry entry{ _blazon, _tags, _description };
	tagIndices.push_back( entry.tagBitset );
	entries.push_back( std::move( entry ) );
}

void blazon::BlazonDatabase::addEntry( const std::string& _blazon, const std::string& _tags, const std::string& _description )
{
	BlazonEntry entry{ _blazon, blazon::makeTagListFromString( _tags ), _description };
	tagIndices.push_back( entry.tagBitset );
	entries.push_back( std::move( entry ) );
}

std::vector<size_t> blazon::BlazonDatabase::query( const std::vector<Tag>& _searchTags )
{
	std::vector<size_t> results;

	for ( size_t i = 0; i < entries.size(); i++ )
	{
		const BlazonEntry& card = entries[ i ];
		if ( card.matchesTags( _searchTags ) )
			results.push_back( i );
	}

	return results;
}

std::vector<size_t> blazon::BlazonDatabase::queryIndices( const std::vector<Tag>& _searchTags )
{
	TagBitset queryBitset = makeBitsetFromTags( _searchTags );
	std::vector<size_t> presearchEntries{};
	std::vector<size_t> results{};

	for ( size_t i = 0; i < tagIndices.size(); i++ )
	{
		if ( ( queryBitset & tagIndices[ i ] ) == queryBitset )
			presearchEntries.push_back( i );
	}
	
	for ( size_t i = 0; i < presearchEntries.size(); i++ )
	{
		size_t entriesIndex = presearchEntries[ i ];

		const BlazonEntry& card = entries[ entriesIndex ];
		if ( card.matchesTags( _searchTags ) )
			results.push_back( entriesIndex );
	}
	
	return results;
}

std::vector<unsigned long long> blazon::BlazonDatabase::generateIndexTable()
{
	std::vector<unsigned long long> vec;
	for ( size_t i = 0; i < entries.size(); i++ )
		vec.push_back( entries[ i ].tagBitset.to_ullong() );
	
	return vec;
}

bool blazon::isModifierTincture( const std::string& _modifier )
{
	for ( const auto& t : CONST_TINCTURES )
	{
		if ( t != _modifier )
			continue;
		return true;
	}

	return false;
}

size_t blazon::getModifierIndex( const std::string& _modifier )
{
	for ( size_t i = 0; i < CONST_MODIFIERS.size(); i++ )
	{
		if ( CONST_MODIFIERS[ i ] != _modifier )
			continue;

		return i;
	}

	return -1;
}

blazon::Tag blazon::makeTag( const std::string& _tagName, const std::vector<std::string>& _modifierNames )
{
	Tag tag{};

	for ( size_t i = 0; i < CONST_TAGS.size(); i++ )
	{
		if ( _tagName != CONST_TAGS[ i ] )
			continue;

		tag.index = i;
		break;
	}

	std::vector<std::string> tinctureTags;

	// build modifier mask
	for ( const std::string& modifierName : _modifierNames )
	{
		if ( isModifierTincture( modifierName ) )
		{
			tinctureTags.push_back( modifierName );
		}
		else
		{
			size_t index = getModifierIndex( modifierName );

			if ( index != (size_t)-1 ) // nonexistent modifiers are ignored
				tag.modifiersBitset.set( index, true );
		}
	}

	if ( !tinctureTags.empty() )
	{
		std::string currentTinctureCombo = "";
		for ( size_t i = 0; i < tinctureTags.size(); i++ )
		{
			currentTinctureCombo += tinctureTags[ i ];
			uint32_t hash = wv::Hash::djb2( currentTinctureCombo );
			tag.tinctureHashes.push_back( hash );
		}
	}

	return tag;
}

blazon::Tag blazon::makeTagFromString( const std::string& _string )
{
	std::stringstream sstream( _string );
	std::string tempSegment;

	std::string tagString;
	std::vector<std::string> modifiers;

	if ( std::getline( sstream, tempSegment, ':' ) )
		tagString = tempSegment;

	while ( std::getline( sstream, tempSegment, ':' ) )
		modifiers.push_back( tempSegment );

	Tag tag = makeTag( tagString, modifiers );
	return tag;
}

blazon::TagBitset blazon::makeBitsetFromTags( const std::vector<Tag>& _tags )
{
	TagBitset bitset{};
	for ( const Tag& tag : _tags )
		bitset.set( tag.index, true );
	return bitset;
}

std::vector<blazon::Tag> blazon::makeTagListFromString( const std::string& _string )
{
	std::stringstream sstream( _string );
	std::string tempSegment;
	std::vector<std::string> tagStrings;

	while ( std::getline( sstream, tempSegment, ',' ) )
		tagStrings.push_back( tempSegment );

	std::vector<Tag> tags;
	for ( const auto& str : tagStrings )
		tags.push_back( makeTagFromString( str ) );

	return tags;
}

const bool blazon::doesTagsMatch( const Tag& _entry, const Tag& _query )
{
	// check tag index
	if ( _entry.index != _query.index )
		return false;

	// check modifiers
	if ( ( _entry.modifiersBitset & _query.modifiersBitset ) != _query.modifiersBitset )
		return false;

	// check tinctures, this only fails if two hashes of the same depth are different
	// meaning [azure:or] will match with both [azure] and [azure:or] but not [azure:argent]

	// if the query has MORE tinctures than the entry, then we know it can never match
	if ( _query.tinctureHashes.size() > _entry.tinctureHashes.size() )
		return false;

	for ( size_t i = 0; i < _query.tinctureHashes.size(); i++ )
	{
		if ( _entry.tinctureHashes[ i ] != _query.tinctureHashes[ i ] )
			return false;
	}

	return true;
}

void blazon::to_json( nlohmann::json& _json, const blazon::Tag& _p )
{
	std::vector<int> tagModifierIndices{};

	for ( size_t i = 0; i < _p.modifiersBitset.size(); i++ )
	{
		if ( _p.modifiersBitset.test( i ) )
			tagModifierIndices.push_back( i );
	}

	_json = nlohmann::json{
		{ "tagIndex", _p.index },
		{ "modifiers", tagModifierIndices },
		{ "tinctureHashes", _p.tinctureHashes }
	};
}

void blazon::from_json( const nlohmann::json& _json, blazon::Tag& _p )
{
	std::vector<int> tagModifierIndices{};

	_json.at( "tagIndex" ).get_to( _p.index );
	_json.at( "modifiers" ).get_to( tagModifierIndices );
	_json.at( "tinctureHashes" ).get_to( _p.tinctureHashes );

	for ( size_t i = 0; i < tagModifierIndices.size(); i++ )
		_p.modifiersBitset.set( tagModifierIndices[ i ] );
}

void blazon::to_json( nlohmann::json& _json, const blazon::BlazonEntry& _p )
{
	_json = {
		{ "blazon", _p.blazon },
		{ "description", _p.description },
		{ "hash", _p.tagsHash },
		{ "tags", _p.tags }
	};
}

void blazon::from_json( const nlohmann::json& _json, blazon::BlazonEntry& _p )
{
	_json.at( "blazon" ).get_to( _p.blazon );
	_json.at( "description" ).get_to( _p.description );
	_json.at( "hash" ).get_to( _p.tagsHash );
	_json.at( "tags" ).get_to( _p.tags );
}