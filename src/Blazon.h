#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <bitset>

#include <nlohmann/json.hpp>

namespace blazon {

typedef std::bitset<128> TagBitset;
typedef std::bitset<128> ModifierBitset;

const std::vector<std::string> CONST_TAGS{
	"field",
	"per_fess",
	"lion",
	"bends",
	"per_fess",
	"crown"
};

const std::vector<std::string> CONST_MODIFIERS{
	"rampant", "passant", "sejant", "couchant",
	"dexter", "sinister",
	"wavy",
	"two", "three", "four", 
	"2_over_1"
};

const std::vector<std::string> CONST_TINCTURES{
	"azure", "gules", "vert", "sable",
	"argent", "or",
	"ermine", "vair",
	"proper"
};

struct Tag
{
	size_t index{ 0 };
	ModifierBitset modifiersBitset{ 0 };
	std::vector<uint32_t> tinctureHashes;
};

struct BlazonEntry
{
	BlazonEntry() = default;
	BlazonEntry( const std::string& _blazon, const std::vector<Tag>& _tags, const std::string& _description = "" ) {
		blazon = _blazon;
		tags = _tags;
		description = _description;

		for ( const Tag& tag : _tags )
			tagBitset.set( tag.index, true );

		std::stringstream ss;
		ss << std::hex << tagBitset.to_ullong();
		tagsHash = ss.str();
	}

	bool matchesTags( const std::vector<Tag>& _tags, bool _skipBitset = false ) const;

	std::string blazon;
	std::string description;
	std::vector<Tag> tags;
	TagBitset tagBitset;
	std::string tagsHash;
};

struct BlazonDatabase
{
	std::vector<BlazonEntry> entries;
	std::vector<TagBitset> tagIndices;

	void addEntry( const std::string& _blazon, const std::vector<Tag>& _tags, const std::string& _description = "" );
	void addEntry( const std::string& _blazon, const std::string& _tags, const std::string& _description = "" );

	std::vector<size_t> query( const std::vector<Tag>& _searchTags );
	std::vector<size_t> queryIndices( const std::vector<Tag>& _searchTags );

	std::vector<unsigned long long> generateIndexTable();
};

bool isModifierTincture( const std::string& _modifier );

size_t getModifierIndex( const std::string& _modifier );

Tag makeTag( const std::string& _tagName, const std::vector<std::string>& _modifierNames );
Tag makeTagFromString( const std::string& _string );
TagBitset makeBitsetFromTags( const std::vector<Tag>& _tags );

std::vector<Tag> makeTagListFromString( const std::string& _string );

const bool doesTagsMatch( const Tag& _entry, const Tag& _query );


void to_json( nlohmann::json& _json, const blazon::Tag& _p );
void from_json( const nlohmann::json& _json, blazon::Tag& _p );

void to_json( nlohmann::json& _json, const blazon::BlazonEntry& _p );
void from_json( const nlohmann::json& _json, blazon::BlazonEntry& _p );


}