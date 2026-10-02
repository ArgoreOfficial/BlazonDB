#include "Blazon.h"

#include <sstream>
#include <math.h>

#include "hash.h"

bool blazon::BlazonIndexCard::matchesTags( const std::vector<Tag>& _tags ) const
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

void blazon::IndexContainer::addIndexCard( size_t _entryIndex, const std::vector<Tag>& _tags )
{
	indexCards.push_back( { _entryIndex, _tags } );
}

std::vector<size_t> blazon::IndexContainer::query( const std::vector<Tag>& _searchTags )
{
	std::vector<size_t> results;

	for ( const BlazonIndexCard& card : indexCards )
	{
		if ( card.matchesTags( _searchTags ) )
			results.push_back( card.entryIndex );
	}

	return results;
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
				tag.modifiersMask.set( index, true );
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
	tag.debugString = _string;
	return tag;
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
	if ( ( _entry.modifiersMask & _query.modifiersMask ) != _query.modifiersMask )
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
