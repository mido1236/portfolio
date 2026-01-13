add_library(football_core)

target_sources(football_core PRIVATE
        src/Match.cpp
        src/Player.cpp
        src/GameEvent.cpp
)

target_include_directories(football_core PUBLIC
        include
)
