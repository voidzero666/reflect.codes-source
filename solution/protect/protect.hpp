#include <Windows.h>
#include "../aes256/aes256.hpp"
#include "encoding.hpp"
#include "fnv_a1.hpp"
#include <chrono>
#include <iterator>
#include <algorithm>
#include <map>

//static inline const std::vector<uint8_t> &xs64_extp_key{ 11,190,135,131,55,23,111,102,87,234,154,208,43,44,44,58,81,104,209,65,157,38,8,96,172,0,114,164,28,142,187,208,18,214,46,100,174,2,98,63,239,73,196,220,214,214,25,173,100,184,228,163,190,139,37,246,244,64,165,127,2,196,61,90,4,190,54,56,8,216,54,146,97,252,60,110,78,103,180,143,161,43,38,168,229,194,251,26,122,223,117,220,132,206,36,107,68,226,19,85,44,231,238,162,11,45,6,58,173,15,84,233,124,24,202,142,132,65,24,91,114,203,55,211,92,235,196,252,118,73,220,198,222,234,58,122,54,85,71,214,47,42,32,146,49,110,156,118,106,251,5,170,255,246,65,53,88,160,102,139,135,31,228,100,26,119,238,41,132,111,137,75,147,47,96,10,81,149,33,158,109,189,33,237,118,158,57,129,55,230,35,62,159,201,208,230,165,236,223,102,160,29,226,26,250,221,136,202,38,106,180,85,21,50,245,92,184,89,217,175,242,148,222,60,114,68,134,17,49,13,21,158,26,242,165,136,208,13,206,252,191,112,101,24,215,244,43,19,220,242,92,51,87,0,155,213,146,100,157,159,94,244,108,172,153,126,150,108,176,234,172,208,167,191,88,84,108,21,200,135,218,117,210,176,86,207,36,40,62,177,7,163,176,172,247,11,171,12,156,85,34,176,191,25,50,142,45,190,110,90,166,99,53,180,90,131,81,190,182,17,204,169,248,139,155,193,156,54,182,214,114,16,71,95,26,203,134,47,157,49,172,130,123,61,196,80,248,22,136,142,85,44,63,126,81,190,252,84,170,144,111,156,186,200,175,33,184,7,254,17,133,120,45,25,122,237,80,175,84,2,222,16,149,165,6,100,100,105,5,83,214,96,140,138,180,181,173,145,47,207,164,11,122,229,237,145,98,163,189,248,103,169,244,101,87,221,81,49,237,51,191,157,138,89,167,172,51,149,173,44,158,245,76,86,137,149,129,214,190,31,54,12,151,250,97,149,15,19,107,255,207,230,70,69,58,201,135,59,50,211,37,1,123,120,99,234,216,27,31,235,51,167,142,146,183,142,156,2,127,50,221,138,64,146,28,54,222,54,205,162,7,0,125,150,173,214,93,7,86,213,158,27,114,7,17,123,188,108,159,26,151,16,125,93,69,110,58,190,10,204,213,96,64,68,38,249,7,156,192,114,218,173,3,239,157,109,212,253,148,168,177,38,249,205,220,106,211,123,92,20,181,204,36,156,71,132,254,74,174,209,82,130,172,249,226,163,82,224,48,5,68,216,242,3,182,228,72,145,188,236,75,194,16,63,141,197,93,223,192,148,116,206,152,165,147,37,238,73,252,216,134,210,176,221,37,4,60,178,136,188,58,238,155,95,3,127,171,203,167,133,187,29,242,0,216,86,114,76,243,136,25,125,37,209,227,118,91,249,158,97,208,71,49,219,145,254,73,213,98,201,70,35,196,225,218,17,152,54,54,58,83,214,217,119,120,176,221,199,200,234,231,186,93,177,8,185,207,73,47,175,36,78,252,116,251,81,144,200,250,198,110,205,249,226,162,43,161,227,92,212,249,75,104,132,177,232,113,16,55,157,66,198,222,87,177,121,199,185,139,142,12,31,167,234,73,116,230,16,160,206,160,139,177,190,42,193,118,88,153,117,250,198,225,200,108,149,19,82,44,251,120,165,78,139,96,193,145,181,123,130,8,186,246,247,253,104,202,232,187,43,46,31,107,217,32,91,118,232,236,94,206,115,12,57,253,219,141,207,74,254,182,107,51,218,119,29,248,143,71,201,192,122,87,37,17,62,66,17,31,60,26,232,106,34,124,171,237,222,94,220,105,145,226,209,27,158,141,184,207,140,229,205,153,196,15,177,103,151,178,108,87,96,14,234,98,97,37,186,161,197,181,100,79,199,121,23,4,123,216,109,39,182,31,8,75,86,226,112,218,95,148,100,183,216,6,227,59,35,198,230,245,216,182,14,224,160,2,136,86,104,34,217,146,191,83,57,54,240,172,48,41,3,51,47,104,38,114,136,187,0,182,137,59,140,140,215,36,2,112,12,161,137,86,194,88,16,188,242,145,78,27,109,223,191,214,121,2,2,2,42,23,204,235,63,245,93,97,228,238,146,105,26,102,179,187,90,66,8,98,202,13,170,212,88,255,14,218,248,115,141,106,139,11,194,18,101,10,145,123,17,240,64,131,211,90,48,42,97,13,20,143,224,148,49,67,14,2,88,17,188,169,32,188,140,214,154,232,203,110,105,215,177,114,174,41,247,166,193,33,21,22,17,18,19 };
static uint64_t XS64_SEED = 1;
static inline const std::vector<uint8_t>& xs64_extp_key2{13,12};
static inline const std::vector<uint8_t>& xs64_extp_key{ 61,89,11,43 };

static __forceinline uint64_t xs64_from_seed(uint64_t seed)
{
	seed ^= seed << 13;
	seed ^= seed >> 7;
	seed ^= seed << 17;
	return seed;
}

static __forceinline void xs64_reset_seed()
{
	XS64_SEED = xs64_from_seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

static __forceinline uint64_t xs64()
{
	return XS64_SEED = xs64_from_seed(XS64_SEED);
}

class bit_vector
{
protected:
	std::vector<uint8_t>& data;
public:
	__forceinline bit_vector(std::vector<uint8_t>& data) : data(data)
	{
	}
	__forceinline uint8_t operator[](const size_t pos) const
	{
		return (data[pos / 8] & (1 << (pos % 8)));
	}
	__forceinline uint8_t set(const size_t pos, const bool val)
	{
		const size_t pos_b = pos / 8;
		return (data[pos_b] = (data[pos_b] ^ (*this)[pos]) | ((val ? 1 : 0) << (pos % 8)));
	}
	__forceinline size_t size() const
	{
		return data.size() * 8;
	}
};
namespace bXor {
	//extern std::vector<uint8_t> g_Key;
	static __forceinline std::vector<uint8_t> encrypt(std::vector<uint8_t> data, const std::vector<uint8_t>& key) {
		const uint64_t r_u64 = xs64();
		const uint8_t* r_u8 = reinterpret_cast<const uint8_t*>(&r_u64);
		const size_t key_size = key.size();
		size_t data_size = data.size();
		for (size_t i = 0; i < data_size; ++i)
			data[i] ^= r_u8[i % sizeof(uint64_t)];
		data.insert(data.end(), r_u8, r_u8 + sizeof(uint64_t));
		data_size = data.size();
		const size_t seed_size = (key_size < 8) ? key_size : 8;
		uint64_t seed = 0;
		for (size_t i = 0; i < seed_size; ++i)
			seed |= (key[i] << (8 * i));
		bit_vector data_b(data);
		const size_t data_b_size = data_b.size();
		for (size_t i = 0; i < data_b_size; ++i) {
			seed = xs64_from_seed(seed);
			const size_t j = (seed % data_b_size);
			if (i != j) {
				uint8_t a = data_b[i], b = data_b[j], c;
				c = a;
				data_b.set(i, b > 0);
				data_b.set(j, c > 0);
			}
		}
		for (size_t i = 0; i < data_size; ++i)
			data[i] ^= key[i % key_size];
		return data;
	}

	static __forceinline std::vector<uint8_t> decrypt(std::vector<uint8_t> data, const std::vector<uint8_t>& key) {
		const size_t key_size = key.size();
		size_t data_size = data.size();
		for (size_t i = 0; i < data_size; ++i)
			data[i] ^= key[i % key_size];
		const size_t seed_size = (key_size < 8) ? key_size : 8;
		uint64_t seed = 0;
		for (size_t i = 0; i < seed_size; ++i)
			seed |= (key[i] << (8 * i));
		bit_vector data_b(data);
		const size_t data_b_size = data_b.size();
		std::vector<size_t> jv;
		jv.resize(data_b_size);
		for (size_t i = 0; i < data_b_size; ++i) {
			seed = xs64_from_seed(seed);
			const size_t j = (seed % data_b_size);
			jv[i] = j;
		}
		size_t i = data_b_size;
		while (i > 0) {
			--i;
			const size_t j = jv[i];
			if (i != j)

			{
				uint8_t a = data_b[i], b = data_b[j], c;
				c = a;
				data_b.set(i, b > 0);
				data_b.set(j, c > 0);
			}
		}
		const uint8_t* r_u8 = &data[data_size - sizeof(uint64_t)];
		data_size -= sizeof(uint64_t);
		for (size_t i = 0; i < data_size; ++i)
			data[i] ^= r_u8[i % sizeof(uint64_t)];
		data.resize(data_size);
		return data;
	}
}

#define crypt_key 0xA5

template<std::size_t strLen>
class _hiddenString
{
protected:
	static __forceinline constexpr std::uint64_t hash(std::uint64_t x, std::uint64_t sol)
	{
		x ^= 948274649985346773LLU ^ sol;
		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;
		return x;
	}
	mutable bool m_isDecrypted;
	mutable char m_str[strLen];
	std::uint64_t m_hashingSol;
public:
	__forceinline constexpr _hiddenString(const char(&str)[strLen], std::uint64_t hashingSol) : m_isDecrypted(false), m_str{ 0 }, m_hashingSol(hashingSol)
	{
		for (std::size_t i = 0; i < strLen; ++i)
			this->m_str[i] = str[i] ^ _hiddenString<strLen>::hash(i, this->m_hashingSol);
	}
	__forceinline constexpr operator std::string() const
	{
		if (!this->m_isDecrypted)
		{
			this->m_isDecrypted = true;
			for (std::size_t i = 0; i < strLen; ++i)
				this->m_str[i] ^= _hiddenString<strLen>::hash(i, this->m_hashingSol);
		}
		return { this->m_str, this->m_str + strLen - 1 };
	}
};

template<std::size_t strLen>
class _hiddenWString
{
protected:
	static __forceinline constexpr std::uint64_t hash(std::uint64_t x, std::uint64_t sol)
	{
		x ^= 948274649985346773LLU ^ sol;
		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;
		return x;
	}
	mutable bool m_isDecrypted;
	mutable wchar_t m_str[strLen];
	std::uint64_t m_hashingSol;
public:
	__forceinline constexpr _hiddenWString(const wchar_t(&str)[strLen], std::uint64_t hashingSol) : m_isDecrypted(false), m_str{ 0 }, m_hashingSol(hashingSol)
	{
		for (std::size_t i = 0; i < strLen; ++i)
			this->m_str[i] = str[i] ^ _hiddenWString<strLen>::hash(i, this->m_hashingSol);
	}
	__forceinline constexpr operator std::wstring() const
	{
		if (!this->m_isDecrypted)
		{
			this->m_isDecrypted = true;
			for (std::size_t i = 0; i < strLen; ++i)
				this->m_str[i] ^= _hiddenWString<strLen>::hash(i, this->m_hashingSol);
		}
		return { this->m_str, this->m_str + strLen - 1 };
	}
};

#define crypt_str(s) ([]() -> std::string \
{ \
	static constexpr _hiddenString hiddenStr { s, __COUNTER__ }; \
	return hiddenStr; \
})().c_str()

#define crypt_strw(ws) ([]() -> std::wstring \
{ \
	static constexpr _hiddenWString hiddenStr { ws, __COUNTER__ }; \
	return hiddenStr; \
})().c_str()


template<std::size_t SIZE>
class hiddenString
{
public:
	short d[SIZE];
	constexpr hiddenString() :d{ 0 } { }

	__forceinline std::string s() const
	{
		std::string rv;
		rv.reserve(SIZE + 1);
		std::transform(d, d + SIZE - 1, std::back_inserter(rv), [](auto ch) {
			return ch ^ crypt_key;
		});
		int p = rv.find('\0');
		while (p != -1) {
			rv.erase(rv.begin() + p);
			p = rv.find('\0');
		}
		return rv;
	}
};

template<typename T, std::size_t N> constexpr std::size_t sizeCalculate(const T(&)[N])
{
	return N;
}

template<std::size_t SIZE>
constexpr auto encoder(const char str[SIZE])
{
	hiddenString<SIZE> encoded;
	for (std::size_t i = 0; i < SIZE/* - 1*/; i++)
		encoded.d[i] = str[i] ^ crypt_key;
	//encoded.d[SIZE - 1] = 0;
	return encoded;
}

#define _(name, x) static constexpr auto name = encoder<sizeCalculate(x)>(x)

class encrypted_string {
private:
	std::string base;
public:
	operator std::string() const {
		auto dec = bXor::decrypt(std::vector<uint8_t>(base.begin(), base.end()), xs64_extp_key);
		return std::string(dec.begin(), dec.end());
		//return base.c_str();
	}
	std::string operator()() const {
		return *this;
	}
	template<size_t size>
	void operator()(char(&str)[size]) {
		base = std::string(str, str + size);
	}
	template<size_t size>
	encrypted_string& operator=(char(&str)[size])
	{
		auto s = std::string(str);
		auto enc = bXor::encrypt(std::vector<uint8_t>(s.begin(), s.end()), xs64_extp_key);
		base = std::string(enc.begin(), enc.end());
		return *this;
	}
	encrypted_string& operator=(const char* str)
	{
	/*	base = str;
		return *this;*/
		auto s = std::string(str, str + strlen(str));
		auto enc = bXor::encrypt(std::vector<uint8_t>(s.begin(), s.end()), xs64_extp_key);
		base = std::string(enc.begin(), enc.end());
		return *this;
	}
};

typedef std::map<unsigned int, encrypted_string> crypted_map;
typedef std::map<unsigned int, unsigned int> hook_index;

extern hook_index g_HookIndices;
extern crypted_map g_Patterns;
extern crypted_map g_Modules;

namespace hs {

	/* modules */
	_(serverbrowser_dll, "serverbrowser.dll"); // serverbrowser.dll
	_(engine_dll, "engine.dll"); // engine.dll
	_(client_dll, "client.dll"); // client.dll
	_(vstdlib_dll, "vstdlib.dll"); // vstdlib.dll
	_(input_system_dll, "inputsystem.dll"); // inputsystem.dll
	_(vgui_mat_surface_dll, "vguimatsurface.dll"); // vguimatsurface.dll
	_(server_dll, "server.dll"); // server.dll
	_(vgui2_dll, "vgui2.dll"); // vgui2.dll
	_(mat_sys_dll, "materialsystem.dll"); // materialsystem.dll
	_(studio_render_dll, "studiorender.dll"); // studiorender.dll
	_(physics_dll, "vphysics.dll"); // vphysics.dll
	_(data_cache_dll, "datacache.dll"); // datacache.dll
	_(tier0_dll, "tier0.dll"); // tier0.dll
	_(gameoverlayrenderer_dll, "gameoverlayrenderer.dll"); // gameoverlayrenderer.dll
	_(filesystem_dll, "filesystem_stdio.dll");
	_(panorama_dll, "panorama.dll");

	/* cloud patterns */
	_(reset, "eset"); // reset
	_(client_state, "client_state"); // client_state
	_(move_helper, "move_helper"); // move_helper
	_(input, "input"); // input
	_(glow_manager, "glow_manager"); // glow_manage
	_(beams, "beams"); // beams
	_(update_clientside_anim, "update_clientside_anim"); // update_clientside_anim
	_(debp, "debp"); // debp
	_(standard_blending_rules, "standard_blending_rules"); // standard_blending_rules
	_(should_skip_anim_frame, "should_skip_anim_frame"); // should_skip_anim_frame
	_(get_foreign_fall_back_name, "get_foreign_fall_back_name"); // get_foreign_fall_back_name
	_(setup_bones, "setup_bones"); // setup_bones
	_(build_transformations, "build_transformations"); // build_transformations
	_(effects, "effects"); // effects

	_(reset_p, "53 57 C7 45"); // 53 57 C7 45
	_(client_state_p, "A1 ? ? ? ? 8B 80 ? ? ? ? C3"); // A1 ? ? ? ? 8B 80 ? ? ? ? C3
	_(move_helper_p, "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01"); // 8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01
	_(input_p, "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"); // B9 ? ? ? ? F3 0F 11 04 24 FF 50 10
	_(glow_manager_p, "0F 11 05 ? ? ? ? 83 C8 01"); // 0F 11 05 ? ? ? ? 83 C8 01
	_(beams_p, "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08"); // A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08
	_(update_clientside_anim_p, "55 8B EC 51 56 8B F1 80 BE ?? ?? 00 00 00 74 36 8B 06 FF 90 ?? ?? 00 00"); // 55 8B EC 51 56 8B F1 80 BE ?? ?? 00 00 00 74 36 8B 06 FF 90 ?? ?? 00 00
	_(debp_p, "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"); // 55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C
	_(standard_blending_rules_p, "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"); // 55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6
	_(should_skip_anim_frame_p, "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"); // 57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02 5F C3 -> 57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02
	_(get_foreign_fall_back_name_p, "80 3D ? ? ? ? ? 74 06 B8"); // 80 3D ? ? ? ? ? 74 06 B8
	_(setup_bones_p, "55 8B EC 83 E4 F0 B8 D8"); // 55 8B EC 83 E4 F0 B8 D8
	_(effects_p, "8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F"); // 8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F
	_(build_transformations_p, "55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 28 8B"); // 55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 1C
	_(new_shit_function_p, "55 8B EC 56 8B F1 33 C0 57 8B 7D 08"); // 55 8B EC 56 8B F1 33 C0 57 8B 7D 08
	_(seq_duration, "E8 ? ? ? ? F3 0F 11 86 ? ? ? ? 51"); // E8 ? ? ? ? F3 0F 11 86 ? ? ? ? 51
	/* misc */
	_(ecstasy, "Reflect");
	_(ecstasy_logo, "Reflect.codes");

	_(clantag1, "...........");
	_(clantag2, ".......R...");
	_(clantag3, "R..........");
	_(clantag4, "R.......efl");
	_(clantag5, "Refl.......");
	_(clantag6, "Refl....ect");
	_(clantag7, "Reflect....");
	_(clantag8, "Reflect..codes");
	_(clantag9, "Reflect.codes");
	_(clantag10, "Reflect.codes");
	_(clantag11, "flect.codes..");
	_(clantag12, "ct.codes.....");
	_(clantag13, "es.........");

	/*
	_(clantag1, "C");
	_(clantag2, "CI");
	_(clantag3, "CIA");
	_(clantag4, "CIA N");
	_(clantag5, "CIA NI");
	_(clantag6, "CIA NIG");
	_(clantag7, "CIA NIGG");
	_(clantag8, "CIA NIGGE");
	_(clantag9, "CIA NIGGER");
	_(clantag10, "CIA NIGGER");
	*/

	// netvar tables
	_(DT_BaseEntity, "DT_BaseEntity"); // DT_BaseEntity
	_(DT_BasePlayer, "DT_BasePlayer"); // DT_BasePlayer
	_(DT_CSPlayer, "DT_CSPlayer"); // DT_CSPlayer
	_(DT_LocalPlayerExclusive, "DT_LocalPlayerExclusive"); // DT_LocalPlayerExclusive
	_(DT_BaseAnimating, "DT_BaseAnimating"); // DT_BaseAnimating
	_(DT_WeaponCSBase, "DT_WeaponCSBase"); // DT_WeaponCSBase
	_(DT_WeaponCSBaseGun, "DT_WeaponCSBaseGun"); // DT_WeaponCSBaseGun
	_(DT_BaseCombatWeapon, "DT_BaseCombatWeapon"); // DT_BaseCombatWeapon
	_(DT_Ragdoll, "DT_Ragdoll"); // DT_BaseCombatWeapon
	_(DT_BaseAttributableItem, "DT_BaseAttributableItem"); // DT_BaseAttributableItem
	_(DT_BaseCSGrenade, "DT_BaseCSGrenade"); // DT_BaseCSGrenade
	_(DT_CSGameRulesProxy, "DT_CSGameRulesProxy"); // DT_CSGameRulesProxy

	// netvar names
	_(m_vecMins, "m_vecMins"); // m_vecMins
	_(m_vecMaxs, "m_vecMaxs"); // m_vecMaxs
	_(m_hObserverTarget, "m_hObserverTarget"); // m_hObserverTarget
	_(m_vecAbsVelocity, "m_vecAbsVelocity"); // m_vecAbsVelocity
	_(m_hGroundEntity, "m_hGroundEntity"); // m_hGroundEntity
	_(m_fFlags, "m_fFlags"); // m_fFlags
	_(m_flHealthShotBoostExpirationTime, "m_flHealthShotBoostExpirationTime"); // m_flHealthShotBoostExpirationTime
	_(m_vecBaseVelocity, "m_vecBaseVelocity"); // m_vecBaseVelocity
	_(m_nButtons, "m_nButtons"); // m_nButtons
	_(m_afButtonLast, "m_afButtonLast"); // m_afButtonLast
	_(m_afButtonPressed, "m_afButtonPressed"); // m_afButtonPressed
	_(m_afButtonReleased, "m_afButtonReleased"); // m_afButtonReleased
	_(nNextThinkTick, "nNextThinkTick"); // nNextThinkTick
	_(m_surfaceFriction, "m_surfaceFriction"); // m_surfaceFriction
	_(m_flPoseParameter, "m_flPoseParameter"); // m_flPoseParameter
	_(m_vecViewOffset, "m_vecViewOffset[0]"); // m_vecViewOffset[0]
	_(m_flDuckSpeed, "m_flDuckSpeed"); // m_flDuckSpeed
	_(m_flDuckAmount, "m_flDuckAmount"); // m_flDuckAmount
	_(m_iObserverMode, "m_iObserverMode"); // m_iObserverMode
	_(m_angEyeAngles, "m_angEyeAngles[0]"); // m_angEyeAngles[0]
	_(m_flVelocityModifier, "m_flVelocityModifier"); // m_flVelocityModifier
	_(m_nSequence, "m_nSequence"); // m_nSequence
	_(m_flFallVelocity, "m_flFallVelocity"); // m_flFallVelocity
	_(m_iTeamNum, "m_iTeamNum"); // m_iTeamNum
	_(m_flFlashMaxAlpha, "m_flFlashMaxAlpha"); // m_flFlashMaxAlpha
	_(m_bHasDefuser, "m_bHasDefuser"); // m_bHasDefuser
	_(m_ArmorValue, "m_ArmorValue"); // m_ArmorValue
	_(m_bReloadVisuallyComplete, "m_bReloadVisuallyComplete"); // m_bReloadVisuallyComplete
	_(m_zoomLevel, "m_zoomLevel"); // m_zoomLevel
	_(m_iClip1, "m_iClip1"); // m_iClip1
	_(m_iClip2, "m_iClip2"); // m_iClip2
	_(m_bHasHelmet, "m_bHasHelmet"); // m_bHasHelmet
	_(m_bHasHeavyArmor, "m_bHasHeavyArmor"); // m_bHasHeavyArmor
	_(m_vecAbsOrigin, "m_vecAbsOrigin"); // m_vecAbsOrigin
	_(m_flSimulationTime, "m_flSimulationTime"); // m_flSimulationTime
	_(m_iEFlags, "m_iEFlags"); // m_iEFlags
	_(m_fEffects, "m_fEffects"); // m_fEffects
	_(m_fLastShotTime, "m_fLastShotTime"); // m_fLastShotTime
	_(m_flRecoilIndex, "m_flRecoilIndex"); // m_flRecoilIndex
	_(m_fAccuracyPenalty, "m_fAccuracyPenalty"); // m_fAccuracyPenalty
	_(m_bStartedArming, "m_bStartedArming"); // m_bStartedArming
	_(m_bGunGameImmunity, "m_bGunGameImmunity"); // m_bGunGameImmunity
	_(m_weaponMode, "m_weaponMode"); // m_weaponMode
	_(m_hConstraintEntity, "m_hConstraintEntity"); // m_hConstraintEntity
	_(m_nTickBase, "m_nTickBase"); // m_nTickBase
	_(m_nImpulse, "m_nImpulse"); // m_nImpulse
	_(m_flAnimTime, "m_flAnimTime"); // m_flAnimTime
	_(m_flCycle, "m_flCycle"); // m_flCycle
	_(m_nForceBone, "m_nForceBone"); // m_nForceBone
	_(m_flLowerBodyYawTarget, "m_flLowerBodyYawTarget"); // m_flLowerBodyYawTarget
	_(m_flPostponeFireReadyTime, "m_flPostponeFireReadyTime"); // m_flPostponeFireReadyTime
	_(m_bIsScoped, "m_bIsScoped"); // m_bIsScoped
	_(m_CollisionGroup, "m_CollisionGroup"); // m_CollisionGroup
	_(m_hViewModel, "m_hViewModel[0]"); // m_hViewModel
	_(m_hActiveWeapon, "m_hActiveWeapon"); // m_hActiveWeapon
	_(m_aimPunchAngle, "m_aimPunchAngle"); // m_aimPunchAngle
	_(m_aimPunchAngleVel, "m_aimPunchAngleVel"); // m_aimPunchAngleVel
	_(m_viewPunchAngle, "m_viewPunchAngle"); // m_viewPunchAngle
	_(m_angAbsRotation, "m_angAbsRotation"); // m_angAbsRotation
	_(m_ragPos, "m_ragPos"); // m_ragPos
	_(m_bClientSideAnimation, "m_bClientSideAnimation"); // m_bClientSideAnimation
	_(m_iItemDefinitionIndex, "m_iItemDefinitionIndex"); // m_iItemDefinitionIndex
	_(m_flNextSecondaryAttack, "m_flNextSecondaryAttack"); // m_flNextSecondaryAttack
	_(m_flNextPrimaryAttack, "m_flNextPrimaryAttack"); // m_flNextPrimaryAttack
	_(m_fThrowTime, "m_fThrowTime"); // m_fThrowTime
	_(m_bFreezePeriod, "m_bFreezePeriod"); //m_bFreezePeriod
	_(m_bIsValveDS, "m_bIsValveDS"); //m_bIsValveDS
	_(m_bBombDropped, "m_bBombDropped"); //m_bBombDropped
	_(m_bBombPlanted, "m_bBombPlanted"); //m_bBombPlanted

	_(CCSPlayer, "CCSPlayer"); //CCSPlayer

	// window
	_(Valve001, "Valve001"); // Valve001


	// interface
	_(create_interface, "CreateInterface"); // CreateInterface
	_(create_interface_ex, "Can't create interface"); // Can't create interface

	// hook name
	_(impact, "Impact"); // Impact

	// world name
	_(World, "World"); // World
	_(StaticProp, "StaticProp"); // StaticProp
	_(SkyBox, "SkyBox"); // SkyBox

	// prefix
	_(prefix, "[Reflect.codes] ");

	// sky names
	_(sky_dust, "sky_dust"); // sky_dust
	_(sky_csgo_night02, "sky_csgo_night02"); // sky_csgo_night02

	// model group
	_(mdl_textures, "Model textures"); // debug/Model textures

	// model name
	_(player, "models/player"); // player
	_(arms, "arms"); // arms
	_(v, "weapons/v_"); // weapons/v_
	_(propz, "props"); // props

	// model material
	_(debugambientcube, "debug/debugambientcube"); // debug/debugambientcube
	_(debugdrawflat, "debug/debugdrawflat"); // debug/debugdrawflat
	_(regular_ref, "regular_ref"); // regular_ref
	_(glow_armsrace, "dev/glow_armsrace"); // glow_armsrace
	_(regular_glow, "regular_glow"); // regular_glow

	// material settings name
	_(phongexponent, "$phongexponent"); // phongexponent
	_(phongboost, "$phongboost"); // $phongboost
	_(rimlightexponent, "$rimlightexponent"); // $rimlightexponent
	_(rimlightboost, "$rimlightboost"); // $rimlightboost
	_(pearlescent, "$pearlescent"); // $pearlescent
	_(envmaptint, "$envmaptint"); // $envmaptint
	_(phongtint, "$phongtint"); // $phongtint

	// hitboxes
	_(gear, "gear"); // gear
	_(h_generic, "generic"); // generic
	_(question, "?"); // ?
	_(head, "head"); // head
	_(neck, "neck"); // neck
	_(pelvis, "pelvis"); // pelvis
	_(stomach, "stomach"); // stomach
	_(lower_chest, "lower chest"); // lower chest
	_(chest, "chest"); // chest
	_(upper_chest, "upper chest"); // upper chest
	_(right_thigh, "right thigh"); // right thigh
	_(left_thigh, "left thigh"); // left thigh
	_(right_leg, "right leg"); // right leg
	_(left_leg, "left leg"); // left leg
	_(right_foot, "right foot"); // right foot
	_(left_foot, "left foot"); // left foot
	_(right_hand, "right hand"); // right hand
	_(left_hand, "left hand"); // left hand
	_(right_arm, "right arm"); // right arm
	_(left_arm, "left arm"); // left arm
	_(right_upper_arm, "right upper arm"); // right upper arm
	_(right_lower_arm, "right lower arm"); // right lower arm
	_(left_upper_arm, "left upper arm"); // left upper arm
	_(left_lower_arm, "left lower arm"); // left lower arm

	// eventlog shit
	_(H_, " [H:"); // H:
	_(R_, " [R:"); // H:
	_(B_, " [BT:"); // H:
	_(SHOT, " [SHOT] "); // H:
	_(prefix_end, "] "); // ]
	_(cant_pr_shot, "Shot rejected"); // Can't proceed shot
	_(cant_pr_impact, "ServeShot rejected."); // Can't proceed impact
	_(miss, "Missed shot to "); // Missed shot to
	_(spread_miss, "Missed shot due to spread"); // Missed shot due to spread
	_(spread, " due to spread "); // SPREAD
	_(spspread, "  due to spread "); // SPREAD
	_(sp, " [SAFE] "); // SPREAD

	// resolver mode
	_(AB, "AB"); // AB
	_(AC, "AC"); // AC
	_(B, "B"); // B
	_(C, "C"); // C
	_(A, "A"); // A
	_(DA, "DA"); // DA
	_(DB, "DB"); // DB

	/* cvar names */
	_(cl_interp, "cl_interp"); // cl_interp
	_(sv_maxunlag, "sv_maxunlag"); // sv_maxunlag
	_(cl_interp_ratio, "cl_interp_ratio"); // cl_interp_ratio
	_(sv_client_min_interp_ratio, "sv_client_min_interp_ratio"); // sv_client_min_interp_ratio
	_(sv_client_max_interp_ratio, "sv_client_max_interp_ratio"); // sv_client_max_interp_ratio
	_(sv_minupdaterate, "sv_minupdaterate"); // sv_minupdaterate
	_(sv_maxupdaterate, "sv_maxupdaterate"); // sv_maxupdaterate
	_(cl_updaterate, "cl_updaterate"); // cl_updaterate
	_(sv_gravity, "sv_gravity"); // sv_gravity
	_(sv_jump_impulse, "sv_jump_impulse"); // sv_jump_impulse
	_(mp_damage_scale_ct_head, "mp_damage_scale_ct_head"); // mp_damage_scale_ct_head
	_(mp_damage_scale_t_head, "mp_damage_scale_t_head"); // mp_damage_scale_t_head
	_(mp_damage_scale_ct_body, "mp_damage_scale_ct_body"); // mp_damage_scale_ct_body
	_(mp_damage_scale_t_body, "mp_damage_scale_t_body"); // mp_damage_scale_t_body
	_(cl_sidespeed, "cl_sidespeed"); // cl_sidespeed
	_(cl_forwardspeed, "cl_forwardspeed"); // cl_forwardspeed
	_(r_DrawSpecificStaticProp, "r_DrawSpecificStaticProp"); // cl_sidespeed
	_(cl_foot_contact_shadows, "cl_foot_contact_shadows"); // cl_foot_contact_shadows
	_(viewmodel_offset_x, "viewmodel_offset_x"); // viewmodel_offset_x
	_(viewmodel_offset_y, "viewmodel_offset_y"); // viewmodel_offset_y
	_(viewmodel_offset_z, "viewmodel_offset_z"); // viewmodel_offset_z
	_(r_aspectratio, "r_aspectratio"); // r_aspectratio
	_(mat_postprocess_enable, "mat_postprocess_enable"); // mat_postprocess_enable
	_(weapon_debug_spread_show, "weapon_debug_spread_show"); // weapon_debug_spread_show
	_(sv_friction, "sv_friction"); // sv_friction
	_(sv_stopspeed, "sv_stopspeed"); // sv_stopspeed
	_(sv_skyname, "sv_skyname"); // sv_skyname
	_(r_3dsky, "r_3dsky"); // r_3dsky
	_(sv_cheats, "sv_cheats"); // sv_cheats
	_(camera, "cl_camera_height_restriction_debug"); // cl_camera_height_restriction_debug
	_(shadows, "cl_csm_shadows"); // cl_csm_shadows
	_(ff_damage_reduction_bullets, "ff_damage_reduction_bullets"); // ff_damage_reduction_bullets
	_(ff_damage_bullet_penetration, "ff_damage_bullet_penetration"); // ff_damage_bullet_penetration

	/* cmd commands */
	_(clear, "clear"); // clear
	_(unload_message, "echo Hake unloaded!"); // echo Cheat has been unloaded!

	//_(unload_message, "echo Hake unloaded!"); // echo Cheat has been unloaded!
	_(unload_message1, "echo [Reflect.codes] Succesfully injected!"); // echo Cheat has been unloaded!

	/* interface names */
	_(client, "VClient018"); // VClient018
	_(ent_list, "VClientEntityList003"); // VClientEntityList003
	_(leaf_sys, "ClientLeafSystem002"); // ClientLeafSystem002
	_(engine_model, "VEngineModel016"); // VEngineModel016
	_(model_info, "VModelInfoClient004"); // VModelInfoClient004
	_(cvar, "VEngineCvar007"); // VEngineCvar007
	_(engine_client, "VEngineClient014"); // VEngineClient014
	_(engine_trace, "EngineTraceClient004"); // EngineTraceClient004
	_(input_sys, "InputSystemVersion001"); // InputSystemVersion001
	_(vgui_surface, "VGUI_Surface031"); // VGUI_Surface031
	_(server_game_dll, "ServerGameDLL0"); // ServerGameDLL0
	_(vgui_panel, "VGUI_Panel009"); // VGUI_Panel009
	_(mat_sys, "VMaterialSystem080"); // VMaterialSystem080
	_(render_view, "VEngineRenderView014"); // VEngineRenderView014
	_(debug_overlay, "VDebugOverlay004"); // VDebugOverlay004
	_(events_manager, "GAMEEVENTSMANAGER002"); // GAMEEVENTSMANAGER002
	_(studio_render, "VStudioRender026"); // VStudioRender026
	_(phys_props, "VPhysicsSurfaceProps001"); // VPhysicsSurfaceProps001
	_(game_movement, "GameMovement001"); // GameMovement001
	_(engine_vgui, "VEngineVGui001"); // VEngineVGui001
	_(engine_sound, "IEngineSoundClient003"); // IEngineSoundClient003
	_(mdl_cache, "MDLCache004"); // MDLCache004
	_(client_prediction, "VClientPrediction001"); // VClientPrediction001
	_(mem_alloc, "g_pMemAlloc"); // g_pMemAlloc
	_(file_sys, "VFileSystem017"); // VFileSystem017
	_(panorama_ui_engine, "PanoramaUIEngine001");

	/*patterns, netvars & netvar tables*/
	_(shoot_pos, "55 8B EC 56 8B 75 08 57 8B F9 56 8B 07 FF 90 ? ? ? ?"); // 55 8B EC 56 8B 75 08 57 8B F9 56 8B 07 FF 90 ? ? ? ?
	_(bone_cache, "FF B7 ?? ?? ?? ?? 52"); // FF B7 ?? ?? ?? ?? 52
	_(visibility, "E8 ? ? ? ? 83 7D D8 00 7C 0F"); // E8 ? ? ? ? 83 7D D8 00 7C 0F
	_(server_hitbox, "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE"); // 55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE
	_(server_edict, "8B 15 ? ? ? ? 33 C9 83 7A 18 01"); // 8B 15 ? ? ? ? 33 C9 83 7A 18 01
	_(reset_animstate, "55 8B EC 83 EC 08 56 8B F1 57 6A 0D"); // "55 8B EC 83 EC 08 56 8B F1 57 6A 0D"
	_(create_animstate, "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"); // 55 8B EC 56 8B F1 B9 ? ? ? ? C7 46
	_(update_animstate, "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3"); // 55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3
	_(post_think, "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9"); // 55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9
	_(simulate_player, "56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 72"); // 56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 72
	_(physics_run_think, "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87"); // 55 8B EC 83 EC 10 53 56 57 8B F9 8B 87
	_(set_abs_angles, "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"); // 55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8
	_(set_abs_origin, "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"); // 55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8
	_(unk_func, "55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B C6"); // 55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B C6
	_(select_item, "55 8B EC 56 8B F1 ? ? ? 85 C9 74 71 8B 06"); // 55 8B EC 56 8B F1 ? ? ? 85 C9 74 71 8B 06
	_(seq_activity, "55 8B EC 53 8B 5D 08 56 8B F1 83"); // 55 8B EC 53 8B 5D 08 56 8B F1 83
	_(bone_cache_validation, "C6 05 ? ? ? ? ? 89 47 70"); // C6 05 ? ? ? ? ? 89 47 70
	_(physics_recursive, "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56"); // 55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56
	_(trace_filter, "55 8B EC 83 E4 F0 83 EC 7C 56 52"); // 55 8B EC 83 E4 F0 83 EC 7C 56 52
	_(is_breakable, "55 8B EC 51 56 8B F1 85 F6 74 68"); // 55 8B EC 51 56 8B F1 85 F6 74 68
	//_(bone_cache_validation, "C6 05 ? ? ? ? ? 89 47 70"); // C6 05 ? ? ? ? ? 89 47 70
	_(load_named_sky, "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"); // C6 05 ? ? ? ? ? 89 47 70
	_(set_clantag, "53 56 57 8B DA 8B F9 FF 15"); // 53 56 57 8B DA 8B F9 FF 15
	_(get_seq_linear_motion, "E8 ? ? ? ? F3 0F 10 4D ? 83 C4 08 F3 0F 10 45 ? F3 0F 59 C0"); // E8 ? ? ? ? F3 0F 10 4D ? 83 C4 08 F3 0F 10 45 ? F3 0F 59 C0
	_(seq_transition, "84 C0 74 17 8B 87"); // 84 C0 74 17 8B 87
	_(setup_bones_timing, "84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05"); //84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05
	_(choke_limit, "B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC"); // B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC
	_(write_user_cmd, "55 8B EC 83 E4 F8 51 53 56 8B D9"); // 55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D
	_(write_user_cmd_retn, "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84 ? ? ? ?"); // 84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84 ? ? ? ?
	_(modify_eye_pos, "85 C0 0F 84 ? ? ? ? 8B 45 08 8B 0D"); // 85 C0 0F 84 ? ? ? ? 8B 45 08 8B 0D
	_(ret_engine, "\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C"); // \x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C
	_(setup_vel, "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80"); // 84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80
	_(accum_layers, "84 C0 75 0D F6 87"); // 84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80
	_(retn_camera, "85 C0 75 30 38 86"); // 85 C0 75 30 38 86
	_(game_rules, "A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6"); // A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6
	_(view_matrix, "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9"); // 0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9
	_(eye_pos, "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14 83 7F 60");
	_(smoke_effect, "A3 ? ? ? ? 57 8B CB"); // A3 ? ? ? ? 57 8B CB
	_(physicssimulate, "56 8B F1 8B ? ? ? ? ? 83 F9 FF 74 23");
	_(pred_seed, "A3 ? ? ? ? 66 0F 6E 86");
	_(pred_player, "89 35 ? ? ? ? F3 0F 10 48");
	_(invalidate_bone_cache, "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81"); // 80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81
	_(get_clr_modulation, "55 8B EC 83 EC ? 56 8B F1 8A 46"); // 55 8B EC 83 EC ? 56 8B F1 8A 46
	_(do_anim_event, "8B 89 ? ? 00 00 6A 00 6A 08 8B 01 FF 50 ?");
	_(using_static_prop, "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D"); // 8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D
	_(hud, "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08");
	_(chicken_eventkilled, "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 56 57 51 68 ? ? ? ?");
	// misc
	_(_s_n, "%s\n");

	/* hook indices */
	_(Hooked_ProcessMovement, "Hooked_ProcessMovement");
	_(Hooked_ProcessPacket, "Hooked_ProcessPacket");
	_(Hooked_SendDatagram, "Hooked_SendDatagram");
	_(Hooked_FrameStageNotify, "Hooked_FrameStageNotify");
	_(Hooked_GetColorModulation, "Hooked_GetColorModulation");
	_(Hooked_WriteUsercmdDeltaToBuffer, "Hooked_WriteUsercmdDeltaToBuffer");
	_(Hooked_IsPaused, "Hooked_IsPaused");
	_(Hooked_IsBoxVisible, "Hooked_IsBoxVisible");
	_(Hooked_IsHLTV, "Hooked_IsHLTV");
	_(Hooked_ClipRayColliedable, "Hooked_ClipRayColliedable");
	_(Hooked_GetScreenAspectRatio, "Hooked_GetScreenAspectRatio");
	_(Hooked_ShouldDrawShadow, "Hooked_ShouldDrawShadow");
	_(Hooked_CreateMove, "Hooked_CreateMove");
	_(Hooked_ShouldDrawFog, "Hooked_ShouldDrawFog");
	_(Hooked_OverrideView, "Hooked_OverrideView");
	_(Hooked_ViewModel, "Hooked_ViewModel");
	_(Hooked_DoPostScreenEffects, "Hooked_DoPostScreenEffects");
	_(Hooked_PaintTraverse, "Hooked_PaintTraverse");
	_(Hooked_BeginFrame, "Hooked_BeginFrame");
	_(Hooked_PacketStart, "Hooked_PacketStart");
	_(Hooked_PacketEnd, "Hooked_PacketEnd");
	_(Hooked_DrawModelExecute, "Hooked_DrawModelExecute");
	_(Hooked_RunCommand, "Hooked_RunCommand");
	_(Hooked_InPrediction, "Hooked_InPrediction");
	_(Hooked_ListLeavesInBox, "Hooked_ListLeavesInBox");
	_(Hooked_GetBool, "Hooked_GetBool");
	_(Hooked_SceneEnd, "Hooked_SceneEnd");
	_(Hooked_EmitSound, "Hooked_EmitSound");
	_(Hooked_OverrideMouseInput, "Hooked_OverrideMouseInput");
	_(Hooked_GetUnverifiedFileHashes, "Hooked_GetUnverifiedFileHashes");
	_(Hooked_CanLoadThirdPartyFiles, "Hooked_CanLoadThirdPartyFiles");
	_(Hooked_OnScreenSizeChanged, "Hooked_OnScreenSizeChanged");
	_(Hooked_DispatchUserMessage, "Hooked_DispatchUserMessage");
	_(Hooked_FindIndex, "Hooked_FindIndex");
}