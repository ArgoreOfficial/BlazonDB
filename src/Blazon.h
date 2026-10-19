#pragma once

#include <string>
#include <vector>
#include <bitset>

namespace blazon {

typedef std::bitset<128> TagIndexBitset;
typedef std::bitset<128> ModifierBitset;

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

struct Tag
{
	std::string debugString = "";

	size_t index{ 0 };
	ModifierBitset modifiersMask{ 0 };
	std::vector<uint32_t> tinctureHashes;
};

struct BlazonEntry
{
	BlazonEntry() = default;
	BlazonEntry( const std::string& _blazon, const std::vector<Tag>& _tags ) {
		blazon = _blazon;
		tags = _tags;

		for ( const Tag& tag : _tags )
			tagIndexMask.set( tag.index, true );
	}

	bool matchesTags( const std::vector<Tag>& _tags, bool _skipBitset = false ) const;

	std::string blazon;
	std::vector<Tag> tags;
	TagIndexBitset tagIndexMask;
};

struct BlazonDatabase
{
	std::vector<BlazonEntry> entries;
	std::vector<TagIndexBitset> tagIndices;

	void addEntry( const std::string& _blazon, const std::vector<Tag>& _tags );
	void addEntry( const std::string& _blazon, const std::string& _tags );

	std::vector<size_t> query( const std::vector<Tag>& _searchTags );
	std::vector<size_t> queryIndices( const std::vector<Tag>& _searchTags );
};

bool isModifierTincture( const std::string& _modifier );

size_t getModifierIndex( const std::string& _modifier );

Tag makeTag( const std::string& _tagName, const std::vector<std::string>& _modifierNames );
Tag makeTagFromString( const std::string& _string );
TagIndexBitset makeBitsetFromTags( const std::vector<Tag>& _tags );

std::vector<Tag> makeTagListFromString( const std::string& _string );

const bool doesTagsMatch( const Tag& _entry, const Tag& _query );

}