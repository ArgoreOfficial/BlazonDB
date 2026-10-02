#pragma once

#include <math.h>

#include "hash.h"

namespace blazon {

const std::vector<std::string> tags{
	"field",
	"per_fess",
	"lion",
	"bends"
};

const std::vector<std::string> modifiers{
	"rampant", "passant", "sejant", "couchant",

	"dexter", "sinister",

	"wavy"
};

const std::vector<std::string> tinctures{
	"azure", "gules", "vert", "sable",
	"argent", "or",
	"ermine", "vair",
	"proper"
};

struct BlazonEntry
{
	size_t index;
	std::string blazon;
};

struct Tag
{
	size_t index{ 0 };
	std::bitset<128> modifiersMask{ 0 };
	std::vector<uint32_t> tinctureHashes;
};

static bool isModifierTincture( const std::string& _modifier )
{
	for ( const auto& t : tinctures )
	{
		if ( t != _modifier )
			continue;
		return true;
	}

	return false;
}

static size_t getModifierIndex( const std::string& _modifier )
{
	for ( size_t i = 0; i < modifiers.size(); i++ )
	{
		if ( modifiers[ i ] != _modifier )
			continue;

		return i;
	}

	return -1;
}

static Tag makeTag( const std::string& _tagName, const std::vector<std::string>& _modifierNames ) {
	Tag tag{};

	for ( size_t i = 0; i < tags.size(); i++ )
	{
		if ( tags[ i ] != _tagName )
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

			if( index != (size_t)-1 ) // nonexistent modifiers are ignored
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

static const bool doesTagsMatch( const Tag& _entry, const Tag& _query )
{
	// check tag index
	if ( _entry.index != _query.index )
		return false;

	// check modifiers
	if ( ( _entry.modifiersMask & _query.modifiersMask ) != _query.modifiersMask )
		return false;

	// check tinctures, this only fails if two hashes of the same depth are different
	// meaning [azure:or] will match with both [azure] and [azure:or] but not [azure:argent]
	for ( size_t i = 0; i < std::min( _a.tinctureHashes.size(), _b.tinctureHashes.size() ); i++ )
	{
		if ( _a.tinctureHashes[ i ] != _b.tinctureHashes[ i ] )
			return false;
	}

	return true;
}

}