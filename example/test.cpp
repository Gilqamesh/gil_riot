#include "riot.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: <app> <riot_api_key>" << std::endl;
        return 1;
    }

    riot_api riot(argv[1]);

    riot.get_puuid_async(
        "Im Gilgamesh", "EUW",
        [&riot](const std::string& resulting_puuid) {
            std::cout << "CLIENT successfully got puuid: '" << resulting_puuid << "'" << std::endl;
            riot.get_match_history_async(
                riot_api::REGION_EUW, riot_api::GAME_TYPE_RANKED, resulting_puuid,
                [&riot](const nlohmann::json& resulting_match_history) {
                    std::cout << "CLIENT succesfully got match history:" << std::endl << resulting_match_history.dump(4) << std::endl;

                    if (!resulting_match_history.empty()) {
                        if (!resulting_match_history[0].is_primitive() || !resulting_match_history[0].is_string()) {
                            std::cerr << "CLIENT expected resulting_match_history[0] to be string, but got something else" << std::endl;
                        } else {
                            std::cout << "CLIENT first match info:" << std::endl;
                            std::string match_id = resulting_match_history[0].get<std::string>();
                            riot.get_match_info_async(
                                riot_api::REGION_EUW, match_id,
                                [](const nlohmann::json& resulting_match_info) {
                                    std::cout << "CLIENT successfully got match info:" << std::endl << resulting_match_info.dump(4) << std::endl;
                                },
                                []() {
                                    std::cerr << "CLIENT failed to get match info" << std::endl;
                                }
                            );

                            std::cout << "CLIENT first match timeline:" << std::endl;
                            riot.get_match_timeline_async(
                                riot_api::REGION_EUW, match_id,
                                [](const nlohmann::json& resulting_match_timeline) {
                                    std::cout << "CLIENT successfully got match timeline:" << std::endl << resulting_match_timeline.dump(4) << std::endl;
                                },
                                []() {
                                    std::cerr << "CLIENT failed to get match timeline" << std::endl;
                                }
                            );
                        }
                    } else {
                        std::cout << "CLIENT match history was empty" << std::endl;
                    }


                },
                []() {
                    std::cerr << "CLIENT failed to get match history" << std::endl;
                }
            );

            riot.get_challenges_by_puuid_async(
                riot_api::REGION_EUW, resulting_puuid,
                [](const nlohmann::json& resulting_challenges_info_for_puuid) {
                    std::cout << "CLIENT successfully got challenges info for puuid:" << std::endl << resulting_challenges_info_for_puuid.dump(4) << std::endl;
                },
                []() {
                    std::cerr << "CLIENT failed to get challenges info for puuid" << std::endl;
                }
            );
        },
        []() {
            std::cerr << "CLIENT failed to get puuid" << std::endl;
        }
    );

    riot.get_challenges_info_async(
        riot_api::REGION_EUW,
        [](const nlohmann::json& resulting_challenges_info) {
            std::cout << "CLIENT successfully got challenges info:" << std::endl << resulting_challenges_info.dump(4) << std::endl;
        },
        []() {
            std::cerr << "CLIENT failed to get challenges info" << std::endl;
        }
    );

    get_liveclientdata_allgamedata_async(
        [](const nlohmann::json& resulting_live_client_data) {
            std::cerr << "CLIENT succesfully got live client data" << std::endl << resulting_live_client_data.dump(4) << std::endl;
        },
        []() {
            std::cerr << "CLIENT failed to get live client data" << std::endl;
        }
    );

    get_liveclientdata_activeplayer_async(
        [](const nlohmann::json& resulting_live_client_data) {
            std::cerr << "CLIENT succesfully got live client data" << std::endl << resulting_live_client_data.dump(4) << std::endl;
        },
        []() {
            std::cerr << "CLIENT failed to get live client data" << std::endl;
        }
    );

    return 0;
}
