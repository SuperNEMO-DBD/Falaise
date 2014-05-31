# - Find realine
#
#  Readline_INCLUDE_DIR - where to find readline headers
#  Readline_LIBRARIES   - List of libraries when using readline
#  Readline_FOUND       - True if readline is found

# message(STATUS "FindReadline: Entering...")
IF (Readline_INCLUDE_DIR)
  # Already in cache
  SET(Readline_FIND_QUIETLY TRUE)
ENDIF (Readline_INCLUDE_DIR)

FIND_PATH(Readline_INCLUDE_DIR readline/readline.h readline.h PATHS /usr)
FIND_LIBRARY(Readline_LIBRARIES readline PATHS /usr)

IF(Readline_INCLUDE_DIR AND Readline_LIBRARIES)
   SET(Readline_FOUND TRUE)
ELSE(Readline_INCLUDE_DIR AND Readline_LIBRARIES)
   SET(Readline_FOUND FALSE)
ENDIF (Readline_INCLUDE_DIR AND Readline_LIBRARIES)

IF (Readline_FOUND)
   IF (NOT Readline_FIND_QUIETLY)
      MESSAGE(STATUS "FindReadline: Found Readline library: ${Readline_LIBRARIES}")
      MESSAGE(STATUS "FindReadline: Found Readline headers: ${Readline_INCLUDE_DIR}")
   ENDIF (NOT Readline_FIND_QUIETLY)
ELSE (Readline_FOUND)
   IF (Readline_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "FindReadline: Could NOT find Readline")
   ENDIF (Readline_FIND_REQUIRED)
ENDIF (Readline_FOUND)

MARK_AS_ADVANCED(Readline_LIBRARIES Readline_INCLUDE_DIR)
