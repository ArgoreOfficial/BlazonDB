#pragma once

#include <string>
#include <vector>
#include <bitset>

namespace blazon {

typedef std::bitset<128> TagIndexBitset;

const std::vector<std::string> CONST_TAGS{
	"field",
	"per_fess",
	"lion",
	"bends",
	"per_fess"
};

const std::vector<std::string> CONST_MODIFIERS{
	"rampant", "passant", "sejant", "couchant",
	"dexter", "sinister",
	"wavy"
};

const std::vector<std::string> CONST_TINCTURES{
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
	std::string debugString = "";

	size_t index{ 0 };
	std::bitset<128> modifiersMask{ 0 };
	std::vector<uint32_t> tinctureHashes;
};

struct BlazonIndexCard
{
	BlazonIndexCard() = default;
	BlazonIndexCard( size_t _entryIndex, const std::vector<Tag>& _tags ) {
		entryIndex = _entryIndex;
		tags = _tags;

		for ( const Tag& tag : _tags )
			tagIndexMask.set( tag.index, true );
	}

	bool matchesTags( const std::vector<Tag>& _tags ) const;

	size_t entryIndex;
	std::vector<Tag> tags;
	TagIndexBitset tagIndexMask;
};

struct IndexContainer
{
	std::vector<BlazonIndexCard> indexCards;

	void addIndexCard( size_t _entryIndex, const std::vector<Tag>& _tags );
	std::vector<size_t> query( const std::vector<Tag>& _searchTags );

};

bool isModifierTincture( const std::string& _modifier );

size_t getModifierIndex( const std::string& _modifier );

Tag makeTag( const std::string& _tagName, const std::vector<std::string>& _modifierNames );

Tag makeTagFromString( const std::string& _string );

std::vector<Tag> makeTagListFromString( const std::string& _string );

const bool doesTagsMatch( const Tag& _entry, const Tag& _query );

}