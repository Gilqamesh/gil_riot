#include "riot.h"

#include <iostream>
#include <cassert>

#include "web.h"

static const std::string region_to_regional_route_host_name[riot_api::_REGION_SIZE] = {
    "americas.api.riotgames.com",
    "americas.api.riotgames.com",
    "europe.api.riotgames.com",
    "europe.api.riotgames.com",
    "asia.api.riotgames.com"
    "asia.api.riotgames.com"
};

static const std::string region_to_platform_route_host_name[riot_api::_REGION_SIZE] = {
    "na1.api.riotgames.com",
    "la1.api.riotgames.com",
    "euw1.api.riotgames.com",
    "eun1.api.riotgames.com",
    "oc1.api.riotgames.com",
    "kr.api.riotgames.com"
};

// https://static.developer.riotgames.com/docs/lol/gameModes.json
static const std::string game_type_to_game_type_str[riot_api::_GAME_TYPE_SIZE] = {
    "ranked",
    "tutorial",
    "normal",
    "tourney"
};

// https://static.developer.riotgames.com/docs/lol/queues.json
static const std::string game_type_to_queue_str[riot_api::_GAME_TYPE_SIZE] = {
    "420",
    "2000", // 2010, 2020 also works, need testing
    "430", // 490 for quickplay
    "420" // unsure if this is true, need testing
};

riot_api::riot_api(const std::string& api_key):
    m_api_key(api_key) {
}

riot_api::region_t riot_api::str_to_region(const std::string& region) {
    if (region == "NA")   return riot_api::REGION_NA;
    if (region == "LAN")  return riot_api::REGION_LAN;
    if (region == "EUW")  return riot_api::REGION_EUW;
    if (region == "EUNE") return riot_api::REGION_EUNE;
    if (region == "OCE")  return riot_api::REGION_OCE;
    if (region == "KR")   return riot_api::REGION_KR;

    assert(0);

    return riot_api::_REGION_SIZE;
}

std::string riot_api::region_to_str(region_t region) {
    switch (region) {
    case riot_api::REGION_NA: return "NA";
    case riot_api::REGION_LAN: return "LAN";
    case riot_api::REGION_EUW: return "EUW";
    case riot_api::REGION_EUNE: return "EUNE";
    case riot_api::REGION_OCE: return "OCE";
    case riot_api::REGION_KR: return "KR";
    
    default: assert(0);
    }
}

void riot_api::get_puuid_async(
    const std::string& game_name, const std::string& tag_line,
    std::function<void(const std::string& resulting_puuid)> on_success,
    std::function<void()> on_failure
) {
    const std::string api_end_point = "riot/account/v1/accounts/by-riot-id/";
    const std::string path_name = api_end_point + game_name + "/" + tag_line + "?api_key=" + m_api_key;
    get_async(
        "https", region_to_regional_route_host_name[riot_api::REGION_EUW /* region does not matter, but it is recommended to be closest to client */], 443, path_name,
        [on_success, on_failure](const unsigned char* serialized_data, size_t serialized_data_size) {
            try {
                nlohmann::json json = nlohmann::json::parse(serialized_data, serialized_data + serialized_data_size);
                nlohmann::json result = json["puuid"];
                if (!result.is_primitive() && !result.is_string()) {
                    std::cerr << "RIOT_API expected string result from puuid request, but got something else" << std::endl;
                    on_failure();
                } else {
                    on_success(result.get<std::string>());
                }
            } catch (std::exception& e) {
                std::cerr << "RIOT_API failed to parse request: '" << e.what() << "'" << std::endl;
                on_failure();
            }
        },
        on_failure
    );
}

void riot_api::get_match_history_async(
    region_t region, game_type_t game_type, const std::string& puuid,
    std::function<void(const nlohmann::json& resulting_match_history)> on_success,
    std::function<void()> on_failure
) {
    const std::string api_end_point = "lol/match/v5/matches/by-puuid/" + puuid + "/ids";
    const std::string path_name = api_end_point + "?api_key=" + m_api_key + "&queue=" + game_type_to_queue_str[game_type] + "&type=" + game_type_to_game_type_str[game_type] + "&start=0&count=100";

    get_async(
        "https", region_to_regional_route_host_name[region], 443, path_name,
        [on_success](const unsigned char* serialized_data, size_t serialized_data_size) {
            try {
                nlohmann::json json = nlohmann::json::parse(serialized_data, serialized_data + serialized_data_size);
                on_success(json);
            } catch (std::exception& e) {
                std::cerr << "RIOT_API failed to parse request: '" << e.what() << "'" << std::endl;
            }
        },
        on_failure
    );
}

void riot_api::get_match_info_async(
    region_t region, const std::string& match_id,
    std::function<void(const nlohmann::json& resulting_match_info)> on_success,
    std::function<void()> on_failure
) {
    const std::string api_end_point = "lol/match/v5/matches/" + match_id;
    const std::string path_name = api_end_point + "?api_key=" + m_api_key;

    get_async(
        "https", region_to_regional_route_host_name[region], 443, path_name,
        [on_success, on_failure](const unsigned char* serialized_data, size_t serialized_data_size) {
            try {
                nlohmann::json json = nlohmann::json::parse(serialized_data, serialized_data + serialized_data_size);
                on_success(json);
            } catch (std::exception& e) {
                std::cerr << "RIOT_API failed to parse request: '" << e.what() << "'" << std::endl;
                on_failure();
            }
        },
        on_failure
    );
}

void riot_api::get_match_timeline_async(
    region_t region, const std::string& match_id,
    std::function<void(const nlohmann::json& resulting_match_timeline)> on_success,
    std::function<void()> on_failure
) {
    const std::string api_end_point = "lol/match/v5/matches/" + match_id + "/timeline";
    const std::string path_name = api_end_point + "?api_key=" + m_api_key;

    get_async(
        "https", region_to_regional_route_host_name[region], 443, path_name,
        [on_success, on_failure](const unsigned char* serialized_data, size_t serialized_data_size) {
            try {
                nlohmann::json json = nlohmann::json::parse(serialized_data, serialized_data + serialized_data_size);
                on_success(json);
            } catch (std::exception& e) {
                std::cerr << "RIOT_API failed to parse request: '" << e.what() << "'" << std::endl;
                on_failure();
            }
        },
        on_failure
    );
}

void riot_api::get_challenges_info_async(
    region_t region,
    std::function<void(const nlohmann::json& resulting_challenges_info)> on_success,
    std::function<void()> on_failure
) {
    const std::string api_end_point = "lol/challenges/v1/challenges/config";
    const std::string path_name = api_end_point + "?api_key=" + m_api_key;

    get_async(
        "https", region_to_platform_route_host_name[region], 443, path_name,
        [on_success, on_failure](const unsigned char* serialized_data, size_t serialized_data_size) {
            try {
                nlohmann::json json = nlohmann::json::parse(serialized_data, serialized_data + serialized_data_size);
                on_success(json);
            } catch (std::exception& e) {
                std::cerr << "RIOT_API failed to parse request: '" << e.what() << "'" << std::endl;
                on_failure();
            }
        },
        on_failure
    );
}

void riot_api::get_challenges_by_puuid_async(
    region_t region, const std::string& puuid,
    std::function<void(const nlohmann::json& resulting_challenges_info_for_puuid)> on_success,
    std::function<void()> on_failure
) {
    const std::string api_end_point = "lol/challenges/v1/player-data/" + puuid;
    const std::string path_name = api_end_point + "?api_key=" + m_api_key;

    get_async(
        "https", region_to_platform_route_host_name[region], 443, path_name,
        [on_success, on_failure](const unsigned char* serialized_data, size_t serialized_data_size) {
            try {
                nlohmann::json json = nlohmann::json::parse(serialized_data, serialized_data + serialized_data_size);
                on_success(json);
            } catch (std::exception& e) {
                std::cerr << "RIOT_API failed to parse request: '" << e.what() << "'" << std::endl;
                on_failure();
            }
        },
        on_failure
    );
}

void get_liveclientdata_allgamedata_async(
    std::function<void(const nlohmann::json& resulting_live_client_data)> on_success,
    std::function<void()> on_failure
) {
    get_async(
        "https", "127.0.0.1", 443, "liveclientdata/allgamedata",
        [on_success, on_failure](const unsigned char* serialized_data, size_t serialized_data_size) {
            try {
                nlohmann::json json = nlohmann::json::parse(serialized_data, serialized_data + serialized_data_size);
                on_success(json);
            } catch (std::exception& e) {
                std::cerr << "RIOT_API failed to parse request: '" << e.what() << "'" << std::endl;
                on_failure();
            }
        },
        [on_failure]() {
            std::cerr << "RIOT_API failed get live client data" << std::endl;
            on_failure();
        }
    );
}

void get_liveclientdata_activeplayer_async(
    std::function<void(const nlohmann::json& resulting_live_client_data)> on_success,
    std::function<void()> on_failure
) {
    get_async(
        "https", "127.0.0.1", 443, "liveclientdata/activeplayer",
        [on_success, on_failure](const unsigned char* serialized_data, size_t serialized_data_size) {
            try {
                nlohmann::json json = nlohmann::json::parse(serialized_data, serialized_data + serialized_data_size);
                on_success(json);
            } catch (std::exception& e) {
                std::cerr << "RIOT_API failed to parse request: '" << e.what() << "'" << std::endl;
                on_failure();
            }
        },
        [on_failure]() {
            std::cerr << "RIOT_API failed get live client data" << std::endl;
            on_failure();
        }
    );
}
