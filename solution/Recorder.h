#pragma once
#include <filesystem>
class CUserCmd;
class Vector;


class savingroute {
public:
    explicit savingroute(const char*) noexcept;
    void load(size_t) noexcept;
    void save(size_t, std::string name, std::string creator) const noexcept;
    void add(const char*) noexcept;
    void remove(size_t) noexcept;
    void rename(size_t, const char*) noexcept;
    //void reset() noexcept;
    void listRoutes() noexcept;

    constexpr auto& getRoutes() noexcept
    {
        return routes;
    }

private:
    std::filesystem::path path;
    std::vector<std::string> routes;
};

typedef savingroute;

inline std::unique_ptr<savingroute> route;

class movementRecorder {
public:
    void Draw();
    void Hook(CUserCmd* cmd, Vector unpredictpos);
    void DrawRoutes();
    void infowindow();
    void forcestop();
    //void saveRoute(const char* name) noexcept;
    //void loadRoute(const char* name) noexcept;
};

inline movementRecorder* MoveRecorder;

