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

struct EntryIndex
{
	EntryIndex() = default;
	EntryIndex( const TagBitset& _bitset, const std::string& _entry )
	{
		tagbits = _bitset.to_ullong();
		for ( size_t i = 0; i < _entry.size(); i++ )
			entry[ i ] = _entry[ i ];
	}

	TagBitset getBitset() const {
		return TagBitset( tagbits );
	}

	std::string getEntryName() const {
		std::string s;
		s.resize( 8 );
		for ( size_t i = 0; i < 8; i++ )
			s[ i ] = entry[ i ];
		return s;
	}

	unsigned long long tagbits;
	char entry[ 8 ] = { 0 };
};

struct BlazonEntry
{
	BlazonEntry() = default;
	BlazonEntry( const std::string& _blazon, const std::string& _tags, const std::string& _description = "" );
	BlazonEntry( const std::string& _blazon, const std::vector<Tag>& _tags, const std::string& _description = "" );

	void generateEntryID()
	{
		constexpr char chars[] = {
			'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
			'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
		};

		entryID.resize( 8, ' ' );
		for ( size_t i = 0; i < entryID.size(); i++ )
			entryID[ i ] = chars[ std::rand() % 52 ];
	}

	bool matchesTags( const std::vector<Tag>& _tags, bool _skipBitset = false ) const;

	std::string entryID;
	std::string blazon;
	std::string description;
	std::vector<Tag> tags;
	TagBitset tagBitset;
	std::string tagsHash;
};

struct BlazonDatabase
{
	BlazonDatabase();

	std::vector<BlazonEntry> entries;
	std::vector<EntryIndex> entryIndices;

	size_t createEntry( const BlazonEntry& _entry ) {
		entryIndices.push_back( { _entry.tagBitset, _entry.entryID } );
		entries.push_back( _entry );
		return entries.size() - 1;
	}

	size_t loadEntry( const std::string& _entryID );
	BlazonEntry getEntry( const std::string& _entryID );
	void cacheEntry( const BlazonEntry& _entry );

	std::vector<std::string> query( const std::vector<Tag>& _searchTags );
	std::vector<std::string> queryIndices( const std::vector<Tag>& _searchTags );

	void generateIndexTable();
	void loadIndexTable();
};

bool isModifierTincture( const std::string& _modifier );

size_t getModifierIndex( const std::string& _modifier );

Tag makeTag( const std::string& _tagName, const std::vector<std::string>& _modifierNames );
Tag makeTagFromString( const std::string& _string );
TagBitset makeBitsetFromTags( const std::vector<Tag>& _tags );

std::vector<Tag> makeTagListFromString( const std::string& _string );

const bool doesTagsMatch( const Tag& _entry, const Tag& _query );

std::filesystem::path getEntryPath( const std::string& _entryID );
std::filesystem::path getEntryIndexPath();

void to_json( nlohmann::json& _json, const blazon::Tag& _p );
void from_json( const nlohmann::json& _json, blazon::Tag& _p );

void to_json( nlohmann::json& _json, const blazon::BlazonEntry& _p );
void from_json( const nlohmann::json& _json, blazon::BlazonEntry& _p );


}