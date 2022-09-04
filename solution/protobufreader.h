#pragma once
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

enum class UserMessageType {
	CS_UM_VGUIMenu = 1,
	CS_UM_Geiger = 2,
	CS_UM_Train = 3,
	CS_UM_HudText = 4,
	CS_UM_SayText = 5,
	CS_UM_SayText2 = 6,
	CS_UM_TextMsg = 7,
	CS_UM_HudMsg = 8,
	CS_UM_ResetHud = 9,
	CS_UM_GameTitle = 10,
	CS_UM_Shake = 12,
	CS_UM_Fade = 13,
	CS_UM_Rumble = 14,
	CS_UM_CloseCaption = 15,
	CS_UM_CloseCaptionDirect = 16,
	CS_UM_SendAudio = 17,
	CS_UM_RawAudio = 18,
	CS_UM_VoiceMask = 19,
	CS_UM_RequestState = 20,
	CS_UM_Damage = 21,
	CS_UM_RadioText = 22,
	CS_UM_HintText = 23,
	CS_UM_KeyHintText = 24,
	CS_UM_ProcessSpottedEntityUpdate = 25,
	CS_UM_ReloadEffect = 26,
	CS_UM_AdjustMoney = 27,
	CS_UM_UpdateTeamMoney = 28,
	CS_UM_StopSpectatorMode = 29,
	CS_UM_KillCam = 30,
	CS_UM_DesiredTimescale = 31,
	CS_UM_CurrentTimescale = 32,
	CS_UM_AchievementEvent = 33,
	CS_UM_MatchEndConditions = 34,
	CS_UM_DisconnectToLobby = 35,
	CS_UM_PlayerStatsUpdate = 36,
	CS_UM_DisplayInventory = 37,
	CS_UM_WarmupHasEnded = 38,
	CS_UM_ClientInfo = 39,
	CS_UM_XRankGet = 40,
	CS_UM_XRankUpd = 41,
	CS_UM_CallVoteFailed = 45,
	CS_UM_VoteStart = 46,
	CS_UM_VotePass = 47,
	CS_UM_VoteFailed = 48,
	CS_UM_VoteSetup = 49,
	CS_UM_ServerRankRevealAll = 50,
	CS_UM_SendLastKillerDamageToClient = 51,
	CS_UM_ServerRankUpdate = 52,
	CS_UM_ItemPickup = 53,
	CS_UM_ShowMenu = 54,
	CS_UM_BarTime = 55,
	CS_UM_AmmoDenied = 56,
	CS_UM_MarkAchievement = 57,
	CS_UM_MatchStatsUpdate = 58,
	CS_UM_ItemDrop = 59,
	CS_UM_GlowPropTurnOff = 60,
	CS_UM_SendPlayerItemDrops = 61,
	CS_UM_RoundBackupFilenames = 62,
	CS_UM_SendPlayerItemFound = 63,
	CS_UM_ReportHit = 64,
	CS_UM_XpUpdate = 65,
	CS_UM_QuestProgress = 66,
	CS_UM_ScoreLeaderboardData = 67,
	CS_UM_PlayerDecalDigitalSignature = 68,
	MAX_ECSTRIKE15USERMESSAGES
};

class ProtobufReader {
public:
    ProtobufReader(const std::uint8_t* data, int size) : data{ data }, size{ size } {}

    [[nodiscard]] std::int32_t readInt32(std::uint8_t index) const noexcept
    {
        int offset = 0;
        while (offset + 1 < size) {
            if (const auto key = readVarint(offset); wireType(key) == WireType::Varint && (key >> 3) == index)
                return readVarint(offset + 1);
            ++offset;
        }
        return -1;
    }

    [[nodiscard]] std::vector<std::string> readRepeatedString(std::uint8_t index) const noexcept
    {
        std::vector<std::string> strings;
        int offset = 0;
        while (offset + 1 < size) {
            if (const auto key = readVarint(offset); wireType(key) == WireType::LengthDelimited && (key >> 3) == index) {
                offset += strings.emplace_back(readString(offset + 1)).length();
            }
            ++offset;
        }
        return strings;
    }

private:
    [[nodiscard]] std::int8_t readVarint(int offset) const noexcept
    {
        assert(offset >= 0 && offset < size);
        //assert((data[offset] & 0x80) == 0);
        return data[offset] & 0x7F;
    }

    [[nodiscard]] std::string readString(int offset) const noexcept
    {
        assert(offset >= 0 && offset < size);
        const auto length = readVarint(offset);
        return (length > 0 && offset + 1 + length < size) ? std::string{ reinterpret_cast<const char*>(data + offset + 1), static_cast<std::size_t>(length) } : std::string{};
    }

    enum class WireType {
        Varint = 0,
        LengthDelimited = 2
    };

    [[nodiscard]] constexpr WireType wireType(std::int8_t key) const noexcept
    {
        return static_cast<WireType>(key & 3);
    }

    const std::uint8_t* data;
    int size;
};